#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QDialog>
#include <QMessageBox>
#include <QProcess>
#include <QThread>

#include "settingdialog.h"
#include "tools/blocksinfo.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void on_createForensicImageBtn_clicked();

    void on_cloneDiskBtn_clicked();

    void on_wipeBtn_clicked();

    void on_settingsBtn_clicked();

    void on_dashboardBtn_clicked();

    void on_runWipeBtn_clicked();

private:
    Ui::MainWindow *ui;
    SettingDialog *settingDialog;
    BlocksInfo *blkInfo;

    void setupWipePage();
    void setupDashboardPage();
    void updateWipeTable();
    void updateDashboardTable();
};
#endif // MAINWINDOW_H
