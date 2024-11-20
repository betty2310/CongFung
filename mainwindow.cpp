#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), isRootUser(false), settingDialog(new SettingDialog(this)), hiddenAreaDialog(new HiddenAreaDialog(this)), blkInfo(new BlocksInfo(this))
{
    ui->setupUi(this);

    ui->stackedWidget->setCurrentWidget(ui->dashboardPage);
    prepareMega();

    megaCliHandler = new MegaCLIHandler(this);
    setupDashboardPage();
    setupWipePage();
    setupCreateForensicImagePage();

    isRootUser = getuid() ? false : true;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::prepareMega()
{
    // TODO: only make destination disk to good state
    CliCommand::execute("prepare_mega");
}

void MainWindow::setupDashboardPage()
{
    ui->dashboardTable->setColumnCount(8);
    ui->dashboardTable->setHorizontalHeaderLabels({"ID", "Source", "Destination", "Progress", "Speed", "Time", "Log"});

    updateDashboardTable();
}

void MainWindow::setupCreateForensicImagePage()
{
    ui->sourceDiskTable->setColumnCount(5);
    ui->sourceDiskTable->setHorizontalHeaderLabels({"Name", "Identify", "Path", "Size", "Type"});

    ui->destinationsDiskTable->setColumnCount(6);
    ui->destinationsDiskTable->setHorizontalHeaderLabels({"Id", "Slot number", "Enclosure ID", "Size", "Speed", "Name"});

    ui->imageFormatComboBox->insertItem(0, "raw dd");
    ui->imageFormatComboBox->insertItem(1, "e01");
    ui->imageFormatComboBox->insertItem(2, "aff4");
    updateSourceDiskTable();
    updateDestinationDisksTable();
}

void MainWindow::updateSourceDiskTable()
{
    blocks = blkInfo->getBlocksInfo();

    QList<Block> displayBlocks;

    for (int i = 0; i < blocks.size(); ++i)
    {
        if (blocks[i].tran == "nvme")
        { // mean os drive, can't modify
        }
        else if (blocks[i].tran == "") // don't have transport type
        {
            if (blocks[i].parent == nullptr || blocks[i].parent->tran == "") // mean this is disk inserted from MEGA
            {
            }
            else
            {
                displayBlocks.append(blocks[i]); // normal partition
            }
        }
        else
        {
            displayBlocks.append(blocks[i]);
        }
    }

    QList<MegaDisk> megaDisks = megaCliHandler->disks;

    int displayRows = megaDisks.size();
    QList<MegaDisk> displayMegaDisks;

    for (int i = 0; i < megaDisks.size(); ++i)
    {
        if (megaDisks[i].slotNumber >= 4)
        {
            displayRows--;
        }
        else
        {
            displayMegaDisks.append(megaDisks[i]);
        }
    }

    ui->sourceDiskTable->setRowCount(displayBlocks.size() + displayMegaDisks.count());

    for (int i = 0; i < displayBlocks.size(); ++i)
    {
        const Block &dp = displayBlocks[i];
        ui->sourceDiskTable->setItem(i, 0, new QTableWidgetItem(dp.model));
        ui->sourceDiskTable->setItem(i, 1, new QTableWidgetItem(dp.name));
        ui->sourceDiskTable->setItem(i, 2, new QTableWidgetItem(dp.path));
        ui->sourceDiskTable->setItem(i, 3, new QTableWidgetItem(dp.size));
        ui->sourceDiskTable->setItem(i, 4, new QTableWidgetItem(dp.isPartition ? "Partition" : "Disk"));
    }

    for (int i = displayBlocks.size(); i < displayBlocks.size() + displayMegaDisks.count(); ++i)
    {
        if (displayMegaDisks[i - displayBlocks.size()].raidState == "JBOD")
        {
            // need to find the path of this disk in lsblk
            for (auto &block : blocks)
            {
                if (displayMegaDisks[i - displayBlocks.size()].inquiryData.contains(block.name))
                {
                    displayMegaDisks[i - displayBlocks.size()].path = block.path;
                    break;
                }
            }
        }
        QTableWidgetItem *item = new QTableWidgetItem(displayMegaDisks[i - displayBlocks.size()].inquiryData);
        QString pd = QString("%1:%2").arg(displayMegaDisks[i - displayBlocks.size()].enclosureDeviceId).arg(displayMegaDisks[i - displayBlocks.size()].slotNumber);
        item->setData(Qt::UserRole + 1, pd);
        item->setData(Qt::UserRole + 23, displayMegaDisks[i - displayBlocks.size()].raidState);
        ui->sourceDiskTable->setItem(i, 0, item);
        ui->sourceDiskTable->setItem(i, 2, new QTableWidgetItem(displayMegaDisks[i - displayBlocks.size()].path));
        ui->sourceDiskTable->setItem(i, 2, new QTableWidgetItem(""));
        ui->sourceDiskTable->setItem(i, 3, new QTableWidgetItem(displayMegaDisks[i - displayBlocks.size()].rawSize));
        ui->sourceDiskTable->setItem(i, 4, new QTableWidgetItem("Disk"));
    }

    ui->sourceDiskTable->setColumnWidth(0, 400);
}

void MainWindow::updateDestinationDisksTable()
{
    QList<MegaDisk> megaDisks = megaCliHandler->disks;
    int displayRows = megaCliHandler->disks.size();
    QList<MegaDisk> displayMegaDisks;

    for (int i = 0; i < megaDisks.size(); ++i)
    {
        if (megaDisks[i].slotNumber < 4)
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

        if (dp.isBusy)
        {
            for (int col = 0; col < ui->wipeTable->columnCount(); ++col)
            {
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

    connect(process, &QProcess::readyReadStandardOutput, this, [this, process, taskId]()
            {
        QString output = process->readAllStandardOutput();
        qDebug() << output; });

    connect(stopButton, &QPushButton::clicked, this, [this, process, taskId, path]()
            { this->stopWipeProcess(process, taskId, path); });

    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, [this, process, path, taskId](int exitCode, QProcess::ExitStatus exitStatus)
            {
        this->onWipeTaskFinished(taskId, path, exitCode == 0 && exitStatus == QProcess::NormalExit);
        process->deleteLater(); });
    qDebug() << "Run dc3dd wipe" << path;
    process->start("dc3dd", QStringList() << "wipe=" + path);

    qDebug() << "Enable read write on" << path;

    QMessageBox::information(this, "Wipe operation", "Create wipe operation successfully!");
    for (auto &block : blocks)
    {
        if (block.path == path)
        {
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

    if (match.hasMatch())
    {
        QString progress = match.captured(4) + "%";                         // Changed from 3 to 4
        QString speed = match.captured(6) + " " + match.captured(7) + "/s"; // Changed from 5,6 to 6,7

        // Update dashboard
        for (int i = 0; i < ui->dashboardTable->rowCount(); ++i)
        {
            if (ui->dashboardTable->item(i, 0)->text() == taskName)
            {
                ui->dashboardTable->setItem(i, 2, new QTableWidgetItem(progress));
                ui->dashboardTable->setItem(i, 3, new QTableWidgetItem(speed));
                break;
            }
        }
    }
    else
    {
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

        for (auto &block : blocks)
        {
            if (block.path == path)
            {
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
    for (auto &block : blocks)
    {
        if (block.path == path)
        {
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
    if (ui->imageNameTextBox->text().isEmpty())
    {
        QMessageBox::critical(this, "Missing information", "Please enter image name!");
        return;
    }

    QList<MegaDisk> destinationDisks = seletectedRowsToMegadisks();
    if (destinationDisks.isEmpty())
    {
        QMessageBox::warning(this, "Invalid selection", "Please select at least on disk");
        return;
    }

    SourceDiskInfo sourceDisk = getSourceDiskNameFromSourceTable();

    if (sourceDisk.name == "")
    {
        return;
    }

    if (sourceDisk.path.isEmpty()) // source from megacli
    {
        bool retFlag;
        makeJbodFromSourceDisk(sourceDisk, retFlag);
        if (retFlag)
            return;
    }

    hiddenAreaDialog = new HiddenAreaDialog(this, sourceDisk.path);
    connect(hiddenAreaDialog, &HiddenAreaDialog::handledDiskArea, this, [this, destinationDisks, sourceDisk]()
            {
                bool isOk = makeRaidArrayFromDestinationDicks(destinationDisks, sourceDisk);
                this->hiddenAreaDialog->setProgressBar(50);
                if (!isOk)
                {
                    return;
                }
                handleCreateImageTask(); 
                this->hiddenAreaDialog->setProgressBar(100);
                this->hiddenAreaDialog->close(); });
    hiddenAreaDialog->show();
}

bool MainWindow::makeRaidArrayFromDestinationDicks(QList<MegaDisk> destinationDisks, SourceDiskInfo sourceDisk)
{
    // TODO: handle mutiple disk created but only in one raid array
    if (destinationDisks.size() == 1 && !destinationDisks[0].mountedPath.isEmpty()) // select one destination disk, and thid disk also is raid
    {
        QString imageName = ui->imageNameTextBox->text().trimmed();
        int command = ui->imageFormatComboBox->currentIndex();
        Task task = makeTask(sourceDisk, destinationDisks[0].path, destinationDisks[0].mountedPath, imageName, command, QString("%1").arg(destinationDisks[0].pdNumber));
        currentTasks.append(task);
        // megaCliHandler->reload();
        // updateDestinationDisksTable();
        return true;
    }

    QList<QString> raidArrays;
    for (int i = 0; i < destinationDisks.size(); ++i)
    {
        int enclosureId = destinationDisks[i].enclosureDeviceId;
        int slotId = destinationDisks[i].slotNumber;
        QString pd = QString("%1:%2").arg(enclosureId).arg(slotId);
        raidArrays.append(pd);
    }

    QList<MegaCLIResponse> result = megaCliHandler->createRaid(raidArrays, blkInfo);
    if (result.isEmpty())
    {
        QMessageBox::critical(this, "Failed to create RAID", "Please try again!");
        return false;
    }

    for (auto &res : result)
    {
        QString imageName = ui->imageNameTextBox->text().trimmed();
        int command = ui->imageFormatComboBox->currentIndex();
        Task task = makeTask(sourceDisk, res.path, "", imageName, command, res.pdNumber);
        currentTasks.append(task);
        // megaCliHandler->reload();
        // updateDestinationDisksTable();
    }
    return true;
}

Task MainWindow::makeTask(SourceDiskInfo &sourceDisk, QString &destination, QString mountedPath, QString imageName, int command, QString pdNumber)
{
    Task task;
    task.source = sourceDisk.path;
    task.sourceModel = sourceDisk.name;

    task.destination = destination;
    task.command = DC3DD;
    task.imageName = imageName;
    task.mountedPath = mountedPath;
    switch (command)
    {
    case 0:
        task.command = DC3DD;
        break;
    case 1:
        task.command = EWFACQUIRE;
        break;
    case 2:
        task.command = AFFIMAGER;
        break;
    default:
        task.command = DC3DD;
        break;
    }

    task.hash = MD5;
    task.pdNumber = pdNumber;
    task.description = "";
    task.caseNumber = "";
    task.examiner = "";
    task.notes = "";
    task.evidence = "";
    return task;
}

void MainWindow::makeJbodFromSourceDisk(SourceDiskInfo &info, bool &retFlag)
{
    retFlag = true;
    QString pd = info.pd;
    QString raidState = info.raidState;
    if (raidState == "Unknown")
    {
        QMessageBox::critical(this, "Source disk fail", "Please re-plug the source disk!");
        return;
    }
    else if (raidState == "JBOD")
    {
    }
    else
    {
        QString jbodDiskPath = MegaCLIHandler::createJbod(pd, blkInfo);
        if (jbodDiskPath.isEmpty())
        {
            QMessageBox::warning(this, "Operation failed!", "");
            return;
        }
        // update sourcedisk table
        for (int i = 0; i < ui->sourceDiskTable->rowCount(); ++i)
        {
            if (ui->sourceDiskTable->item(i, 0)->text() == info.name)
            {
                ui->sourceDiskTable->setItem(i, 2, new QTableWidgetItem(jbodDiskPath));
                break;
            }
        }
        info.path = jbodDiskPath;
        retFlag = false;
    }
}

void MainWindow::cleanRaid(const Task &task)
{
    QProcess process;
    process.start("clean_create_image_task", QStringList() << task.mountedPath << task.pdNumber);
    process.waitForFinished();
    QString output = process.readAllStandardOutput();
    qDebug() << output;
    qDebug() << "Clean pd" << task.pdNumber << "\nUmount" << task.mountedPath;
    megaCliHandler->reload();
    updateDestinationDisksTable();
}

void MainWindow::cleanRaid()
{
    megaCliHandler->reload();
    // for (auto &task : currentTasks)
    // {
    //     cleanRaid(task);
    // }
}

void MainWindow::handleCreateImageTask()
{
    // TODO: tasks need to be clear after finish or cancel operation
    // need to make a historyTasks
    if (currentTasks.count() == 1)
    {
        handleCreateImageTask(currentTasks[0]);
    }
    else if (currentTasks.count() == 2)
    {
        for (auto &task : currentTasks)
        {
            QProcess process;
            qDebug() << "Prepare disk" << task.destination;
            process.start("prepare_disk", QStringList() << task.destination);
            process.waitForFinished();
            const QString output = process.readAllStandardOutput();
            qDebug() << output;
            QStringList lines = output.split("\n", Qt::SkipEmptyParts);
            QString mountedPath = lines.last();
            qDebug() << mountedPath;
            task.mountedPath = mountedPath;
            task.id = QString("createImageTask_%1_%2").arg(task.imageName).arg(QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss"));
        }

        // Add task to dashboard
        int rowCount = ui->dashboardTable->rowCount();
        ui->dashboardTable->insertRow(rowCount);
        ui->dashboardTable->setItem(rowCount, 0, new QTableWidgetItem(currentTasks[0].id));
        ui->dashboardTable->setItem(rowCount, 1, new QTableWidgetItem(currentTasks[0].sourceModel));
        QString destination = QString("%1/%2").arg(currentTasks[0].mountedPath).arg(currentTasks[0].imageName);
        if (currentTasks[0].command == DC3DD)
        {
            destination += ".dd";
        }
        else if (currentTasks[0].command == EWFACQUIRE)
        {
            destination += ".e01";
        }
        else
        {
            destination += ".aff4";
        }
        ui->dashboardTable->setItem(rowCount, 2, new QTableWidgetItem(destination));
        ui->dashboardTable->setItem(rowCount, 6, new QTableWidgetItem("Running"));

        currentTasks[0].caseNumber = ui->caseNumberTextBox->text().trimmed();
        currentTasks[0].description = ui->descriptionTextBox->text().trimmed();
        currentTasks[0].evidence = ui->evidenceNumberTextBox->text().trimmed();
        currentTasks[0].notes = ui->notesTextBox->text().trimmed();
        currentTasks[0].examiner = ui->examinerTextBox->text().trimmed();

        // Add Stop button
        QPushButton *stopButton = new QPushButton("Stop");
        ui->dashboardTable->setCellWidget(rowCount, 7, stopButton);

        // connect button to stop process

        // connect(worker, &Worker::finished, this, &MainWindow::onWipeTaskFinished);
        // connect(worker, &Worker::progressUpdate, this, &MainWindow::onWipeProgressUpdate);

        // run dc3dd command, like: dc3dd if=/dev/sdc1 of=/mnt/md0/test hash=md5 log=test.md5 bufsz=16M

        Task task = currentTasks[0];

        auto *taskProcess = new QProcess(this);
        connect(taskProcess, &QProcess::readyReadStandardError, this, [this, taskProcess, task]()
                { this->parseCreateImageTaskOutput(taskProcess, task); });

        connect(taskProcess, &QProcess::readyReadStandardOutput, this, [this, taskProcess, task]()
                { this->parseCreateImageTaskOutput(taskProcess, task); });

        connect(stopButton, &QPushButton::clicked, this, [this, taskProcess, task]()
                {
                    this->stopCreateImageTaskProcess(taskProcess, task);
                    currentTasks.clear();
                    cleanRaid(); });

        connect(taskProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                this, [this, taskProcess, task](int exitCode, QProcess::ExitStatus exitStatus)
                {
                    this->onCreateImageTaskFinished(task, exitCode == 0 && exitStatus == QProcess::NormalExit);
                    currentTasks.clear();
                    taskProcess->deleteLater();
                    // cleanRaid();
                });

        qDebug() << "Create image task";
        QStringList arguments;
        if (task.command == DC3DD)
        {
            arguments << QString("if=%1").arg(task.source)
                      << QString("of=%1/%2.dd").arg(currentTasks[0].mountedPath).arg(task.imageName)
                      << QString("of=%1/%2.dd").arg(currentTasks[1].mountedPath).arg(task.imageName)
                      << "hash=md5"
                      << QString("log=%1.md5").arg(task.imageName)
                      << "bufsz=16M";
            taskProcess->start("dc3dd", arguments);
        }
        else if (task.command == EWFACQUIRE)
        {
            arguments << task.source << "-t" << QString("%1/%2").arg(currentTasks[0].mountedPath).arg(task.imageName)
                      << "-2" << QString("%1/%2").arg(currentTasks[1].mountedPath).arg(task.imageName)
                      << "-C" << task.caseNumber
                      << "-D" << task.description
                      << "-e" << task.examiner
                      << "-E" << task.evidence
                      << "-N" << task.notes
                      << "-f" << "encase6"
                      << "-m" << "fixed" << "-M" << "physical"
                      << "-c"
                      << "deflate:best"
                      << "-S" << "2G" << "-b" << "64" << "-g" << "64" << "-r" << "2" << "-u";
            taskProcess->start("ewfacquire", arguments);
        }
        else
        {
            // TODO: affimager case
            // aff4imager -i /dev/sda1 -o /tmp/test.aff4
            arguments << "-i" << task.source << "-o" << QString("%1/%2.aff4").arg(task.mountedPath).arg(task.imageName);
            taskProcess->start("aff4imager", arguments);
        }

        // Start the taskProcess with the command and its arguments
        if (!taskProcess->waitForStarted())
        {
            qDebug() << "Failed to start process:" << taskProcess->errorString();
            return; // Exit the function if the process did not start
        }
        QMessageBox::information(this, "Create image task", "Oke!");
    }
    else
    {
        return;
    }
}

void MainWindow::handleCreateImageTask(Task &task)
{
    if (task.mountedPath == "")
    {
        QProcess process;
        qDebug() << "Prepare disk" << task.destination;
        process.start("prepare_disk", QStringList() << task.destination);
        process.waitForFinished();
        const QString output = process.readAllStandardOutput();
        qDebug() << output;
        QStringList lines = output.split("\n", Qt::SkipEmptyParts);
        QString mountedPath = lines.last();
        megaCliHandler->reload();
        task.mountedPath = mountedPath;
    }

    qDebug() << "Task mounted path" << task.mountedPath;

    task.id = QString("createImageTask_%1_%2").arg(task.imageName).arg(QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss"));
    // Add task to dashboard
    int rowCount = ui->dashboardTable->rowCount();
    ui->dashboardTable->insertRow(rowCount);
    ui->dashboardTable->setItem(rowCount, 0, new QTableWidgetItem(task.id));
    ui->dashboardTable->setItem(rowCount, 1, new QTableWidgetItem(task.sourceModel));
    QString destination = QString("%1/%2").arg(task.mountedPath).arg(task.imageName);
    if (task.command == DC3DD)
    {
        destination += ".dd";
    }
    else if (task.command == EWFACQUIRE)
    {
        destination += ".e01";
    }
    else
    {
        destination += ".aff4";
    }
    ui->dashboardTable->setItem(rowCount, 2, new QTableWidgetItem(destination));
    ui->dashboardTable->setItem(rowCount, 6, new QTableWidgetItem("Running"));

    task.caseNumber = ui->caseNumberTextBox->text().trimmed();
    task.description = ui->descriptionTextBox->text().trimmed();
    task.evidence = ui->evidenceNumberTextBox->text().trimmed();
    task.notes = ui->notesTextBox->text().trimmed();
    task.examiner = ui->examinerTextBox->text().trimmed();

    // Add Stop button
    QPushButton *stopButton = new QPushButton("Stop");
    ui->dashboardTable->setCellWidget(rowCount, 7, stopButton);

    // connect button to stop process

    // connect(worker, &Worker::finished, this, &MainWindow::onWipeTaskFinished);
    // connect(worker, &Worker::progressUpdate, this, &MainWindow::onWipeProgressUpdate);

    // run dc3dd command, like: dc3dd if=/dev/sdc1 of=/mnt/md0/test hash=md5 log=test.md5 bufsz=16M

    auto *taskProcess = new QProcess(this);
    connect(taskProcess, &QProcess::readyReadStandardError, this, [this, taskProcess, task]()
            { this->parseCreateImageTaskOutput(taskProcess, task); });

    connect(taskProcess, &QProcess::readyReadStandardOutput, this, [this, taskProcess, task]()
            { this->parseCreateImageTaskOutput(taskProcess, task); });

    connect(stopButton, &QPushButton::clicked, this, [this, taskProcess, task]()
            {
                this->stopCreateImageTaskProcess(taskProcess, task);
                currentTasks.clear();
                // cleanRaid(task);
            });

    connect(taskProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, [this, taskProcess, task](int exitCode, QProcess::ExitStatus exitStatus)
            {
                this->onCreateImageTaskFinished(task, exitCode == 0 && exitStatus == QProcess::NormalExit);
                currentTasks.clear();
                taskProcess->deleteLater();
                // cleanRaid(task);
            });

    qDebug() << "Create image task";
    QStringList arguments;
    if (task.command == DC3DD)
    {
        arguments << QString("if=%1").arg(task.source)
                  << QString("of=%1/%2.dd").arg(task.mountedPath).arg(task.imageName)
                  << "hash=md5"
                  << QString("log=%1.md5").arg(task.imageName)
                  << "bufsz=16M";
        taskProcess->start("dc3dd", arguments);
    }
    else if (task.command == EWFACQUIRE)
    {
        arguments << task.source << "-t" << QString("%1/%2").arg(task.mountedPath).arg(task.imageName)
                  << "-C" << task.caseNumber
                  << "-D" << task.description
                  << "-e" << task.examiner
                  << "-E" << task.evidence
                  << "-N" << task.notes
                  << "-f" << "encase6"
                  << "-m" << "fixed" << "-M" << "physical"
                  << "-c"
                  << "deflate:best"
                  << "-S" << "2G" << "-b" << "64" << "-g" << "64" << "-r" << "2" << "-u";
        taskProcess->start("ewfacquire", arguments);
    }
    else
    {
        // TODO: affimager case
        // aff4imager -i /dev/sda1 -o /tmp/test.aff4
        arguments << "-i" << task.source << "-o" << QString("%1/%2.aff4").arg(task.mountedPath).arg(task.imageName);
        taskProcess->start("aff4imager", arguments);
    }

    // Start the taskProcess with the command and its arguments
    if (!taskProcess->waitForStarted())
    {
        qDebug() << "Failed to start process:" << taskProcess->errorString();
        return; // Exit the function if the process did not start
    }
    QMessageBox::information(this, "Create image task", "Oke!");
}

void MainWindow::parseCreateImageTaskOutput(QProcess *process, const Task &task)
{
    qDebug() << "[Task create image task]";
    QString output;
    QString progress;
    QString speed;

    if (task.command == DC3DD)
    {
        output = QString::fromUtf8(process->readAllStandardError()).trimmed();
        QRegularExpression progressRegex(R"((\d+) bytes \( (\d+(?:\.\d+)?) (G|M) \) copied \( *(\d+)% *\), *(\d+) s, (\d+(?:\.\d+)?) (M|G)/s)");
        QRegularExpressionMatch match = progressRegex.match(output);

        if (match.hasMatch())
        {
            progress = match.captured(4) + "%";
            speed = match.captured(6) + " " + match.captured(7) + "/s";
        }
    }
    else if (task.command == EWFACQUIRE)
    {
        output = QString::fromUtf8(process->readAllStandardOutput()).trimmed();
        QRegularExpression progressRegex(R"(Status: at (\d+)%\.[\s\S]*?completion in \d+ second\(s\) with (\d+) (MiB|GiB)/s)");
        QRegularExpressionMatch match = progressRegex.match(output);

        if (match.hasMatch())
        {
            progress = match.captured(1) + "%";
            speed = match.captured(2) + " " + match.captured(3) + "/s";
        }
        qDebug() << "Output " << output << "Progress: " << progress << speed;
    }

    if (!progress.isEmpty() && !speed.isEmpty())
    {
        // Update dashboard
        for (int i = 0; i < ui->dashboardTable->rowCount(); ++i)
        {
            if (ui->dashboardTable->item(i, 0)->text() == task.id)
            {
                ui->dashboardTable->setItem(i, 3, new QTableWidgetItem(progress));
                ui->dashboardTable->setItem(i, 4, new QTableWidgetItem(speed));
                break;
            }
        }
    }

    qDebug() << "[Output]: " << output;
}

void MainWindow::stopCreateImageTaskProcess(QProcess *process, const Task &task)
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
            if (ui->dashboardTable->item(i, 0)->text() == task.id)
            {
                ui->dashboardTable->setItem(i, 6, new QTableWidgetItem("Stopped"));
                ui->dashboardTable->cellWidget(i, 7)->setEnabled(false);
                break;
            }
        }
    }
}

void MainWindow::onCreateImageTaskFinished(const Task &task, bool success)
{
    // Update dashboard with task completion status
    for (int i = 0; i < ui->dashboardTable->rowCount(); ++i)
    {
        if (ui->dashboardTable->item(i, 0)->text() == task.id)
        {
            ui->dashboardTable->setItem(i, 6, new QTableWidgetItem(success ? "Completed" : "Failed"));
            ui->dashboardTable->setItem(i, 3, new QTableWidgetItem("100%"));
            ui->dashboardTable->cellWidget(i, 7)->setEnabled(false);
            break;
        }
    }
    // TODO: verify

    writeTaskMetadata(task, success);
}

void MainWindow::writeTaskMetadata(const Task &task, bool success)
{
    task_metadata metadata;

    // Set metadata fields
    metadata.imagePath = QString("%1/%2.dd").arg(task.mountedPath).arg(task.imageName);
    metadata.caseNumber = ui->caseNumberTextBox->text();
    metadata.description = ui->descriptionTextBox->text();
    metadata.examinerName = ui->examinerTextBox->text();
    metadata.evidenceName = ui->evidenceNumberTextBox->text();
    metadata.note = ui->notesTextBox->text();
    metadata.fileFormat = ui->imageFormatComboBox->currentText();
    metadata.averageSpeed = "";
    metadata.time = "";

    // for (int i = 0; i < ui->dashboardTable->rowCount(); ++i)
    // {
    //     if (ui->dashboardTable->item(i, 0)->text() == task.id)
    //     {
    //         metadata.averageSpeed = ui->dashboardTable->item(i, 4)->text();
    //         metadata.time = ui->dashboardTable->item(i, 5)->text();
    //         break;
    //     }
    // }

    // Write metadata to file
    QString metadataPath = QString("%1/%2_metadata.txt").arg(task.mountedPath).arg(task.imageName);
    metadata.writeTo(metadataPath);

    qDebug() << "Task metadata written to:" << metadataPath;
}

void MainWindow::on_destinationDiskTableReloadBtn_clicked()
{
    megaCliHandler->reload();
    updateDestinationDisksTable();
}

QList<MegaDisk> MainWindow::seletectedRowsToMegadisks()
{
    QList<MegaDisk> megaDisks;
    QList<QTableWidgetItem *> selectedDestinationDisks = ui->destinationsDiskTable->selectedItems();
    QList<int> selectedDestinationDiskRows;

    for (const auto &item : selectedDestinationDisks)
    {
        if (!selectedDestinationDiskRows.contains(item->row()))
        {
            selectedDestinationDiskRows.append(item->row());
        }
    }

    for (int i = 0; i < selectedDestinationDiskRows.size(); ++i)
    {
        QString slotId = ui->destinationsDiskTable->item(selectedDestinationDiskRows[i], 1)->text();
        for (auto &disk : megaCliHandler->disks)
        {
            if (disk.slotNumber == slotId.toInt())
            {
                megaDisks.append(disk);
            }
        }
    }
    return megaDisks;
}

SourceDiskInfo MainWindow::getSourceDiskNameFromSourceTable()
{
    SourceDiskInfo info;
    info.name = "";
    info.path = "";
    QList<QTableWidgetItem *> selectedSourceItems = ui->sourceDiskTable->selectedItems();
    if (selectedSourceItems.count() == 0)
    {
        QMessageBox::warning(this, "Invalid selection", "Please select at least on disk");
        return info;
    }
    QString sourceImagePath = ui->sourceDiskTable->item(selectedSourceItems[0]->row(), 2)->text();
    info.path = sourceImagePath;
    info.name = ui->sourceDiskTable->item(selectedSourceItems[0]->row(), 0)->text();
    if (info.path.isEmpty()) // source disk from megacli and still not in jbod
    {
        info.pd = ui->sourceDiskTable->item(selectedSourceItems[0]->row(), 0)->data(Qt::UserRole + 1).toString();
        info.raidState = ui->sourceDiskTable->item(selectedSourceItems[0]->row(), 0)->data(Qt::UserRole + 23).toString();
    }
    return info;
}