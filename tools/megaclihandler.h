#ifndef MEGACLIHANDLER_H
#define MEGACLIHANDLER_H

#include <QObject>
#include <QList>
#include <QDebug>
#include <QThread>

#include "../structs/block.h"
#include "../structs/megadisk.h"
#include "blocksinfo.h"
#include "clicommand.h"

// 252:4 -> VD0 -> /dev/sdb -> /dev/sdb1 -> /mnt/sdb1
enum RaidArrayInfo
{
    PD,
    VD,
    BLOCK,
    PARTITION,
    MOUNTPOINT
};

class MegaCLIHandler : public QObject
{
    Q_OBJECT
public:
    QList<MegaDisk> disks;
    void reload();
    explicit MegaCLIHandler(QObject *parent = nullptr);
    QList<MegaDisk> getDisks();
    void parserRaidMountInfo();
    QString extractLDRaidInfo(const QString &id, RaidArrayInfo infoType);
    QList<MegaCLIResponse> createRaid(const QList<QString> &diskParis, BlocksInfo *blksInfo);
    MegaCLIResponse createRaid(const QString &raidArray, const QString &raidLevel, BlocksInfo *blksInfo);

    static QString createJbod(const QString &raidArray, BlocksInfo *blksInfo);

signals:

private:
    QStringList raidInfoLines;
    MegaDisk parseDiskInfo(const QStringList &lines);
};

#endif // MEGACLIHANDLER_H
