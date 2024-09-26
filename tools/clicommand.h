#ifndef CLICOMMAND_H
#define CLICOMMAND_H

#include <QString>
#include <QStringList>
#include <QProcess>

class CliCommand
{
public:
    CliCommand();

    static QString execute(const QString &command);
    static QStringList executeAndSplit(const QString &command);
};

#endif // CLICOMMAND_H
