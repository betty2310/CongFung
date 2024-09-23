#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , settingDialog(new SettingDialog(this))
    , blkInfo(new BlocksInfo(this))
{
    ui->setupUi(this);

    ui->stackedWidget->setCurrentWidget(ui->dashboardPage);

    qDebug() << "Application run on thread:" << QThread::currentThread();

    setupDashboardPage();
    setupWipePage();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupDashboardPage() {
    ui->dashboardTable->setColumnCount(1);
    ui->dashboardTable->setHorizontalHeaderLabels({"Task ID"});

    updateDashboardTable();
}

void MainWindow::updateDashboardTable()
{
    // For now, we'll just add a dummy task ID
    int rowCount = ui->dashboardTable->rowCount();
    ui->dashboardTable->insertRow(rowCount);
    ui->dashboardTable->setItem(rowCount, 0, new QTableWidgetItem(QString("Task %1").arg(rowCount + 1)));
}

void MainWindow::setupWipePage() {
    ui->wipeTable->setColumnCount(5);
    ui->wipeTable->setHorizontalHeaderLabels({"Name", "Path", "Size", "Type", "Tran"});
    updateWipeTable();
}

void MainWindow::updateWipeTable()
{
    QList<Block> blocks = blkInfo->getBlocksInfo();

    int displayRows = blocks.size();

    for(int i = 0; i< blocks.size(); ++i) {
        if(blocks[i].tran == "nvme") {
            displayRows--;
        }
    }

    ui->wipeTable->setRowCount(displayRows);

    for (int i = 0; i < blocks.size(); ++i) {
        const Block &dp = blocks[i];
        if(dp.tran == "nvme") {
            continue;
        }
        ui->wipeTable->setItem(i, 0, new QTableWidgetItem(dp.name));
        ui->wipeTable->setItem(i, 1, new QTableWidgetItem(dp.path));
        ui->wipeTable->setItem(i, 2, new QTableWidgetItem(dp.size));
        ui->wipeTable->setItem(i, 3, new QTableWidgetItem(dp.isPartition ? "Partition" : "Disk"));
        ui->wipeTable->setItem(i, 4, new QTableWidgetItem(dp.tran));

    }
}

void MainWindow::on_dashboardBtn_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->dashboardPage);
    ui->dashboardBtn->setChecked(true);
    ui->createForensicImageBtn->setChecked(false);
    ui->wipeBtn->setChecked(false);
    ui->cloneDiskBtn->setChecked(false);
}


void MainWindow::on_createForensicImageBtn_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->forensicImagePage);
    ui->createForensicImageBtn->setChecked(true);
    ui->wipeBtn->setChecked(false);
    ui->cloneDiskBtn->setChecked(false);
    ui->dashboardBtn->setChecked(false);
}


void MainWindow::on_cloneDiskBtn_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->cloneDiskPage);
    ui->cloneDiskBtn->setChecked(true);
    ui->createForensicImageBtn->setChecked(false);
    ui->wipeBtn->setChecked(false);
    ui->dashboardBtn->setChecked(false);
}


void MainWindow::on_wipeBtn_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->wipeDiskPage);
    ui->wipeBtn->setChecked(true);
    ui->cloneDiskBtn->setChecked(false);
    ui->createForensicImageBtn->setChecked(false);
    ui->dashboardBtn->setChecked(false);
}


void MainWindow::on_settingsBtn_clicked()
{
    if (!settingDialog) {
        settingDialog = new SettingDialog(this);
    }
    settingDialog->show();
}




void MainWindow::on_runWipeBtn_clicked()
{
    QList<QTableWidgetItem*> selectedItems = ui->wipeTable->selectedItems();
    if (selectedItems.isEmpty()) {
        QMessageBox::warning(this, "No Selection", "Please select a disk or partition to wipe.");
        return;
    }

    QString path = ui->wipeTable->item(selectedItems[0]->row(), 1)->text();

    qDebug() << path;

    QThread *wipeThread = QThread::create([this, path]() {

        QProcess process;

        // Connect signals to capture output
        connect(&process, &QProcess::readyReadStandardOutput, [&process]() {
            QString output = QString::fromUtf8(process.readAllStandardOutput()).trimmed();
            qDebug() << "DD Progress:" << output;
        });

        connect(&process, &QProcess::readyReadStandardError, [&process]() {
            QString error = QString::fromUtf8(process.readAllStandardError()).trimmed();
            qDebug() << "DD Error:" << error;
        });


        process.start("pkexec", QStringList() << "dd" << "if=/dev/zero" << QString("of=%1").arg(path) << "bs=4M" << "status=progress");
        qDebug() << "DD process started in thread:" << QThread::currentThread();
        process.waitForFinished(-1);

        if (process.exitStatus() == QProcess::NormalExit) {
            int exitCode = process.exitCode();
            if (exitCode == 0) {
                qDebug() << "dd command executed successfully";
            } else {
                qDebug() << "dd command failed with exit code:" << exitCode;
            }
        } else {
            qDebug() << "dd command crashed or was terminated";
        }


    });
    connect(wipeThread, &QThread::finished, wipeThread, &QThread::deleteLater);
    wipeThread->start();

    updateDashboardTable();
}

