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

QList<Block> BlocksInfo::getDisks() {
    QList<Block> blocks;

    QProcess process;
    process.start("lsblk", QStringList() << "-ndo" << "PATH");
    process.waitForFinished();
    QString output = process.readAllStandardOutput();
    QStringList lines = output.split("\n", Qt::SkipEmptyParts);
    for (const QString &line : lines) {
        Block blk;
        blk.path = line.trimmed();
        blocks.append(blk);
    }
    return blocks;
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
            blk.parent = nullptr;
            if(parts[3] == "loop") {
                continue;
            }
            blk.isPartition = parts[3] == "part" ? true: false;
            blk.isBusy = false;
            blocks.append(blk);
        }
    }

    for(int i = 0; i < blocks.count(); ++i) {
        for(int j = 0; j < blocks.count(); ++j) {
            if(blocks[i].path.contains(blocks[j].name) && blocks[i].name != blocks[j].name) {
                blocks[i].parent = &blocks[j];
            }
        }
    }

    QProcess tranProcess;
    tranProcess.start("lsblk", QStringList() << "-no" << "NAME,TRAN");
    tranProcess.waitForFinished();
    QString tranOutput = tranProcess.readAllStandardOutput();
    QStringList tranLines = tranOutput.split("\n", Qt::SkipEmptyParts);

    for (const QString &line : tranLines) {
        QStringList parts = line.split(" ", Qt::SkipEmptyParts);
        if (parts.size() >= 2) {
            for(auto &blk : blocks) {
                if(blk.name == parts[0]) {
                    blk.tran = parts[1];
                }
            }
        }
    }

    QProcess fstypeProcess;
    fstypeProcess.start("lsblk", QStringList() << "-no" << "NAME,FSTYPE");
    fstypeProcess.waitForFinished();
    QString fstypeOutput = fstypeProcess.readAllStandardOutput();
    QStringList fstypeLines = fstypeOutput.split("\n", Qt::SkipEmptyParts);

    for (const QString &line : fstypeLines) {
        QStringList parts = line.split(" ", Qt::SkipEmptyParts);
        if (parts.size() >= 2) {
            for(auto &blk : blocks) {
                if(blk.name == parts[0]) {
                    blk.fstype = parts[1];
                }
            }
        }
    }

    QProcess modelProcess;
    modelProcess.start("lsblk", QStringList() << "-no" << "NAME,MODEL");
    modelProcess.waitForFinished();
    QString modelString = modelProcess.readAllStandardOutput();
    QStringList modelStringLines = modelString.split("\n", Qt::SkipEmptyParts);

    for (const QString &line : modelStringLines) {
        QStringList parts = line.split(" ", Qt::SkipEmptyParts);
        if (parts.size() >= 2) {
            for(auto &blk : blocks) {
                if(blk.name == parts[0]) {
                    blk.model = parts[1];
                }
            }
        }
    }
    return blocks;
}
