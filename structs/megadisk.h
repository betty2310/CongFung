#ifndef MEGADISK_H
#define MEGADISK_H

#include <QString>

struct MegaCLIResponse {
    QList<QString> path;
    QString pdNumber;
};

struct MegaDisk {
    int enclosureDeviceId;
    int slotNumber;
    int deviceId;
    QString rawSize;
    QString deviceSpeed;
    QString inquiryData;
    QString raidState;
};

#endif // MEGADISK_H
