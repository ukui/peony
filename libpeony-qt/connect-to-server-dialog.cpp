/*
 * Peony-Qt's Library
 *
 * Copyright (C) 2020, KylinSoft Co., Ltd.
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
 * Authors: Ding Jing <dingjing@kylinos.cn>
 *
 */

#include "connect-to-server-dialog.h"
#include "global-settings.h"

#include <QUrl>
#include <QIcon>
#include <QDebug>
#include <QLineEdit>
#include <QSizePolicy>
#include <QSpacerItem>
#include <QButtonGroup>

using namespace Peony;


ConnectServerDialog::ConnectServerDialog(QWidget *parent) : QDialog(parent)
{
    setFixedSize(m_widget_size);
    setWindowIcon(QIcon::fromTheme("network-server"));
    setWindowTitle(tr("connect to server"));
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);

    m_main_layout = new QVBoxLayout(this);

    m_remote_type_edit      = new QComboBox;
    m_remote_type_label     = new QLabel;
    m_ip_label              = new QLabel;
    m_port_label            = new QLabel;
    m_ip_edit               = new QLineEdit;
    m_port_editor           = new QComboBox;
    m_remote_layout         = new QGridLayout;
    m_remote_type           = new QStringList;
    m_remote_port           = new QStringList;

    m_main_layout->addSpacing(12);

    m_remote_type->append("ftp");
    m_remote_type->append("sftp");
    m_remote_type->append("samba");
    m_remote_port->append("20");
    m_remote_port->append("21");
    m_remote_port->append("22");
    m_remote_port->append("445");
    m_ip_label->setText(tr("ip"));
    m_port_editor->setEditable(true);
    m_port_label->setText(tr("port"));
    m_remote_type_label->setText(tr("type"));
    m_main_layout->setMargin(m_widget_margin);
    m_remote_type_edit->setAutoCompletion(true);
    m_ip_label->setFixedHeight(36);
    m_ip_edit->setFixedHeight(36);
    m_ip_edit->setFixedWidth(194);
    m_port_label->setFixedHeight(36);
    m_port_editor->setFixedHeight(36);
    m_port_editor->setFixedWidth(65);
    m_remote_type_label->setFixedHeight(36);
    m_remote_type_edit->setFixedHeight(36);
    m_remote_type_edit->addItems(*m_remote_type);
    m_port_editor->addItems(*m_remote_port);
    m_remote_type_edit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

    m_remote_layout->addWidget(m_remote_type_label, 0, 0, 1, 1);
    m_remote_layout->addWidget(m_remote_type_edit,  0, 1, 1, 5);
    m_remote_layout->setVerticalSpacing(20);
    m_remote_layout->addWidget(m_ip_label,          1, 0, 1, 1);
    m_remote_layout->addWidget(m_ip_edit,           1, 1, 1, 1);
    m_remote_layout->addWidget(m_port_label,        1, 2, 1, 1);
    m_remote_layout->addWidget(m_port_editor,       1, 3, 1, 3);

    m_main_layout->addLayout(m_remote_layout);
    m_main_layout->addSpacing(28);

    m_favorite_label        = new QLabel;
    m_favorite_layout       = new QVBoxLayout;
    m_favorite_list         = new QListWidget;
    m_favorite_list->setFixedSize(m_favorite_list_size);
    m_favorite_label->setText(tr("Personal Collection server:"));

    m_favorite_layout->addWidget(m_favorite_label);
    m_favorite_layout->addWidget(m_favorite_list);
    m_main_layout->addLayout(m_favorite_layout);

    m_btn_add               = new QPushButton;
    m_btn_del               = new QPushButton;
    m_btn_conn              = new QPushButton;
    m_btn_layout            = new QHBoxLayout;

    m_main_layout->addSpacing(12);

    m_btn_add->setText(tr("add"));
    m_btn_del->setText(tr("delete"));
    m_btn_conn->setText(tr("connect"));
    m_btn_layout->addWidget(m_btn_add);
    m_btn_layout->addWidget(m_btn_del);
    m_btn_layout->addSpacing(72);
    m_btn_layout->addWidget(m_btn_conn);
    m_btn_conn->setAutoDefault(true);
    m_btn_add->setAutoDefault(false);
    m_btn_del->setAutoDefault(false);
    m_main_layout->addLayout(m_btn_layout);

    setLayout(m_main_layout);

    if (GlobalSettings::getInstance()->isExist(REMOTE_SERVER_REMOTE_IP)) {
        QStringList uriList = GlobalSettings::getInstance()->getValue(REMOTE_SERVER_REMOTE_IP).toStringList();
        for (auto uri = uriList.constBegin(); uri != uriList.constEnd(); ++uri) {
            QUrl url(*uri);
            if ("" != *uri) {
                QString urit = *uri == url.toDisplayString() ? *uri : url.toDisplayString();
                QListWidgetItem* item = new QListWidgetItem;
                item->setText(urit);
                m_favorite_uri += urit;
                m_favorite_list->addItem(item);
                m_favorite_widgets.insert(urit, item);
            }
        }

        syncUri();
    }

    connect(m_btn_add, &QPushButton::clicked, this, [=] (bool checked) {
        addUri(uri());        
        Q_UNUSED(checked);
    });

    connect(m_remote_type_edit, &QComboBox::currentTextChanged, this, [=] (const QString& type) {
        if ("samba" == type.toLower()) {
            m_port_editor->setEditText("445");
        } else if ("ftp" == type.toLower()) {
            m_port_editor->setEditText("21");
        }
    });
    Q_EMIT m_remote_type_edit->currentTextChanged("ftp");

    connect(m_btn_del, &QPushButton::clicked, this, [=] (bool checked) {
        removeUri(uri());
        if (m_favorite_uri.count() <= 0) {
            m_favorite_list->clear();
        } else {
            setUri(*(m_favorite_uri.begin()));
            m_favorite_list->setCurrentItem(m_favorite_widgets[*(m_favorite_uri.begin())]);
        }
        m_favorite_list->update();
        Q_UNUSED(checked);
    });

    connect(m_favorite_list, &QListWidget::itemClicked, this, [=] (QListWidgetItem *item) {
        setUri(item->text());
    });

    connect(m_btn_conn, &QPushButton::clicked, this, [=] (bool checked) {
        if ("" != uri()) {
            accept();
        }

        Q_UNUSED(checked);
    });
}

