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

#include "permissions-properties-page.h"

#include "linux-pwd-helper.h"
#include "file-watcher.h"

#include <glib.h>
#include <glib/gstdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <QVBoxLayout>
#include <QTableWidget>
#include <QHeaderView>
#include <QLabel>
#include <QCheckBox>

#include <QUrl>

#include <QDebug>

using namespace Peony;

#define OWNER 0
#define GROUP 1
#define OTHERS 2
#define USER 0

#define READABLE 2
#define WRITEABLE 3
#define EXECUTEABLE 4

PermissionsPropertiesPage::PermissionsPropertiesPage(const QStringList &uris, QWidget *parent) : QWidget(parent)
{
    m_uri = uris.first();

    auto layout = new QVBoxLayout(this);
    this->setLayout(layout);
    m_table = new QTableWidget(this);
    m_table->setRowCount(4);
    m_table->setColumnCount(5);
    m_table->verticalHeader()->setVisible(false);
    m_table->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_table->horizontalHeader()->setFrameShape(QFrame::NoFrame);
    m_table->horizontalHeader()->setSelectionMode(QTableWidget::NoSelection);
    m_table->setSelectionMode(QTableWidget::NoSelection);
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    auto l = QStringList();
    l<<tr("User or Group")<<tr("Type")<<tr("Readable")<<tr("Writeable")<<tr("Excuteable");
    m_table->setHorizontalHeaderLabels(l);
    m_table->setEditTriggers(QTableWidget::NoEditTriggers);

    m_label = new QLabel(tr("File: %1").arg(m_uri), this);
    m_message = new QLabel(this);
    m_message->setAlignment(Qt::AlignCenter);
    layout->addWidget(m_label);
    layout->addWidget(m_table);
    layout->addWidget(m_message);

    m_message->setVisible(false);

    m_watcher = std::make_shared<FileWatcher>(m_uri);
    connect(m_watcher.get(), &FileWatcher::locationChanged, this, &PermissionsPropertiesPage::queryPermissionsAsync);
    connect(this, &PermissionsPropertiesPage::checkBoxChanged, this, &PermissionsPropertiesPage::changePermission);

    queryPermissionsAsync(nullptr, m_uri);
}

PermissionsPropertiesPage::~PermissionsPropertiesPage()
{

}

void PermissionsPropertiesPage::queryPermissionsAsync(const QString &, const QString &uri)
{
    m_uri = uri;
    m_label->setText(m_uri);
    m_table->setEnabled(false);

    GFile *file = g_file_new_for_uri(m_uri.toUtf8().constData());
    g_file_query_info_async(file,
                            "owner::*," "access::*," "unix::mode",
                            G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS,
                            0,
                            nullptr,
                            GAsyncReadyCallback(async_query_permisson_callback),
                            this);
    g_object_unref(file);
}

