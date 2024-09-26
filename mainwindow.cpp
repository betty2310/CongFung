#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , isRootUser(false)
    , settingDialog(new SettingDialog(this))
    , blkInfo(new BlocksInfo(this))
{
    ui->setupUi(this);

    ui->stackedWidget->setCurrentWidget(ui->dashboardPage);
    setupDashboardPage();
    setupWipePage();
    setupCreateForensicImagePage();

    isRootUser = getuid() ? false : true;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupDashboardPage() {
    ui->dashboardTable->setColumnCount(4);
    ui->dashboardTable->setHorizontalHeaderLabels({"Task ID", "Status"});

    updateDashboardTable();
}

void MainWindow::setupCreateForensicImagePage() {
    ui->sourceDiskTable->setColumnCount(4);
    ui->sourceDiskTable->setHorizontalHeaderLabels({"Name", "Path", "Size", "Type"});

    ui->destinationsDiskTable->setColumnCount(6);
    ui->destinationsDiskTable->setHorizontalHeaderLabels({"Id", "Slot number", "Enclosure ID", "Size", "Speed", "Name"});

    updateSourceDiskTable();
    updateDestinationDisksTable();
}

void MainWindow::updateSourceDiskTable() {
    QList<Block> blocks = blkInfo->getBlocksInfo();

    int displayRows = blocks.size();
    QList<Block> displayBlocks;

    for(int i = 0; i< blocks.size(); ++i) {

        if(blocks[i].tran == "nvme") { // mean os drive, can't modify
            displayRows--;
        } else if (blocks[i].tran == "") {  // logical disk: partition or raid?
            if(blocks[i].parent == nullptr) {
                displayRows--;
            } else {
                displayBlocks.append(blocks[i]);
            }
        } else {
            qDebug() << blocks[i].name;
            displayBlocks.append(blocks[i]);
        }

    }

    ui->sourceDiskTable->setRowCount(displayRows);

    for (int i = 0; i < displayBlocks.size(); ++i) {

        const Block &dp = displayBlocks[i];

        ui->sourceDiskTable->setItem(i, 0, new QTableWidgetItem(dp.name));
        ui->sourceDiskTable->setItem(i, 1, new QTableWidgetItem(dp.path));
        ui->sourceDiskTable->setItem(i, 2, new QTableWidgetItem(dp.size));
        ui->sourceDiskTable->setItem(i, 3, new QTableWidgetItem(dp.isPartition ? "Partition" : "Disk"));
    }

}


void MainWindow::updateDestinationDisksTable() {
    QList<MegaDisk> megaDisks = MegaCLIHandler::getDisks();

    int displayRows = megaDisks.size();
    QList<MegaDisk> displayMegaDisks;

    for(int i = 0; i < megaDisks.size(); ++i) {
        if(megaDisks[i].raidState == "Online") {
            displayRows--;
        } else {
            displayMegaDisks.append(megaDisks[i]);
        }

    }

    ui->destinationsDiskTable->setRowCount(displayRows);

    for(int i = 0; i < displayMegaDisks.count(); ++i) {

        ui->destinationsDiskTable->setItem(i, 0, new QTableWidgetItem(QString::number(displayMegaDisks[i].deviceId)));
        ui->destinationsDiskTable->setItem(i, 1, new QTableWidgetItem(QString::number(displayMegaDisks[i].slotNumber)));
        ui->destinationsDiskTable->setItem(i, 2, new QTableWidgetItem(QString::number(displayMegaDisks[i].enclosureDeviceId)));
        ui->destinationsDiskTable->setItem(i, 3, new QTableWidgetItem(displayMegaDisks[i].rawSize));
        ui->destinationsDiskTable->setItem(i, 4, new QTableWidgetItem(displayMegaDisks[i].deviceSpeed));
        ui->destinationsDiskTable->setItem(i, 5, new QTableWidgetItem(displayMegaDisks[i].inquiryData));
    }

    ui->destinationsDiskTable->setColumnWidth(5, 400);

}

void MainWindow::updateDashboardTable()
{

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

    QString taskId = QString("Wipe_%1").arg(QDateTime::currentDateTime().toString("yyymmmDDD__hhmmss"));

    qDebug() << path;

    QThread *workerThread = new QThread();
    Worker *worker = new Worker(taskId, path);
    worker->moveToThread(workerThread);

    // Add task to dashboard
    int rowCount = ui->dashboardTable->rowCount();
    ui->dashboardTable->insertRow(rowCount);
    ui->dashboardTable->setItem(rowCount, 0, new QTableWidgetItem(taskId));

    // Add Stop button
    QPushButton *stopButton = new QPushButton("Stop");
    ui->dashboardTable->setCellWidget(rowCount, 2, stopButton);
    connect(stopButton, &QPushButton::clicked, this, [this, taskId, worker, workerThread]() {
        this->onStopTaskClicked(taskId, worker, workerThread);
    });

    connect(workerThread, &QThread::started, worker, &Worker::process);
    connect(worker, &Worker::finished, this, &MainWindow::onWipeTaskFinished);
    connect(worker, &Worker::progressUpdate, this, &MainWindow::onWipeProgressUpdate);
    connect(workerThread, &QThread::finished, worker, &QObject::deleteLater);
    connect(workerThread, &QThread::finished, workerThread, &QObject::deleteLater);

    workerThread->start();
}

void MainWindow::onWipeTaskFinished(const QString &taskId, bool success)
{
    // Update dashboard with task completion status
    for (int i = 0; i < ui->dashboardTable->rowCount(); ++i) {
        if (ui->dashboardTable->item(i, 0)->text() == taskId) {
            ui->dashboardTable->setItem(i, 1, new QTableWidgetItem(success ? "Completed" : "Failed"));
            ui->dashboardTable->cellWidget(i, 2)->setEnabled(false);
            break;
        }
    }
}

void MainWindow::onWipeProgressUpdate(const QString &taskId, const QString &progress)
{
    // Update dashboard with task progress
    for (int i = 0; i < ui->dashboardTable->rowCount(); ++i) {
        if (ui->dashboardTable->item(i, 0)->text() == taskId) {
            ui->dashboardTable->setItem(i, 1, new QTableWidgetItem(progress));
            break;
        }
    }
}

void MainWindow::onStopTaskClicked(const QString &taskId, Worker *worker, QThread *workerThread)
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Confirm Stop Task",
                                  "Are you sure you want to stop this task?",
                                  QMessageBox::Yes|QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        worker->requestInterruption();
        workerThread->quit();
        workerThread->wait();

        // Update dashboard
        for (int i = 0; i < ui->dashboardTable->rowCount(); ++i) {
            if (ui->dashboardTable->item(i, 0)->text() == taskId) {
                ui->dashboardTable->setItem(i, 1, new QTableWidgetItem("Stopped"));
                ui->dashboardTable->cellWidget(i, 2)->setEnabled(false);
                break;
            }
        }
    }
}

void MainWindow::on_pushButton_clicked()
{
    updateWipeTable();
}


void MainWindow::on_sourceDiskTableReloadBtn_clicked()
{
    updateSourceDiskTable();
}


void MainWindow::on_createImageTaskBtn_clicked()
{
    QList<QTableWidgetItem*> selectedItems = ui->destinationsDiskTable->selectedItems();
    QList<int> selectedRows;

    for(const auto &item: selectedItems) {
        if(!selectedRows.contains(item->row())) {
            selectedRows.append(item->row());
        }
    }

    if(selectedRows.count() == 0) {
        QMessageBox::warning(this, "Invalid selection", "Please select at least on disk");
        return;
    }

    QList<QString> raidArrays;
    for(int i = 0; i < selectedRows.size(); ++i) {
        QString enclosureId = ui->destinationsDiskTable->item(selectedRows[i], 2)->text();
        QString slotId = ui->destinationsDiskTable->item(selectedRows[i], 1)->text();
        QString pd = QString("%1:%2").arg(enclosureId).arg(slotId);
        raidArrays.append(pd);
    }

    QString result = MegaCLIHandler::createRaid(raidArrays);
    updateDestinationDisksTable();
}

