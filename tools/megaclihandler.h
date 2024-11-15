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

class MegaCLIHandler : public QObject
{
    Q_OBJECT
public:
    QList<MegaDisk> disks;
    void reload();
    explicit MegaCLIHandler(QObject *parent = nullptr);
    QList<MegaDisk> getDisks();
    void parserRaidMountInfo();
    QString findMountedPath(const QString &id);
    static QList<MegaCLIResponse> createRaid(const QList<QString> &diskParis, BlocksInfo *blksInfo);
    static MegaCLIResponse createRaid(const QString &raidArray, const QString &raidLevel, BlocksInfo *blksInfo);

    static QString createJbod(const QString &raidArray, BlocksInfo *blksInfo);

signals:

private:
    QStringList raidInfoLines;
    MegaDisk parseDiskInfo(const QStringList &lines);
};

#endif // MEGACLIHANDLER_H