ConnectServerDialog::~ConnectServerDialog()
{

}

QString ConnectServerDialog::uri()
{
    QString uuri = "";

    if (m_remote_type_edit->currentText() == "samba") {
        uuri = "smb://" + m_ip_edit->text() + ":" + m_port_editor->currentText();
    } else if (m_remote_type_edit->currentText() == "ftp") {
        uuri = "ftp://" + m_ip_edit->text() + ":" + m_port_editor->currentText();
    }else if (m_remote_type_edit->currentText() == "sftp") {
        uuri = "sftp://" + m_ip_edit->text() + ":" + m_port_editor->currentText();
    }

    return uuri;
}

QString ConnectServerDialog::getIP()
{
    return m_ip_edit->text();
}

void ConnectServerDialog::syncUri()
{
    GlobalSettings::getInstance()->setValue(REMOTE_SERVER_REMOTE_IP, QStringList(m_favorite_uri.values()));
    GlobalSettings::getInstance()->forceSync(REMOTE_SERVER_REMOTE_IP);

}

void ConnectServerDialog::setUri(QString uri)
{
    QUrl        rl   = uri;
    QString     port = QString::number(rl.port());
    QString     schema = rl.scheme();

    if ("smb" == schema) {
        m_remote_type_edit->setCurrentText("samba");
    } else {
        m_remote_type_edit->setCurrentText(rl.scheme());
    }

    m_ip_edit->setText(rl.host());
    m_port_editor->setCurrentText(port);
}

