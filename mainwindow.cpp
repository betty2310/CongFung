#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_st1_btn_clicked()
{
    qDebug() << "button 1 clicked!";
    ui->stackedWidget->setCurrentWidget(ui->page_2);
}


void MainWindow::on_st2_btn_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->page);
}

