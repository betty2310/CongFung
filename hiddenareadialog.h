#ifndef HIDDENAREADIALOG_H
#define HIDDENAREADIALOG_H

#include <QDialog>
#include <QProcess>
#include <QRegularExpression>
#include <QDebug>

#include "structs/hiddenAreas.h"

namespace Ui
{
    class HiddenAreaDialog;
}

class HiddenAreaDialog : public QDialog
{
    Q_OBJECT

public:
    bool shouldShow;
    explicit HiddenAreaDialog(QWidget *parent = nullptr, QString path = "");
    HPAarea checkHpaArea();
    DCOarea checkDcoArea();
    ~HiddenAreaDialog();
    void runRemoveHpaScript();
    void setProgressBar(int value);

signals:
    void dialogClosed();
    void handledDiskArea();

protected:
    void closeEvent(QCloseEvent *event) override
    {
        QDialog::closeEvent(event);
        emit dialogClosed();
    }

private slots:
    void onScriptFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void handleProcessOutput();
    void on_okeButton_clicked();

private:
    Ui::HiddenAreaDialog *ui;
    QString path;
    QProcess *rescanHpaArea;
    HPAarea a;
    DCOarea d;
};

#endif // HIDDENAREADIALOG_H
