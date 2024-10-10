#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), isRootUser(false), settingDialog(new SettingDialog(this)), blkInfo(new BlocksInfo(this))
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

void MainWindow::setupDashboardPage()
{
    ui->dashboardTable->setColumnCount(5);
    ui->dashboardTable->setHorizontalHeaderLabels({"Task ID", "Status"});

    updateDashboardTable();
}

void MainWindow::setupCreateForensicImagePage()
{
    ui->sourceDiskTable->setColumnCount(4);
    ui->sourceDiskTable->setHorizontalHeaderLabels({"Name", "Path", "Size", "Type"});

    ui->destinationsDiskTable->setColumnCount(6);
    ui->destinationsDiskTable->setHorizontalHeaderLabels({"Id", "Slot number", "Enclosure ID", "Size", "Speed", "Name"});

    updateSourceDiskTable();
    updateDestinationDisksTable();
}

void MainWindow::updateSourceDiskTable()
{
    blocks = blkInfo->getBlocksInfo();

    int displayRows = blocks.size();
    QList<Block> displayBlocks;

    for (int i = 0; i < blocks.size(); ++i)
    {
        if (blocks[i].tran == "nvme")
        { // mean os drive, can't modify
            displayRows--;
        }
        else if (blocks[i].tran == "")
        { // logical disk: partition or raid?
            if (blocks[i].parent == nullptr || blocks[i].parent->tran == "")
            {
                displayRows--;
            }
            else
            {
                displayBlocks.append(blocks[i]);
            }
        }
        else
        {
            displayBlocks.append(blocks[i]);
        }
    }

    ui->sourceDiskTable->setRowCount(displayRows);

    for (int i = 0; i < displayBlocks.size(); ++i)
    {

        const Block &dp = displayBlocks[i];

        ui->sourceDiskTable->setItem(i, 0, new QTableWidgetItem(dp.name));
        ui->sourceDiskTable->setItem(i, 1, new QTableWidgetItem(dp.path));
        ui->sourceDiskTable->setItem(i, 2, new QTableWidgetItem(dp.size));
        ui->sourceDiskTable->setItem(i, 3, new QTableWidgetItem(dp.isPartition ? "Partition" : "Disk"));
    }
}

