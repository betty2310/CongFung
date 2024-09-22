#include "blocksinfo.h"

#include <QProcess>
#include <QDebug>

BlocksInfo::BlocksInfo(QObject *parent)
    : QObject{parent}
{}


QList<Block> BlocksInfo::getBlocksInfo() {
    QProcess process;
    process.start("lsblk", QStringList() << "-no" << "NAME,PATH,SIZE,TYPE");
    process.waitForFinished();
    QString output = process.readAllStandardOutput();

    return parseLsblkOutput(output);
}

QList<Block> BlocksInfo::parseLsblkOutput(const QString &output)
{
    QList<Block> blocks;
    QStringList lines = output.split("\n", Qt::SkipEmptyParts);

    for (const QString &line : lines) {
        QStringList parts = line.split(" ", Qt::SkipEmptyParts);
        if (parts.size() >= 4) {
            Block blk;
            blk.name = parts[0];
            blk.path = parts[1];
            blk.size = parts[2];
            if(parts[3] == "loop") {
                continue;
            }
            blk.isPartition = (parts[3] == "part");
            blocks.append(blk);
        }
    }

    return blocks;
}
