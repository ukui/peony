/*
 * Peony-Qt's Library
 *
 * Copyright (C) 2019, Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Authors: Yue Lan <lanyue@kylinos.cn>
 *
 */

#include "connect-server-dialog.h"
#include "ui_connect-server-dialog.h"

ConnectServerDialog::ConnectServerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConnectServerDialog)
{
    ui->setupUi(this);
    ui->pwd_lineEdit->setEchoMode(QLineEdit::Password);

    ui->port_comboBox->setEditText("445");

//    connect(ui->type_comboBox, &QComboBox::currentTextChanged, [=](QString& tp) {
//        if ("SAMBA" == tp) {
//            ui->port_comboBox->setEditText("445");
//        } else if ("FTP" == tp) {
//            ui->port_comboBox->setEditText("21");
//        }
//    });
}

ConnectServerDialog::~ConnectServerDialog()
{
    disconnect();
    delete ui;
}

QString ConnectServerDialog::user()
{
    return ui->user_lineEdit->text();
}

QString ConnectServerDialog::password()
{
    return ui->pwd_lineEdit->text();
}

QString ConnectServerDialog::domain()
{
    return ui->ip_edit->text() + ":" + ui->port_comboBox->currentText();
}

QString ConnectServerDialog::uri()
{
    QString uuri = "";

    if (ui->type_comboBox->currentText() == "SAMBA") {
        uuri = "smb://" + ui->ip_edit->text() + ":" + ui->port_comboBox->currentText() + "/" + ui->file_lineEdit->text();
    } else if (ui->type_comboBox->currentText() == "FTP") {
        uuri = "ftp://" + ui->ip_edit->text() + ":" + ui->port_comboBox->currentText() + "/" + ui->file_lineEdit->text();
    }

    return uuri;
}