void MainWindow::updateDestinationDisksTable()
{
    QList<MegaDisk> megaDisks = MegaCLIHandler::getDisks();

    int displayRows = megaDisks.size();
    QList<MegaDisk> displayMegaDisks;

    for (int i = 0; i < megaDisks.size(); ++i)
    {
        if (megaDisks[i].raidState == "Online")
        {
            displayRows--;
        }
        else
        {
            displayMegaDisks.append(megaDisks[i]);
        }
    }

    ui->destinationsDiskTable->setRowCount(displayRows);

    for (int i = 0; i < displayMegaDisks.count(); ++i)
    {

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

void MainWindow::setupWipePage()
{
    ui->wipeTable->setColumnCount(5);
    ui->wipeTable->setHorizontalHeaderLabels({"Name", "Path", "Size", "Type", "Tran"});
    blocks = blkInfo->getBlocksInfo();
    updateWipeTable();
}

void MainWindow::updateWipeTable()
{
    int displayRows = blocks.size();

    for (int i = 0; i < blocks.size(); ++i)
    {
        if (blocks[i].tran == "nvme")
        {
            displayRows--;
        }
    }

    ui->wipeTable->setRowCount(displayRows);

    for (int i = 0; i < blocks.size(); ++i)
    {
        const Block &dp = blocks[i];
        if (dp.tran == "nvme")
        {
            continue;
        }
        ui->wipeTable->setItem(i, 0, new QTableWidgetItem(dp.name));
        ui->wipeTable->setItem(i, 1, new QTableWidgetItem(dp.path));
        ui->wipeTable->setItem(i, 2, new QTableWidgetItem(dp.size));
        ui->wipeTable->setItem(i, 3, new QTableWidgetItem(dp.isPartition ? "Partition" : "Disk"));
        ui->wipeTable->setItem(i, 4, new QTableWidgetItem(dp.tran));

        if(dp.isBusy) {
            for(int col = 0; col < ui->wipeTable->columnCount(); ++col) {
                QTableWidgetItem *item = ui->wipeTable->item(i, col);
                item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
                item->setForeground(QBrush(Qt::gray));
                item->setBackground(QBrush(QColor(245, 245, 245)));
            }
        }
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
    if (!settingDialog)
    {
        settingDialog = new SettingDialog(this);
    }
    settingDialog->show();
}

void MainWindow::on_runWipeBtn_clicked()
{
    QList<QTableWidgetItem *> selectedItems = ui->wipeTable->selectedItems();
    if (selectedItems.isEmpty())
    {
        QMessageBox::warning(this, "No Selection", "Please select a disk or partition to wipe.");
        return;
    }

    QString path = ui->wipeTable->item(selectedItems[0]->row(), 1)->text();

    QString taskId = QString("Wipe_%1").arg(QDateTime::currentDateTime().toString("yyymmmDDD__hhmmss"));

    // Add task to dashboard
    int rowCount = ui->dashboardTable->rowCount();
    ui->dashboardTable->insertRow(rowCount);
    ui->dashboardTable->setItem(rowCount, 0, new QTableWidgetItem(taskId));
    ui->dashboardTable->setItem(rowCount, 1, new QTableWidgetItem("Wipe"));
    ui->dashboardTable->setItem(rowCount, 2, new QTableWidgetItem("0%"));
    ui->dashboardTable->setItem(rowCount, 3, new QTableWidgetItem("0 MB/s"));

    // Add Stop button
    QPushButton *stopButton = new QPushButton("Stop");
    ui->dashboardTable->setCellWidget(rowCount, 4, stopButton);

    QProcess *process = new QProcess(this);
    connect(process, &QProcess::readyReadStandardError, this, [this, process, taskId]()
            { this->parseWipeOutput(process, taskId); });

    connect(stopButton, &QPushButton::clicked, this, [this, process, taskId, path]()
            { this->stopWipeProcess(process, taskId, path); });

    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, [this, process, path, taskId](int exitCode, QProcess::ExitStatus exitStatus)
            {
        this->onWipeTaskFinished(taskId, path, exitCode == 0 && exitStatus == QProcess::NormalExit);
        process->deleteLater();
            });
    qDebug() << "Run dc3dd wipe" << path;
    process->start("dc3dd", QStringList() << "wipe=" + path);
    QMessageBox::information(this, "Wipe operation", "Create wipe operation successfully!");
    for(auto & block : blocks) {
        if(block.path == path) {
            block.isBusy = true;
        }
    }
    updateWipeTable();

}

void MainWindow::parseWipeOutput(QProcess *process, const QString &taskName)
{
    QString output = QString::fromUtf8(process->readAllStandardError()).trimmed();
    qDebug() << "Wipe output" << output;
    QRegularExpression progressRegex(R"((\d+) bytes \( (\d+(?:\.\d+)?) (G|M) \) copied \( *(\d+)% *\), *(\d+) s, (\d+(?:\.\d+)?) (M|G)/s)");
    QRegularExpressionMatch match = progressRegex.match(output);

    if (match.hasMatch()) {
        QString progress = match.captured(4) + "%";  // Changed from 3 to 4
        QString speed = match.captured(6) + " " + match.captured(7) + "/s";  // Changed from 5,6 to 6,7

        // Update dashboard
        for (int i = 0; i < ui->dashboardTable->rowCount(); ++i) {
            if (ui->dashboardTable->item(i, 0)->text() == taskName) {
                ui->dashboardTable->setItem(i, 2, new QTableWidgetItem(progress));
                ui->dashboardTable->setItem(i, 3, new QTableWidgetItem(speed));
                break;
            }
        }
    } else {
        qDebug() << "No match found in output";
    }
}

void MainWindow::stopWipeProcess(QProcess *process, const QString &taskName, const QString path)
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Confirm Stop Task",
                                  "Are you sure you want to stop this task?",
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes)
    {
        process->terminate();
        if (!process->waitForFinished(500))
        {
            process->kill();
        }

        // Update dashboard
        for (int i = 0; i < ui->dashboardTable->rowCount(); ++i)
        {
            if (ui->dashboardTable->item(i, 0)->text() == taskName)
            {
                ui->dashboardTable->setItem(i, 2, new QTableWidgetItem("Stopped"));
                ui->dashboardTable->cellWidget(i, 4)->setEnabled(false);
                break;
            }
        }

        for(auto & block : blocks) {
            if(block.path == path) {
                block.isBusy = false;
            }
        }
        updateWipeTable();
    }
}

