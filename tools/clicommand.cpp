#include "clicommand.h"

CliCommand::CliCommand() {}

QString CliCommand::execute(const QString &command) {
    QProcess process;
    process.start("sh", QStringList() << "-c" << command);
    process.waitForFinished(-1);
    return process.readAllStandardOutput();
}

QStringList CliCommand::executeAndSplit(const QString &command) {
    return execute(command).split('\n', Qt::SkipEmptyParts);
}