GAsyncReadyCallback PermissionsPropertiesPage::async_query_permisson_callback(GObject *obj, GAsyncResult *res, PermissionsPropertiesPage *p_this)
{
    GError *err = nullptr;
    auto info = g_file_query_info_finish(G_FILE(obj),
                                         res,
                                         &err);

    if (!info) {
        if (p_this) {
            p_this->m_table->setVisible(false);
            p_this->m_message->setText(tr("Can not get the permission info."));
            p_this->m_message->setVisible(true);
        }
    }

    if (err) {
        qDebug()<<err->message;
        if (p_this) {
            p_this->m_table->setVisible(false);
            p_this->m_message->setText(tr("Can not get the permission info."));
            p_this->m_message->setVisible(true);
        }
        g_error_free(err);
    }

    if (info) {
        if (p_this) {
            bool enable = true;
            auto table = p_this->m_table;
            auto user = g_file_info_get_attribute_string(info, G_FILE_ATTRIBUTE_OWNER_USER);
            auto owner = g_file_info_get_attribute_string(info, G_FILE_ATTRIBUTE_OWNER_USER_REAL);
            QString userString = user;
            QString groupName = g_file_info_get_attribute_string(info, G_FILE_ATTRIBUTE_OWNER_GROUP);
            QString userNameDisplayString = owner;

            bool current_user_readable = g_file_info_get_attribute_boolean(info, G_FILE_ATTRIBUTE_ACCESS_CAN_READ);
            bool current_user_writeable = g_file_info_get_attribute_boolean(info, G_FILE_ATTRIBUTE_ACCESS_CAN_WRITE);
            bool current_user_executeable = g_file_info_get_attribute_boolean(info, G_FILE_ATTRIBUTE_ACCESS_CAN_EXECUTE);

            bool has_unix_mode = g_file_info_has_attribute(info, G_FILE_ATTRIBUTE_UNIX_MODE);
            guint32 mode = 0;
            if (has_unix_mode)
                mode = g_file_info_get_attribute_uint32(info, G_FILE_ATTRIBUTE_UNIX_MODE);

            auto owner_readable = mode & S_IRUSR;
            p_this->m_permissions[0][0] = owner_readable;
            auto owner_writeable = mode & S_IWUSR;
            p_this->m_permissions[0][1] = owner_writeable;
            auto owner_executeable = mode & S_IXUSR;
            p_this->m_permissions[0][2] = owner_executeable;

            auto group_readable = mode & S_IRGRP;
            p_this->m_permissions[1][0] = group_readable;
            auto group_writeable = mode & S_IWGRP;
            p_this->m_permissions[1][1] = group_writeable;
            auto group_executeable = mode & S_IXGRP;
            p_this->m_permissions[1][2] = group_executeable;

            auto other_readable = mode & S_IROTH;
            p_this->m_permissions[2][0] = other_readable;
            auto other_writeable = mode & S_IWOTH;
            p_this->m_permissions[2][1] = other_writeable;
            auto other_executeable = mode & S_IXOTH;
            p_this->m_permissions[2][2] = other_executeable;

            qDebug()<<current_user_readable<<current_user_writeable<<current_user_executeable;

            uid_t uid = geteuid();
            struct passwd *pw = getpwuid(uid);
            if( pw ) {
                bool isSelf = false;
                bool isSameGroup = false;
                auto username = pw->pw_name;
                if (userString == username) {
                    userNameDisplayString += tr("(Me)");
                    isSelf = true;
                }
                /*
                if (userNameDisplayString.isEmpty())
                    userNameDisplayString = tr("Unkwon");
                if (groupName == pw->pw_gecos)
                    isSameGroup = true;
                if (groupName.isEmpty())
                    groupName = tr("Unkwon");
                    */

                if (!isSelf && !isSameGroup)
                    enable = false;

                if (pw->pw_uid == 0) {
                    enable = true;
                }
            } else {
                enable = false;
            }

            if (enable) {
                table->setRowCount(3);

                for (int i = 0; i < 3; i++) {
                    for (int j = 0; j < 3; j++) {
                        table->setCellWidget(i, j + 2, nullptr);
                        QWidget *w = new QWidget(table);
                        QHBoxLayout *l = new QHBoxLayout(w);
                        l->setMargin(0);
                        w->setLayout(l);
                        l->setAlignment(Qt::AlignCenter);
                        auto checkbox = new QCheckBox(w);
                        l->addWidget(checkbox);
                        table->setCellWidget(i, j + 2, w);

                        checkbox->setChecked(p_this->m_permissions[i][j]);

                        connect(checkbox, &QCheckBox::clicked, p_this, [=](){
                            qDebug()<<"clicked"<<i<<j<<checkbox->isChecked();
                            p_this->checkBoxChanged(i, j, checkbox->isChecked());
                        });
                    }
                }

                QTableWidgetItem *itemR0C0 = new QTableWidgetItem(QIcon::fromTheme("emblem-personal"), userNameDisplayString);
                table->setItem(0, 0, nullptr);
                table->setItem(0, 0, itemR0C0);

                QTableWidgetItem *itemR1C0 = new QTableWidgetItem(QIcon::fromTheme("emblem-people"), groupName);
                table->setItem(1, 0, nullptr);
                table->setItem(1, 0, itemR1C0);

                QTableWidgetItem *itemR2C0 = new QTableWidgetItem(QIcon::fromTheme("emblem-people"), tr("Others"));
                table->setItem(2, 0, nullptr);
                table->setItem(2, 0, itemR2C0);

                auto itemR0C1 = new QTableWidgetItem(tr("Owner"));
                itemR0C1->setTextAlignment(Qt::AlignCenter);
                auto itemR1C1 = new QTableWidgetItem(tr("Group"));
                itemR1C1->setTextAlignment(Qt::AlignCenter);
                auto itemR2C1 = new QTableWidgetItem(tr("Other Users"));
                itemR2C1->setTextAlignment(Qt::AlignCenter);

                table->setItem(0, 1, itemR0C1);
                table->setItem(1, 1, itemR1C1);
                table->setItem(2, 1, itemR2C1);

                table->showRow(0);
                table->showRow(1);
                table->showRow(2);
            } else {
                p_this->m_message->setText(tr("You can not change the access of this file."));
                p_this->m_message->show();
                table->setRowCount(1);

                QTableWidgetItem *itemR0C0 = new QTableWidgetItem(QIcon::fromTheme("emblem-personal"), tr("Me"));
                table->setItem(0, 0, nullptr);
                table->setItem(0, 0, itemR0C0);

                auto itemR0C1 = new QTableWidgetItem(tr("User"));
                itemR0C1->setTextAlignment(Qt::AlignCenter);

                table->setItem(0, 1, itemR0C1);

                for (int i = 0; i < 3; i++) {
                    table->setCellWidget(0, i + 2, nullptr);
                    QWidget *w = new QWidget(table);
                    QHBoxLayout *l = new QHBoxLayout(w);
                    l->setMargin(0);
                    w->setLayout(l);
                    l->setAlignment(Qt::AlignCenter);
                    auto checkbox = new QCheckBox(w);
                    l->addWidget(checkbox);
                    table->setCellWidget(0, i + 2, w);

                    switch (i) {
                    case 0:
                        checkbox->setChecked(current_user_readable);
                        break;
                    case 1:
                        checkbox->setChecked(current_user_writeable);
                        break;
                    case 2:
                        checkbox->setChecked(current_user_executeable);
                        break;
                    }
                }
            }

            table->setEnabled(enable);
        }

        g_object_unref(info);
    }
    return nullptr;
}

