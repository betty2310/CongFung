#ifndef BLOCKSINFO_H
#define BLOCKSINFO_H

#include "../structs/block.h"

#include <QObject>
#include <QList>
#include <QString>

class BlocksInfo : public QObject
{
    Q_OBJECT
public:
    explicit BlocksInfo(QObject *parent = nullptr);
    QList<Block> getBlocksInfo();
    QList<Block> getDisks();

signals:

private:
    QList<Block> parseLsblkOutput(const QString &output);
};

#endif // BLOCKSINFO_H
