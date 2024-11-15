#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QDialog>
#include <QMessageBox>
#include <QProcess>
#include <QThread>
#include <QDateTime>
#include <QTableWidgetItem>
#include <unistd.h>

#include "settingdialog.h"
#include "hiddenareadialog.h"
#include "tools/blocksinfo.h"
#include "tools/megaclihandler.h"
#include "worker.h"
#include "structs/task.h"
#include "tools/task_metadata.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;
}
QT_END_NAMESPACE

struct SourceDiskInfo
{
    QString name;
    QString path;
    QString pd;
    QString raidState;
} typedef SourceDiskInfo;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void prepareMega();
    HiddenAreaDialog *hiddenAreaDialog;
    MegaDisk findMegaDiskBySlot(int slotNumber);
    MegaDisk findMegaDiskByName(QString name);

private slots:

    void on_createForensicImageBtn_clicked();

    void on_cloneDiskBtn_clicked();

    void on_wipeBtn_clicked();

    void on_settingsBtn_clicked();

    void on_dashboardBtn_clicked();

    void on_runWipeBtn_clicked();

    void parseWipeOutput(QProcess *process, const QString &taskName);

    void onWipeTaskFinished(const QString &taskName, QString path, bool success);

    void stopWipeProcess(QProcess *process, const QString &taskName, QString path);

    void on_pushButton_clicked();

    void on_sourceDiskTableReloadBtn_clicked();

    QList<MegaDisk> seletectedRowsToMegadisks();

    void parseCreateImageTaskOutput(QProcess *process, const Task &task);

    void stopCreateImageTaskProcess(QProcess *process, const Task &task);

    SourceDiskInfo getSourceDiskNameFromSourceTable();

    void on_createImageTaskBtn_clicked();

    void on_destinationDiskTableReloadBtn_clicked();

private:
    Ui::MainWindow *ui;
    bool isRootUser;
    SettingDialog *settingDialog;
    BlocksInfo *blkInfo;
    QList<Block> blocks;
    QList<Task> tasks;
    MegaCLIHandler *megaCliHandler;

    void setupWipePage();
    void setupDashboardPage();
    void setupCreateForensicImagePage();
    void updateWipeTable();
    void updateDashboardTable();
    void updateSourceDiskTable();
    void updateDestinationDisksTable();
    Task makeTask(SourceDiskInfo &sourceDisk, QString &destination, QString mountedPath, QString imageName, int command, QString pdNumber);

    bool makeRaidArrayFromDestinationDicks(QList<MegaDisk> destinationDisks, SourceDiskInfo sourceDisk);

    void makeJbodFromSourceDisk(SourceDiskInfo &info, bool &retFlag);

    void cleanRaid(const Task &task);

    void cleanRaid();

    void handleCreateImageTask();

    void handleCreateImageTask(Task &task);

    void writeTaskMetadata(const Task &task, bool success);

    void onCreateImageTaskFinished(const Task &task, bool success);
};
#endif // MAINWINDOW_H
