#include "worker.h"
#include <QProcess>
#include <QDebug>

Worker::Worker(const QString &taskId, const QString &path, QObject *parent)
    : QObject(parent), m_taskId(taskId), m_path(path), m_process(nullptr) {}

void Worker::process()
{
    m_process = new QProcess(this);

    connect(m_process, &QProcess::readyReadStandardOutput, [this]() {
        QString output = QString::fromUtf8(m_process->readAllStandardOutput()).trimmed();
        qDebug() << "DD Progress:" << output;
        emit progressUpdate(m_taskId, output);
    });

    connect(m_process, &QProcess::readyReadStandardError, [this]() {
        QString error = QString::fromUtf8(m_process->readAllStandardError()).trimmed();
        qDebug() << "DD Error:" << error;
        emit progressUpdate(m_taskId, error);
    });

    m_process->start("dd", QStringList() << "if=/dev/zero" << QString("of=%1").arg(m_path) << "bs=4M" << "status=progress");

    if (!m_process->waitForFinished(-1)) {
        if (m_process->error() == QProcess::FailedToStart) {
            qDebug() << "Process failed to start:" << m_process->errorString();
            emit finished(m_taskId, false);
        } else if (m_process->state() == QProcess::NotRunning) {
            qDebug() << "Process was interrupted";
            emit finished(m_taskId, false);
        }
        return;
    }

    if (m_process->exitCode() != 0) {
        qDebug() << "Process exited with code:" << m_process->exitCode();
        emit finished(m_taskId, false);
        return;
    }

    emit finished(m_taskId, true);
}

void Worker::requestInterruption()
{
    if (m_process && m_process->state() == QProcess::Running) {
        m_process->kill();
    }
}
