#ifndef MEGADISK_H
#define MEGADISK_H

#include <QString>

struct MegaCLIResponse
{
    QString path = "";
    QString pdNumber;
};

struct MegaDisk
{
    int enclosureDeviceId;
    int slotNumber;
    int deviceId;
    int raidLevel;
    int pdNumber;
    QString rawSize;
    QString deviceSpeed;
    QString inquiryData;
    QString raidState;
    QString path;
    QString mountedPath;
};

#endif // MEGADISK_H
