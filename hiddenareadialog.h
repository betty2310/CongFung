#ifndef HIDDENAREADIALOG_H
#define HIDDENAREADIALOG_H

#include <QDialog>

namespace Ui {
class HiddenAreaDialog;
}

class HiddenAreaDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HiddenAreaDialog(QWidget *parent = nullptr, QString path = "");
    ~HiddenAreaDialog();

private:
    Ui::HiddenAreaDialog *ui;
    QString path;
};

#endif // HIDDENAREADIALOG_H
