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

#include <openssl/aes.h>
#include <glib.h>

using namespace Peony;
static const QString ftpTypeStr="ftp";
static const QString sftpTypeStr="sftp";
static const QString sambaTypeStr="samba";
static const QString ftpDefaultPortStr="21";
static const QString sftpDefaultPortStr="22";
static const QString sambaDefaultPortStr="445";

static QString passwdEncode (QString p);
static QString passwdDecode (QString p);

static ButtonStyle *global_instance = nullptr;

ButtonStyle *ButtonStyle::getStyle()
{
    if (!global_instance) {
        global_instance = new ButtonStyle;
    }
    return global_instance;
}

void ButtonStyle::drawControl(QStyle::ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    switch (element) {
    case CE_PushButton:
    {
        if (const QStyleOptionButton *button = qstyleoption_cast<const QStyleOptionButton *>(option)) {
            proxy()->drawControl(CE_PushButtonBevel, option, painter, widget);
            QStyleOptionButton subopt = *button;
            subopt.rect = proxy()->subElementRect(SE_PushButtonContents, option, widget);
            proxy()->drawControl(CE_PushButtonLabel, &subopt, painter, widget);
            return;
        }
        break;
    }

    default:
        break;
    }
    QProxyStyle::drawControl(element, option, painter, widget);
}

int ButtonStyle::pixelMetric(QStyle::PixelMetric metric, const QStyleOption *option, const QWidget *widget) const
{
    switch (metric) {
    case PM_ButtonMargin:
    {
        return 0;
    }

    default:
        return QProxyStyle::pixelMetric(metric, option, widget);
    }
}