void PermissionsPropertiesPage::changePermission(int row, int column, bool checked)
{
    /*!
      \bug
      even though directory know the file's attributes have been changed, and
      model request updated the data, the view doesn't paint the current emblems correctly.
      */
    //FIXME: should use g_file_set_attribute() with mode info?
    m_permissions[row][column] = checked;

    mode_t mod = 0;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            bool b = m_permissions[i][j];
            if (b) {
                int rc = i*10 + j;
                switch (rc) {
                case 0: {
                    mod |= S_IRUSR;
                    break;
                }
                case 1: {
                    mod |= S_IWUSR;
                    break;
                }
                case 2: {
                    mod |= S_IXUSR;
                    break;
                }
                case 10: {
                    mod |= S_IRGRP;
                    break;
                }
                case 11: {
                    mod |= S_IWGRP;
                    break;
                }
                case 12: {
                    mod |= S_IXGRP;
                    break;
                }
                case 20: {
                    mod |= S_IROTH;
                    break;
                }
                case 21: {
                    mod |= S_IWOTH;
                    break;
                }
                case 22: {
                    mod |= S_IXOTH;
                    break;
                }
                }
            }
        }
    }

    QUrl url = m_uri;
    if (url.isLocalFile()) {
        g_chmod(url.path().toUtf8(), mod);
        qDebug()<<mod;
    }
}
