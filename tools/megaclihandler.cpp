#include "megaclihandler.h"

#include <qregularexpression.h>

#include "blocksinfo.h"

MegaCLIHandler::MegaCLIHandler(QObject *parent)
    : QObject{parent}
{
    reload();
}

void MegaCLIHandler::reload()
{
    this->disks.clear();
    parserRaidMountInfo();
    getDisks();
}

QList<MegaDisk> MegaCLIHandler::getDisks()
{
    QString command = "megacli -pdList -a0 | grep -E 'Id|Slot|Enclosure Device|Inquiry|Raw|Device Speed|Firmware state'";
    QStringList output = CliCommand::executeAndSplit(command);

    for (int i = 0; i < output.size(); i += 7)
    {
        if (i + 6 < output.size())
        {
            MegaDisk disk = parseDiskInfo(output.mid(i, 7));
            disk.pdNumber = extractLDRaidInfo(QString("%1:%2").arg(disk.enclosureDeviceId).arg(disk.slotNumber), VD).toInt();
            disks.append(disk);
        }
    }
    return this->disks;
}

void MegaCLIHandler::parserRaidMountInfo()
{
    QProcess process;
    process.start("raid-mount-info");
    process.waitForFinished();
    QString output = process.readAllStandardOutput();
    QStringList lines = output.split("\n", Qt::SkipEmptyParts);
    raidInfoLines = lines;
}

QString MegaCLIHandler::extractLDRaidInfo(const QString &id, RaidArrayInfo infoType)
{
    QString res = "";
    for (const QString &line : raidInfoLines)
    {
        QStringList parts = line.split("->", Qt::SkipEmptyParts);
        if (parts.size() >= 3)
        {
            /*
            252:2 -> VD0 -> /dev/sda -> /dev/sda1 -> /mnt/sda1
            252:0 -> VD1 -> /dev/sdd -> /dev/sdd1 -> Not mounted
            */
            QString ld = parts[1].trimmed();
            switch (infoType)
            {
            case PD:
                /* code */
                break;
            case VD:
                if (ld == id)
                {
                    res = parts[1].trimmed();
                }
                break;
            case MOUNTPOINT:
                if (ld == id)
                {
                    qDebug() << "mounted path" << parts[4];
                    if (parts[4] == " Not mounted")
                    {
                        res = "";
                    }
                    else
                    {
                        res = parts[4].trimmed();
                    }
                    qDebug() << "Result: " << res;
                }
            case BLOCK:
                if (ld == id)
                {
                    res = parts[2].trimmed();
                }
                break;
            default:
                res = "";
                break;
            }
        }
    }
    return res;
}

MegaDisk MegaCLIHandler::parseDiskInfo(const QStringList &lines)
{
    MegaDisk disk;
    disk.raidLevel = -1;
    disk.enclosureDeviceId = lines[0].split(':').last().trimmed().toInt();
    disk.slotNumber = lines[1].split(':').last().trimmed().toInt();
    disk.deviceId = lines[2].split(':').last().trimmed().toInt();
    disk.rawSize = lines[3].split(':').last().trimmed();
    disk.rawSize = disk.rawSize.split('[').first().trimmed();
    disk.raidState = lines[4].split(':').last().trimmed();
    disk.raidState = disk.raidState.split(',').first().trimmed();
    disk.inquiryData = lines[5].split(':').last().trimmed();
    disk.deviceSpeed = lines[6].split(':').last().trimmed();
    QString pd = QString("%1:%2").arg(disk.enclosureDeviceId).arg(disk.slotNumber);
    disk.mountedPath = extractLDRaidInfo(pd, MOUNTPOINT);
    return disk;
}

QList<MegaCLIResponse> MegaCLIHandler::createRaid(const QList<QString> &diskPairs, BlocksInfo *blksInfo)
{
    QList<MegaCLIResponse> responses;
    const bool oneDiskOnly = diskPairs.count() == 1;
    const bool isEvenDisk = diskPairs.count() % 2 != 0;

    if (oneDiskOnly)
    {
        MegaCLIResponse response = createRaid(diskPairs[0], "0", blksInfo);
        responses.append(response);
    }
    else
    {
        if (!isEvenDisk)
        {
            QString args;
            for (auto &disk : diskPairs)
            {
                args.append(disk);
                args.append(",");
            }
            args = args.sliced(0, args.length() - 1);
            MegaCLIResponse response = createRaid(args, "1", blksInfo);
            responses.append(response);
        }
        else
        {
            // create raid 1 from odd disks
            QString args;
            for (int i = 0; i < diskPairs.count() - 1; ++i)
            {
                args.append(diskPairs[i]);
                args.append(",");
            }
            args = args.sliced(0, args.length() - 1);
            MegaCLIResponse response1 = createRaid(args, "1", blksInfo);
            MegaCLIResponse response2 = createRaid(diskPairs.last(), "0", blksInfo);
            responses.append(response1);
            responses.append(response2);
        }
    }
    reload();
    return responses;
}

MegaCLIResponse MegaCLIHandler::createRaid(const QString &raidArray, const QString &raidLevel, BlocksInfo *blksInfo)
{
    MegaCLIResponse response;
    QString command = QString("megacli -cfgldadd -r%1 [%2] -a0").arg(raidLevel).arg(raidArray);

    qDebug() << "[MegaCLIHandler::createRaid command] " << command;
    QString result = CliCommand::execute(command);
    qDebug() << "[MegaCLIHandler::createRaid result] " << result;
    QRegularExpression vdRegex(R"(VD\s+(\d+))");
    QRegularExpressionMatch match = vdRegex.match(result);

    if (match.hasMatch())
    {
        QString vdNumber = match.captured(1); // Capture the number after "VD"
        qDebug() << "Virtual Disk Number:" << vdNumber;
        response.pdNumber = vdNumber;
    }
    else
    {
        qDebug() << "No match found";
    }
    // TODO: make sure wait raid command to be created
    QThread::msleep(200);
    parserRaidMountInfo();
    response.path = extractLDRaidInfo(response.pdNumber, BLOCK);
    if (response.path != "")
    {
        qDebug() << "The associated path of this raid array is: " << response.path;
    }
    return response;
}

QString MegaCLIHandler::createJbod(const QString &raidArray, BlocksInfo *blksInfo)
{
    QList<Block> oldDisks = blksInfo->getDisks();

    QString response;
    QString command = QString("make_jbod %1").arg(raidArray);

    qDebug() << "[MegaCLIHandler::createJBOD command] " << command;
    QString result = CliCommand::execute(command);
    qDebug() << "[MegaCLIHandler::createJBOD result] " << result;

    QThread::msleep(200);
    QList<Block> newDisks = blksInfo->getDisks();

    for (auto const &disk : newDisks)
    {
        if (!oldDisks.contains(disk))
        {
            response = disk.path;
        }
    }
    if (response != "")
    {
        qDebug() << "The associated path of this disk is: " << response;
    }
    return response;
}
