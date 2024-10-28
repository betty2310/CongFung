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
    explicit MegaCLIHandler(QObject *parent = nullptr);
    static QList<MegaDisk> getDisks();
    static QList<MegaCLIResponse> createRaid(const QList<QString> &diskParis, BlocksInfo *blksInfo);
    static MegaCLIResponse createRaid(const QString &raidArray, const QString &raidLevel, BlocksInfo *blksInfo);

signals:

private:
    static MegaDisk parseDiskInfo(const QStringList &lines);
};

#endif // MEGACLIHANDLER_H