void ConnectServerDialog::addUri(QString uri)
{
    bool canInsert = false;
    QUrl url(uri);

    // fixme:// fix
    if (!m_favorite_uri.contains(uri) && !m_favorite_uri.contains(url.toDisplayString())) {
        canInsert = true;
    }

    if (canInsert) {
        m_favorite_uri.insert(url.toDisplayString());

        QListWidgetItem* item = new QListWidgetItem;
        item->setText(url.toDisplayString());
        m_favorite_list->addItem(item);
        m_favorite_widgets.insert(url.toDisplayString(), item);
        m_favorite_list->setCurrentItem(item);

        syncUri();
        GlobalSettings::getInstance()->slot_updateRemoteServer(url.toDisplayString(), true);
    }
}

void ConnectServerDialog::removeUri(QString uri)
{
    QUrl url (uri);
    QString removeUrl = uri;

    if (m_favorite_uri.contains(uri)) {
        removeUrl = uri;
    } else if (m_favorite_uri.contains(url.toDisplayString())) {
        removeUrl = url.toDisplayString();
    }

    if (m_favorite_uri.contains(removeUrl)) {
        m_favorite_uri.remove(removeUrl);
        syncUri();
        GlobalSettings::getInstance()->slot_updateRemoteServer(removeUrl,false);
    }

    if (m_favorite_widgets.contains(removeUrl)) {
        QListWidgetItem* item = m_favorite_widgets[removeUrl];
        m_favorite_list->removeItemWidget(item);
        m_favorite_widgets.remove(removeUrl);
        delete item;
    }
}

