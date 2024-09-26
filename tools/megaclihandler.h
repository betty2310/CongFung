#ifndef MEGACLIHANDLER_H
#define MEGACLIHANDLER_H

#include <QObject>
#include <QList>
#include <QDebug>

#include "../structs/megadisk.h"
#include "clicommand.h"

class MegaCLIHandler : public QObject
{
    Q_OBJECT
public:
    explicit MegaCLIHandler(QObject *parent = nullptr);
    static QList<MegaDisk> getDisks();

signals:

private:
    static MegaDisk parseDiskInfo(const QStringList &lines);
};

#endif // MEGACLIHANDLER_H