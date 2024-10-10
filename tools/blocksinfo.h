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
    QString fstype;
    bool isPartition;
    bool isBusy;
    Block *parent;

    bool operator==(const Block &other) const {
        return path == other.path;
    }
};

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
