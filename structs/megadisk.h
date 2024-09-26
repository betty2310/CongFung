#ifndef MEGADISK_H
#define MEGADISK_H

#include <QString>

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
