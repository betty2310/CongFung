#include "taskresult.h"
#include "ui_taskresult.h"

TaskResult::TaskResult(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::TaskResult)
{
    ui->setupUi(this);
}

TaskResult::~TaskResult()
{
    delete ui;
}
