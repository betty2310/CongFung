#ifndef TASKRESULT_H
#define TASKRESULT_H

#include <QDialog>

namespace Ui {
class TaskResult;
}

class TaskResult : public QDialog
{
    Q_OBJECT

public:
    explicit TaskResult(QWidget *parent = nullptr);
    ~TaskResult();

private:
    Ui::TaskResult *ui;
};

#endif // TASKRESULT_H
