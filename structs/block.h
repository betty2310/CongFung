#ifndef BLOCK_H
#define BLOCK_H

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
    QString model;

    bool operator==(const Block &other) const
    {
        return path == other.path;
    }
};

#endif // BLOCK_H