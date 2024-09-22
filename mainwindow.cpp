#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , settingDialog(nullptr)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::on_createForensicImageBtn_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    ui->createForensicImageBtn->setChecked(true);
}


void MainWindow::on_cloneDiskBtn_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
    ui->cloneDiskBtn->setChecked(true);
}


void MainWindow::on_wipeBtn_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
    ui->wipeBtn->setChecked(true);
}


void MainWindow::on_settingsBtn_clicked()
{
    if (!settingDialog) {
        settingDialog = new SettingDialog(this);
    }
    settingDialog->show();
}