void MainWindow::onWipeTaskFinished(const QString &taskName, const QString path, bool success)
{
    // Update dashboard with task completion status
    for (int i = 0; i < ui->dashboardTable->rowCount(); ++i)
    {
        if (ui->dashboardTable->item(i, 0)->text() == taskName)
        {
            ui->dashboardTable->setItem(i, 2, new QTableWidgetItem(success ? "Completed" : "Failed"));
            ui->dashboardTable->cellWidget(i, 4)->setEnabled(false);
            break;
        }
    }
    for(auto & block : blocks) {
        if(block.path == path) {
            block.isBusy = false;
        }
    }
    updateWipeTable();
}

void MainWindow::on_pushButton_clicked()
{
    blocks = blkInfo->getBlocksInfo();
    updateWipeTable();
}

void MainWindow::on_sourceDiskTableReloadBtn_clicked()
{
    updateSourceDiskTable();
}

void MainWindow::on_createImageTaskBtn_clicked()
{
    QList<QTableWidgetItem *> selectedItems = ui->destinationsDiskTable->selectedItems();
    QList<int> selectedRows;

    for (const auto &item : selectedItems)
    {
        if (!selectedRows.contains(item->row()))
        {
            selectedRows.append(item->row());
        }
    }

    if (selectedRows.count() == 0)
    {
        QMessageBox::warning(this, "Invalid selection", "Please select at least on disk");
        return;
    }

    QList<QString> raidArrays;
    for (int i = 0; i < selectedRows.size(); ++i)
    {
        QString enclosureId = ui->destinationsDiskTable->item(selectedRows[i], 2)->text();
        QString slotId = ui->destinationsDiskTable->item(selectedRows[i], 1)->text();
        QString pd = QString("%1:%2").arg(enclosureId).arg(slotId);
        raidArrays.append(pd);
    }

    QList<QString> result = MegaCLIHandler::createRaid(raidArrays, blkInfo);
    if (result.count() == 0)
        return;
    QList<QTableWidgetItem *> selectedSourceItems = ui->sourceDiskTable->selectedItems();

    if (selectedItems.count() == 0)
    {
        QMessageBox::warning(this, "Invalid selection", "Please select at least on disk");
        return;
    }
    QString sourceImagePath = ui->sourceDiskTable->item(selectedSourceItems[0]->row(), 1)->text();

    Task task;
    task.source = sourceImagePath;
    task.destination = result.at(0);
    task.command = DC3DD;
    task.imageName = ui->imageNameTextBox->text();
    task.hash = MD5;

    handleCreateImageTask(task);

    updateDestinationDisksTable();
}

void MainWindow::handleCreateImageTask(const Task &task)
{
    QProcess process;
    qDebug() << "Prepare disk" << task.destination;
    process.start("prepare_disk", QStringList() << task.destination);
    process.waitForFinished();
    QString output = process.readAllStandardOutput();
    QStringList lines = output.split("\n", Qt::SkipEmptyParts);

    // Add task to dashboard
    int rowCount = ui->dashboardTable->rowCount();
    ui->dashboardTable->insertRow(rowCount);
    ui->dashboardTable->setItem(rowCount, 0, new QTableWidgetItem(task.imageName));

    // Add Stop button
    QPushButton *stopButton = new QPushButton("Stop");
    ui->dashboardTable->setCellWidget(rowCount, 2, stopButton);

    // connect button to stop process

    // connect(worker, &Worker::finished, this, &MainWindow::onWipeTaskFinished);
    // connect(worker, &Worker::progressUpdate, this, &MainWindow::onWipeProgressUpdate);

    // run dc3dd command, like: dc3dd if=/dev/sdc1 of=/mnt/md0/test hash=md5 log=test.md5 bufsz=16M
}

void MainWindow::on_destinationDiskTableReloadBtn_clicked()
{
    updateDestinationDisksTable();
}
