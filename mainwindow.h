#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QDialog>

#include "settingdialog.h"

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

private:
    Ui::MainWindow *ui;
    SettingDialog *settingDialog;
};
#endif // MAINWINDOW_H
