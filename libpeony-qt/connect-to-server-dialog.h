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

#ifndef CONNECTTOSERVERDIALOG_H
#define CONNECTTOSERVERDIALOG_H

#include <QLabel>
#include <QWidget>
#include <QWidget>
#include <QDialog>
#include <QCheckBox>
#include <QComboBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QListWidget>
#include <QRadioButton>
#include "peony-core_global.h"

namespace Peony {

class PEONYCORESHARED_EXPORT ConnectServerDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ConnectServerDialog(QWidget *parent = nullptr);
    ~ConnectServerDialog();

    QString uri();
    QString getIP();

private:
    void setUri(QString uri);

private:
    float           m_widget_margin         = 24;
    QSize           m_widget_size           = QSize (424, 440);
    QVBoxLayout*    m_main_layout           = nullptr;
    QStringList*    m_remote_type;
    QStringList*    m_remote_port;

    QPoint          m_remote_type_pos       = QPoint (24, 70);
    QSize           m_remote_type_size      = QSize (28, 24);
    QLabel*         m_remote_type_label     = nullptr;
    QComboBox*      m_remote_type_edit      = nullptr;
    QLabel*         m_ip_label              = nullptr;
    QLineEdit*      m_ip_edit               = nullptr;
    QLabel*         m_port_label            = nullptr;
    QComboBox*      m_port_editor           = nullptr;
    QGridLayout*    m_remote_layout         = nullptr;

    QSize           m_favorite_list_size    = QSize (376, 144);
    QLabel*         m_favorite_label        = nullptr;
    QListWidget*    m_favorite_list         = nullptr;
    QVBoxLayout*    m_favorite_layout       = nullptr;

    QPushButton*    m_btn_add               = nullptr;
    QPushButton*    m_btn_del               = nullptr;
    QPushButton*    m_btn_conn              = nullptr;
    QHBoxLayout*    m_btn_layout            = nullptr;

    QSet<QString>                   m_favorite_uri;
    QMap<QString, QListWidgetItem*> m_favorite_widgets;
};

class PEONYCORESHARED_EXPORT ConnectServerLogin : public QDialog
{
    friend class MountOperation;
    Q_OBJECT
public:
    explicit ConnectServerLogin(QString remoteIP, QWidget* parent = nullptr);
    ~ConnectServerLogin();

    QString user();
    QString domain();
    QString password();
    bool anonymous();
    bool savePassword();

private:
    float           m_widget_margin         = 24;
    QSize           m_widget_size           = QSize (424, 394);
    QSize           m_widget_size_little    = QSize (424, 250);
    QVBoxLayout*    m_main_layout           = nullptr;

    QLabel*         m_tip                   = nullptr;

    QLabel*         m_usr_label             = nullptr;
    QVBoxLayout*    m_usr_btn_group         = nullptr;
    QRadioButton*   m_usr_btn_guest         = nullptr;
    QRadioButton*   m_usr_btn_usr           = nullptr;
    QHBoxLayout*    m_usr_layout            = nullptr;

    QLabel*         m_reg_usr_name_label    = nullptr;
    QLabel*         m_reg_usr_passwd_label  = nullptr;
    QLineEdit*      m_reg_usr_name_editor   = nullptr;
    QLineEdit*      m_reg_usr_passwd_editor = nullptr;
    QCheckBox*      m_reg_usr_combox        = nullptr;
    QGridLayout*    m_reg_usr_layout        = nullptr;

    QPushButton*    m_btn_cancel            = nullptr;
    QPushButton*    m_btn_ok                = nullptr;
    QHBoxLayout*    m_btn_layout            = nullptr;
};
};
#endif // CONNECTTOSERVERDIALOG_H
