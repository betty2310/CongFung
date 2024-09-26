#include "megaclihandler.h"

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

QString MegaCLIHandler::createRaid(const QList<QString> &diskPairs) {
    bool oneDiskOnly = diskPairs.count() == 1 ? true : false;
    bool isEvenDisk = diskPairs.count() % 2 == 0 ? false : true;

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

        }
    }

    QString result = CliCommand::execute(command);
    qDebug() << result;
    return result;
}

