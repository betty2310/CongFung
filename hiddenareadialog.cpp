#include "hiddenareadialog.h"

#include <utility>
#include "ui_hiddenareadialog.h"

HiddenAreaDialog::HiddenAreaDialog(QWidget *parent, QString path)
    : QDialog(parent)
    , ui(new Ui::HiddenAreaDialog), path(path)
{
    ui->setupUi(this);
    ui->hdareaLabel->setText(path);

}

HiddenAreaDialog::~HiddenAreaDialog()
{
    delete ui;
}
