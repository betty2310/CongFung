#ifndef BLOCKSINFO_H
#define BLOCKSINFO_H

#include <QObject>
#include <QList>
#include <QString>

struct Block
{
    QString name;
    QString path;
    QString size;
    QString tran;
    bool isPartition;
    Block *parent;
};

class BlocksInfo : public QObject
{
    Q_OBJECT
public:
    explicit BlocksInfo(QObject *parent = nullptr);
    QList<Block> getBlocksInfo();

signals:

private:
    QList<Block> parseLsblkOutput(const QString &output);
};

#endif // BLOCKSINFO_H
