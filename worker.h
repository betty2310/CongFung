#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include <QProcess>

class Worker : public QObject
{
    Q_OBJECT

public:
    explicit Worker(const QString &taskId, const QString &path, QObject *parent = nullptr);

public slots:
    void process();
    void requestInterruption();

signals:
    void finished(const QString &taskId, bool success);
    void progressUpdate(const QString &taskId, const QString &progress);

private:
    QString m_taskId;
    QString m_path;
    QProcess *m_process;
    bool m_interrupted;
};

#endif // WORKER_H
