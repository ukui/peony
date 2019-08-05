#include "connect-server-dialog.h"
#include "ui_connect-server-dialog.h"

ConnectServerDialog::ConnectServerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConnectServerDialog)
{
    ui->setupUi(this);
    ui->passwd_edit->setEchoMode(QLineEdit::Password);

    connect(ui->anonymous_checkbox, &QCheckBox::toggled, [=](bool checked){
        if (checked) {
            this->ui->usr_edit->clear();
            this->ui->usr_edit->setEnabled(false);
            this->ui->passwd_edit->clear();
            this->ui->passwd_edit->setEnabled(false);
            this->ui->domain_edit->clear();
            this->ui->domain_edit->setEnabled(false);
            this->ui->save_passwd_checkbox->setChecked(false);
            this->ui->save_passwd_checkbox->setEnabled(true);
        } else {
            this->ui->usr_edit->clear();
            this->ui->usr_edit->setEnabled(true);
            this->ui->passwd_edit->clear();
            this->ui->passwd_edit->setEnabled(true);
            this->ui->domain_edit->clear();
            this->ui->domain_edit->setEnabled(true);
            this->ui->save_passwd_checkbox->setChecked(false);
            this->ui->save_passwd_checkbox->setEnabled(true);
        }
    });
    ui->passwd_edit->setEnabled(false);
    ui->anonymous_checkbox->setChecked(true);
}

ConnectServerDialog::~ConnectServerDialog()
{
    disconnect();
    delete ui;
}

QString ConnectServerDialog::user()
{
    return ui->usr_edit->text();
}

QString ConnectServerDialog::password()
{
    return ui->passwd_edit->text();
}

QString ConnectServerDialog::domain()
{
    return ui->domain_edit->text();
}

bool ConnectServerDialog::savePassword()
{
    return ui->save_passwd_checkbox->isChecked();
}

bool ConnectServerDialog::anonymous()
{
    return ui->anonymous_checkbox->isChecked();
}