QRect ButtonStyle::subElementRect(SubElement element, const QStyleOption *option, const QWidget *widget) const
{
    switch (element) {
    case SE_PushButtonContents:
    {
        if (const QStyleOptionButton *button = qstyleoption_cast<const QStyleOptionButton *>(option)) {
            const bool icon = !button->icon.isNull();
            const bool text = !button->text.isEmpty();
            QRect rect = option->rect;
            int Margin_Height = 2;
            int ToolButton_MarginWidth = 10;
            int Button_MarginWidth = proxy()->pixelMetric(PM_ButtonMargin, option, widget);
            if (text && !icon && !(button->features & QStyleOptionButton::HasMenu)) {
                rect.adjust(Button_MarginWidth, 0, -Button_MarginWidth, 0);
            } else if (!text && icon && !(button->features & QStyleOptionButton::HasMenu)) {

            } else {
                rect.adjust(ToolButton_MarginWidth, Margin_Height, -ToolButton_MarginWidth, -Margin_Height);
            }
            if (button->features & (QStyleOptionButton::AutoDefaultButton | QStyleOptionButton::DefaultButton)) {
                int dbw = proxy()->pixelMetric(PM_ButtonDefaultIndicator, option, widget);
                rect.adjust(dbw, dbw, -dbw, -dbw);
            }
            return rect;
        }
        break;
    }

    default:
        break;
    }

    return QProxyStyle::subElementRect(element, option, widget);
}


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

    m_remote_type->append(ftpTypeStr);
    m_remote_type->append(sftpTypeStr);
    m_remote_type->append(sambaTypeStr);
    m_remote_port->append("20");
    m_remote_port->append(ftpDefaultPortStr);
    m_remote_port->append(sftpDefaultPortStr);
    m_remote_port->append(sambaDefaultPortStr);
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
    m_btn_conn->setStyle(ButtonStyle::getStyle());
    m_main_layout->addLayout(m_btn_layout);

    setLayout(m_main_layout);

    if (GlobalSettings::getInstance()->isExist(REMOTE_SERVER_REMOTE_IP)) {

        QMap<QString, QVariant> uriList = GlobalSettings::getInstance()->getValue(REMOTE_SERVER_REMOTE_IP).toMap();
        for (auto uri = uriList.constBegin(); uri != uriList.constEnd(); ++uri) {
            QUrl url(uri.key ());
            if ("" != uri.key ()) {
                QString urit = uri.key () == url.toDisplayString() ? uri.key() : url.toDisplayString();
                QListWidgetItem* item = new QListWidgetItem;
                item->setText(urit);

                m_favorite_uri.insert (urit, uri.value ());
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
        if (sambaTypeStr == type.toLower()) {
            m_port_editor->setEditText(sambaDefaultPortStr);
        } else if (ftpTypeStr == type.toLower()) {
            m_port_editor->setEditText(ftpDefaultPortStr);
        }else if (sftpTypeStr == type.toLower()) {
            m_port_editor->setEditText(sftpDefaultPortStr);
        }
    });
    Q_EMIT m_remote_type_edit->currentTextChanged(ftpTypeStr);

    connect(m_btn_del, &QPushButton::clicked, this, [=] (bool checked) {
        removeUri(uri());
        if (m_favorite_uri.count() <= 0) {
            m_favorite_list->clear();
        } else {
            setUri((m_favorite_uri.firstKey ()));
            m_favorite_list->setCurrentItem(m_favorite_widgets[m_favorite_uri.firstKey ()]);
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

    if (m_remote_type_edit->currentText() == sambaTypeStr) {
        uuri = "smb://" + m_ip_edit->text() + ":" + m_port_editor->currentText();
    } else if (m_remote_type_edit->currentText() == ftpTypeStr) {
        uuri = "ftp://" + m_ip_edit->text() + ":" + m_port_editor->currentText();
    }else if (m_remote_type_edit->currentText() == sftpTypeStr) {
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
    GlobalSettings::getInstance()->setValue(REMOTE_SERVER_REMOTE_IP, m_favorite_uri);
    GlobalSettings::getInstance()->forceSync(REMOTE_SERVER_REMOTE_IP);

}

void ConnectServerDialog::setUri(QString uri)
{
    QUrl        rl   = uri;
    QString     port = QString::number(rl.port());
    QString     schema = rl.scheme();

    if ("smb" == schema) {
        m_remote_type_edit->setCurrentText(sambaTypeStr);
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
        m_favorite_uri.insert(url.toDisplayString(), QMap<QString, QVariant>());

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

ConnectServerLogin::ConnectServerLogin(QString uri, QWidget *parent)
    : QDialog(parent),m_remoteIP(uri)
{
    setFixedSize(m_widget_size);
    setWindowIcon(QIcon::fromTheme("network-server"));
    setWindowTitle(tr("The login user"));
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
    m_main_layout = new QVBoxLayout(this);
    m_main_layout->addSpacing(12);
    m_main_layout->setMargin(m_widget_margin);

    QUrl url(uri);
    m_tip                   = new QLabel;
    m_tip->setWordWrap(true);
    m_tip->setText(QString(tr("Please enter the %1's user name and password of the server.")).arg(url.host ()));
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
    m_reg_usr_name_editor   = new QComboBox;
    m_reg_usr_passwd_editor = new QLineEdit;
    m_reg_usr_combox        = new QCheckBox;
    m_reg_usr_layout        = new QGridLayout;

    m_reg_usr_name_editor->setEditable (true);
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
    m_reg_usr_combox->setChecked (false);

    QMap<QString, QVariant> uriList = GlobalSettings::getInstance()->getValue(REMOTE_SERVER_REMOTE_IP).toMap();
    QString portStr = QString::number(url.port());
    QString type = url.scheme();
    if (portStr.toInt() < 0) {
        if (ftpTypeStr == type.toLower()) {
            portStr = ftpDefaultPortStr;
        } else if (sftpTypeStr == type.toLower()) {
            portStr = sftpDefaultPortStr;
        } else if ("smb"==type.toLower()) {/* samba服务是smb */
            portStr = sambaDefaultPortStr;
        }
    }

    QString remoteUri= type.append("://").append(url.host()).append(":").append(portStr);

    if (uriList.contains (remoteUri)) {
        QMap<QString, QVariant> userInfo = uriList[remoteUri].toMap ();
        if (!userInfo.isEmpty ()) {
            m_userInfo = userInfo;
            for (auto u : userInfo.keys ()) {
                m_reg_usr_name_editor->addItem (u);
            }

            // set default passwd
            QString du = m_reg_usr_name_editor->currentText ();
            if (m_userInfo.contains (du)) {
                m_reg_usr_passwd_editor->setText (passwdDecode (m_userInfo[du].toByteArray ()));
                m_reg_usr_combox->setChecked(true);
            }
        }
    }

    connect (m_reg_usr_name_editor, &QComboBox::currentTextChanged, this, [=] (const QString& u) {
        if (m_userInfo.contains (u) && !m_userInfo[u].toString ().isEmpty ()) {
            m_reg_usr_passwd_editor->setText (passwdDecode (m_userInfo[u].toByteArray ()));
            m_reg_usr_combox->setChecked(true);
        }
    });

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
        m_reg_usr_combox->setHidden(false);
        m_reg_usr_name_label->setHidden(false);
        m_reg_usr_name_editor->setHidden(false);
        m_reg_usr_passwd_label->setHidden(false);
        m_reg_usr_passwd_editor->setHidden(false);
    });

    connect (m_reg_usr_combox, &QCheckBox::clicked, this, [=] (bool checked) {
        if (!checked) {
            // FIXME:// 是否清楚已保存的用户名和密码?
        }
    });

    connect(m_btn_cancel, &QPushButton::clicked, [=] () {
        close();
    });

    connect(m_btn_ok, &QPushButton::clicked, [=] () {
        accept();
        //QUrl url(m_remoteIP);
        //syncRemoteServer(url);
    });
}

ConnectServerLogin::~ConnectServerLogin()
{

}

QString ConnectServerLogin::user()
{
    return m_reg_usr_name_editor->currentText ();
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

void ConnectServerLogin::syncRemoteServer(const QUrl& url)
{
    if (GlobalSettings::getInstance()->isExist(REMOTE_SERVER_REMOTE_IP)) {

        QMap<QString, QVariant> uriList = GlobalSettings::getInstance()->getValue(REMOTE_SERVER_REMOTE_IP).toMap();
        QString portStr = QString::number(url.port());
        QString type = url.scheme();
        if(portStr.toInt()< 0)
        {
            if(ftpTypeStr==type.toLower()){
                portStr = ftpDefaultPortStr;
            }else if(sftpTypeStr==type.toLower()){
                portStr = sftpDefaultPortStr;
            }else if("smb"==type.toLower()){/* samba服务是smb */
                portStr = sambaDefaultPortStr;
            }
        }

        if("smb"==type.toLower() && !url.path().isEmpty()){/* samba的子项挂载登录成功 */
            GlobalSettings::getInstance()->slot_updateRemoteServer(url.toString(), true);
            return;
        }

        QString remoteUri= type.append("://").append(url.host()).append(":").append(portStr);
        QMap<QString, QVariant> userInfo;
        if (!uriList.contains (remoteUri)) {
            if (savePassword () && !m_reg_usr_passwd_editor->text().isEmpty ()) {
                userInfo.insert (m_reg_usr_name_editor->currentText (), passwdEncode (m_reg_usr_passwd_editor->text().toUtf8 ()));
            }

            uriList.insert (remoteUri, userInfo);
            GlobalSettings::getInstance()->slot_updateRemoteServer(remoteUri, true);
        } else {
            userInfo = uriList[remoteUri].toMap ();
            if (savePassword ()  && !m_reg_usr_passwd_editor->text().isEmpty ()) {
                userInfo[m_reg_usr_name_editor->currentText ()] = passwdEncode (m_reg_usr_passwd_editor->text().toUtf8 ());
            } /*else {
                if (userInfo.contains (m_reg_usr_name_editor->currentText ())) {
                    userInfo.remove (m_reg_usr_name_editor->currentText ());
                }
            }*/
            uriList[remoteUri] = userInfo;
        }

        GlobalSettings::getInstance()->setValue(REMOTE_SERVER_REMOTE_IP,uriList);
        GlobalSettings::getInstance()->forceSync(REMOTE_SERVER_REMOTE_IP);
    }
}


static const unsigned char PEONY_AES_KEY[] = "peony key";

static QString passwdEncode (QString p)
{
    g_return_val_if_fail (!p.isEmpty (), "");

    unsigned char aesKey[AES_BLOCK_SIZE] = {0};
    unsigned char ivc[AES_BLOCK_SIZE] = {0};

    int passwdLength = p.toUtf8().size () + 1;
    if (passwdLength % AES_BLOCK_SIZE) passwdLength += (passwdLength % AES_BLOCK_SIZE);
    g_autofree unsigned char* tmp = (unsigned char*) g_malloc0 (passwdLength);

    memcpy (tmp, p.toUtf8 ().constData (), p.toUtf8 ().size ());

    int encslength = ((passwdLength + AES_BLOCK_SIZE) / AES_BLOCK_SIZE) * AES_BLOCK_SIZE;
    g_autofree unsigned char* encodeOut = (unsigned char*) g_malloc0 (encslength + 1);

    memset (ivc, 0, sizeof (ivc));
    AES_KEY encKey;
    memcpy (aesKey, PEONY_AES_KEY, sizeof (PEONY_AES_KEY));
    AES_set_encrypt_key (aesKey, 128, &encKey);
    AES_cbc_encrypt (tmp, encodeOut, p.toUtf8 ().size (), &encKey, ivc, AES_ENCRYPT);

    return QString("%1|%2").arg (p.toUtf8 ().size ()).arg (QString (QByteArray::fromRawData ((char*) encodeOut, encslength).toBase64 ()));
}

static QString passwdDecode (QString p)
{
    g_return_val_if_fail (!p.isEmpty (), "");

    QStringList ls = p.split ("|");

    g_return_val_if_fail (ls.length () == 2, "");

    unsigned char aesKey[AES_BLOCK_SIZE] = {0};
    unsigned char ivc[AES_BLOCK_SIZE] = {0};

    int srcPasswdLength = ls.first ().toInt ();
    QByteArray data = QByteArray::fromBase64 (ls.last ().toUtf8 ());
    int encodePasswdLength = data.length ();

    if (encodePasswdLength % AES_BLOCK_SIZE) encodePasswdLength += (encodePasswdLength % AES_BLOCK_SIZE);
    g_autofree unsigned char* tmp = (unsigned char*) g_malloc0 (encodePasswdLength);

    memcpy (tmp, data.constData (), data.length ());

    g_autofree unsigned char* decodeOut = (unsigned char*) g_malloc0 (srcPasswdLength + 1);

    memset (ivc, 0, sizeof (ivc));
    memcpy (aesKey, PEONY_AES_KEY, sizeof (PEONY_AES_KEY));

    AES_KEY decKey;
    AES_set_decrypt_key (aesKey, 128, &decKey);
    AES_cbc_encrypt (tmp, decodeOut, srcPasswdLength, &decKey, ivc, AES_DECRYPT);

    return (char*) decodeOut;
}