ConnectServerLogin::ConnectServerLogin(QString remoteIP, QWidget *parent) : QDialog(parent),m_remoteIP(remoteIP)
{
    setFixedSize(m_widget_size);
    setWindowIcon(QIcon::fromTheme("network-server"));
    setWindowTitle(tr("The login user"));
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
    m_main_layout = new QVBoxLayout(this);
    m_main_layout->addSpacing(12);
    m_main_layout->setMargin(m_widget_margin);

    m_tip                   = new QLabel;
    m_tip->setWordWrap(true);
    m_tip->setText(QString(tr("Please enter the %1's user name and password of the server.")).arg(remoteIP));
    m_main_layout->addWidget(m_tip);

    m_usr_label             = new QLabel;
    m_usr_btn_group         = new QVBoxLayout;
    m_usr_btn_guest         = new QRadioButton;
    m_usr_btn_usr           = new QRadioButton;
    m_usr_layout            = new QHBoxLayout;

    m_usr_label->setText(tr("User's identity"));
    m_usr_btn_guest->setText(tr("guest"));
    m_usr_btn_usr->setText(tr("Registered users"));
    m_usr_btn_group->addWidget(m_usr_btn_guest);
    m_usr_btn_group->addWidget(m_usr_btn_usr);
    m_usr_layout->addWidget(m_usr_label);
    m_usr_layout->addLayout(m_usr_btn_group);
    m_usr_label->setAlignment (Qt::AlignTop | Qt::AlignLeft);
    m_usr_layout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    m_main_layout->addLayout(m_usr_layout);

    m_reg_usr_name_label    = new QLabel;
    m_reg_usr_passwd_label  = new QLabel;
    m_reg_usr_name_editor   = new QLineEdit;
    m_reg_usr_passwd_editor = new QLineEdit;
    m_reg_usr_combox        = new QCheckBox;
    m_reg_usr_layout        = new QGridLayout;

    m_reg_usr_name_label->setText(tr("name"));
    m_reg_usr_passwd_label->setText(tr("password"));
    m_reg_usr_combox->setText(tr("Remember the password"));
    m_reg_usr_name_label->setFixedHeight(36);
    m_reg_usr_passwd_label->setFixedHeight(36);
    m_reg_usr_name_editor->setFixedHeight(36);
    m_reg_usr_passwd_editor->setFixedHeight(36);
    m_reg_usr_combox->setFixedHeight(36);

    m_reg_usr_passwd_editor->setEchoMode(QLineEdit::Password);
    m_reg_usr_layout->addWidget(m_reg_usr_name_label,       0, 0);
    m_reg_usr_layout->addWidget(m_reg_usr_name_editor,      0, 1);
    m_reg_usr_layout->addWidget(m_reg_usr_passwd_label,     1, 0);
    m_reg_usr_layout->addWidget(m_reg_usr_passwd_editor,    1, 1);
    m_reg_usr_layout->addWidget(m_reg_usr_combox,           2, 1);
    m_reg_usr_layout->setVerticalSpacing(12);
    m_main_layout->addLayout(m_reg_usr_layout);

    m_btn_cancel            = new QPushButton;
    m_btn_ok                = new QPushButton;
    m_btn_layout            = new QHBoxLayout;
    m_btn_layout->addSpacing(192);
    m_btn_cancel->setText(tr("cancel"));
    m_btn_ok->setText(tr("ok"));
    m_btn_layout->addWidget(m_btn_cancel);
    m_btn_layout->addWidget(m_btn_ok);
    m_main_layout->addSpacing(20);
    m_btn_ok->setAutoDefault(true);
    m_btn_cancel->setAutoDefault(false);
    m_main_layout->addLayout(m_btn_layout);

    setLayout(m_main_layout);

    m_usr_btn_usr->setChecked(true);

    connect(m_usr_btn_guest, &QRadioButton::clicked, [=] () {
        setFixedSize(m_widget_size_little);
        m_reg_usr_combox->setHidden(true);
        m_reg_usr_name_label->setHidden(true);
        m_reg_usr_name_editor->setHidden(true);
        m_reg_usr_passwd_label->setHidden(true);
        m_reg_usr_passwd_editor->setHidden(true);
    });

    connect(m_usr_btn_usr, &QRadioButton::clicked, [=] () {
        setFixedSize(m_widget_size);
        // FIXME://
        m_reg_usr_combox->setHidden(true);
        m_reg_usr_name_label->setHidden(false);
        m_reg_usr_name_editor->setHidden(false);
        m_reg_usr_passwd_label->setHidden(false);
        m_reg_usr_passwd_editor->setHidden(false);
    });

    connect(m_btn_cancel, &QPushButton::clicked, [=] () {

        slot_syncRemoteServer();
        close();
    });

    connect(m_btn_ok, &QPushButton::clicked, [=] () {
        accept();
    });
}

ConnectServerLogin::~ConnectServerLogin()
{

}

QString ConnectServerLogin::user()
{
    return m_reg_usr_name_editor->text();
}

QString ConnectServerLogin::domain()
{
    return "WORKGROUP";
}

QString ConnectServerLogin::password()
{
    return m_reg_usr_passwd_editor->text();
}

bool ConnectServerLogin::anonymous()
{
    return m_usr_btn_guest->isChecked() ? true : false;
}

bool ConnectServerLogin::savePassword()
{
    return m_reg_usr_combox->isChecked();
}

void ConnectServerLogin::slot_syncRemoteServer()
{
    if (GlobalSettings::getInstance()->isExist(REMOTE_SERVER_REMOTE_IP)) {
        QStringList uriList = GlobalSettings::getInstance()->getValue(REMOTE_SERVER_REMOTE_IP).toStringList();

        if(!uriList.contains(m_remoteIP)){
            uriList.append(m_remoteIP);

            GlobalSettings::getInstance()->setValue(REMOTE_SERVER_REMOTE_IP,uriList);
            GlobalSettings::getInstance()->forceSync(REMOTE_SERVER_REMOTE_IP);
            GlobalSettings::getInstance()->slot_updateRemoteServer(m_remoteIP, true);
        }
    }
}

