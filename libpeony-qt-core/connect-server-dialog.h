#ifndef CONNECTSERVERDIALOG_H
#define CONNECTSERVERDIALOG_H

#include <QDialog>

namespace Ui {
class ConnectServerDialog;
}

class ConnectServerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConnectServerDialog(QWidget *parent = nullptr);
    ~ConnectServerDialog();

    QString user();
    QString password();
    QString domain();
    bool savePassword();
    bool anonymous();

private:
    Ui::ConnectServerDialog *ui;
};

#endif // CONNECTSERVERDIALOG_H
