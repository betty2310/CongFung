#include "hiddenareadialog.h"

#include "ui_hiddenareadialog.h"

HiddenAreaDialog::HiddenAreaDialog(QWidget *parent, QString path)
    : QDialog(parent), ui(new Ui::HiddenAreaDialog), path(path), shouldShow(false)
{
    ui->setupUi(this);
    ui->rmHpaProgressBar->hide();
    HPAarea hpaArea = checkHpaArea();
    if (hpaArea.isEnable)
    {
        shouldShow = true;
        ui->hpaSectorLb->setText(QString("%1/%2").arg(hpaArea.sector).arg(hpaArea.allSector));
        ui->imageHpaCheckbox->setChecked(true);
    }
    else
    {
        ui->hdareaLabel->setText("HPA is disabled");
        ui->imageHpaCheckbox->hide();
    }
    this->a = hpaArea;
    DCOarea dco = checkDcoArea();
    if (dco.isEnable)
    {
        if (dco.sector == hpaArea.allSector)
        {
            ui->dcoLabel->setText("DCO is disabled");
        }
        else
        {
            shouldShow = true;
            ui->dcoSectorLb->setText(dco.sector);
        }
    }
    else
    {
        ui->dcoLabel->setText("DCO is disabled");
    }
    this->d = dco;
}

HPAarea HiddenAreaDialog::checkHpaArea()
{
    HPAarea a;
    QProcess process;
    process.start("hdparm", QStringList() << "-N" << path);
    process.waitForFinished();
    QString output = process.readAllStandardOutput();
    qDebug() << output;

    QRegularExpression regex(R"(max sectors\s+=\s+(\d+)/(\d+),\s+HPA\s+is\s+(enabled|disabled))");
    QRegularExpressionMatch match = regex.match(output);

    if (match.hasMatch())
    {
        QString hpaSectors = match.captured(1);
        QString allSectors = match.captured(2);
        QString hpaStatus = match.captured(3);

        qDebug() << "HPA Sectors:" << hpaSectors;
        qDebug() << "All Sectors:" << allSectors;
        qDebug() << "HPA Status:" << hpaStatus;

        a.sector = hpaSectors;
        a.allSector = allSectors;
        a.isEnable = hpaStatus == "enabled";
    }
    else
    {
        qDebug() << "Failed to parse hdparm output.";
        a.isEnable = false;
    }
    return a;
}

DCOarea HiddenAreaDialog::checkDcoArea()
{
    DCOarea a;
    QProcess process;
    process.start("hdparm", QStringList() << "--dco-identify" << path);
    process.waitForFinished();
    QString output = process.readAllStandardOutput();
    qDebug() << QString("[Check DCO command] hdparm --dco-identify %1").arg(path);
    qDebug() << output;
    QRegularExpression regex(R"(Real max sectors:\s+(\d+))");
    QRegularExpressionMatch match = regex.match(output);

    if (match.hasMatch())
    {
        QString realMaxSectors = match.captured(1);

        qDebug() << "Real Max Sectors:" << realMaxSectors;
        if (realMaxSectors == "1")
        {
            a.isEnable = false;
        }
        else
        {
            a.sector = realMaxSectors;
            a.isEnable = true;
        }
    }
    else
    {
        a.isEnable = false;
        qDebug() << "Failed to parse hdparm output.";
    }
    return a;
}

HiddenAreaDialog::~HiddenAreaDialog()
{
    delete ui;
}

void HiddenAreaDialog::runRemoveHpaScript()
{
    ui->rmHpaProgressBar->setMinimum(0);
    ui->rmHpaProgressBar->setMaximum(100);
    ui->rmHpaProgressBar->show();
    ui->rmHpaProgressBar->setValue(0);

    QProcess rmHpaProcess;
    rmHpaProcess.start("device_rm_hpa", QStringList() << path << a.allSector);
    rmHpaProcess.waitForFinished(-1);
    qDebug() << rmHpaProcess.readAllStandardOutput();

    rescanHpaArea = new QProcess(this);

    connect(rescanHpaArea, &QProcess::readyReadStandardOutput,
            this, &HiddenAreaDialog::handleProcessOutput);

    connect(rescanHpaArea, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &HiddenAreaDialog::onScriptFinished);

    rescanHpaArea->start("device_rescan", QStringList() << path);
}

void HiddenAreaDialog::handleProcessOutput()
{
    QByteArray output = rescanHpaArea->readAllStandardOutput();
    QString outputStr = QString::fromLocal8Bit(output);

    // Log the output
    qDebug() << "Process output:" << outputStr;
    ui->rmHpaProgressBar->setValue(50);
}

void HiddenAreaDialog::onScriptFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (exitStatus == QProcess::NormalExit && exitCode == 0)
    {
        ui->rmHpaProgressBar->setValue(100);
    }
    else
    {
        qDebug() << "Error", "Failed to remove HPA area. Please try again.";
    }
    close();
    rescanHpaArea->deleteLater();
}

void HiddenAreaDialog::on_okeButton_clicked()
{
    qDebug() << "Button ok!";
    if (ui->imageHpaCheckbox->isChecked())
    {
        runRemoveHpaScript();
    }
    else
    {
        close();
    }
}
