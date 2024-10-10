#include "megaclihandler.h"

#include "blocksinfo.h"

MegaCLIHandler::MegaCLIHandler(QObject *parent)
    : QObject{parent}
{}

QList<MegaDisk> MegaCLIHandler::getDisks() {
    QList<MegaDisk> disks;
    QString command = "megacli -pdList -a0 | grep -E 'Id|Slot|Enclosure Device|Inquiry|Raw|Device Speed|Firmware state'";
    QStringList output = CliCommand::executeAndSplit(command);

    for (int i = 0; i < output.size(); i += 7) {
        if (i + 6 < output.size()) {
            disks.append(parseDiskInfo(output.mid(i, 7)));
        }
    }
    return disks;
}

MegaDisk MegaCLIHandler::parseDiskInfo(const QStringList &lines) {
    MegaDisk disk;
    disk.enclosureDeviceId = lines[0].split(':').last().trimmed().toInt();
    disk.slotNumber = lines[1].split(':').last().trimmed().toInt();
    disk.deviceId = lines[2].split(':').last().trimmed().toInt();
    disk.rawSize = lines[3].split(':').last().trimmed();
    disk.rawSize = disk.rawSize.split('[').first().trimmed();
    disk.raidState = lines[4].split(':').last().trimmed();
    disk.raidState = disk.raidState.split(',').first().trimmed();
    disk.inquiryData = lines[5].split(':').last().trimmed();
    disk.deviceSpeed = lines[6].split(':').last().trimmed();

    return disk;
}

QList<QString> MegaCLIHandler::createRaid(const QList<QString> &diskPairs, BlocksInfo *blksInfo) {
    bool oneDiskOnly = diskPairs.count() == 1 ? true : false;
    bool isEvenDisk = diskPairs.count() % 2 == 0 ? false : true;
    QList<Block> oldDisks = blksInfo->getDisks();

    QString command;
    if(oneDiskOnly) {
       command = "megacli -cfgldadd -r0 [" + diskPairs[0] + "] -a0";
    } else {
        if(!isEvenDisk) {
            QString args;
            for(auto &disk : diskPairs) {
                args.append(disk);
                args.append(",");
            }
            args = args.sliced(0, args.length() - 1);
            command = "megacli -cfgldadd -r1 [" + args + "] -a0";
        } else {
            // create raid 1 from odd disks
            QString args;
            for(int i = 0; i < diskPairs.count() - 1; ++i) {
                args.append(diskPairs[i]);
                args.append(",");
            }
            args = args.sliced(0, args.length() - 1);
            QString createRaid1Command = "megacli -cfgldadd -r1 [" + args + "] -a0";
            QString createRaid0Command = "megacli -cfgldadd -r0 [" + diskPairs.last() + "] -a0";

            CliCommand::execute(createRaid1Command);
            CliCommand::execute(createRaid0Command);
        }
    }
    qDebug() << "MegaCLIHandler::createRaid command: " << command;
    QString result = CliCommand::execute(command);
    qDebug() << "MegaCLIHandler::createRaid result: " << result;
    QList<QString> output;
    QList<Block> newDisks = blksInfo->getDisks();
    for(auto const &disk : newDisks) {
        if(!oldDisks.contains(disk)) {
            output.append(disk.path);
        }
    }
    qDebug() << "The associated disk of created raid array is: " << output.at(0);
    return output;
}

