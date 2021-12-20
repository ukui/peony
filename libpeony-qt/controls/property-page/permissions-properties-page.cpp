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
 * Authors: Yue Lan <lanyue@kylinos.cn>
 *
 */

#include "permissions-properties-page.h"

#include "linux-pwd-helper.h"
#include "file-watcher.h"
#include "file-info.h"
#include "file-info-job.h"

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
#include <QProcess>
#include <QPushButton>

#include <QUrl>
#include <QStandardPaths>

#include <QDebug>
#include <QFileInfo>

using namespace Peony;

#define OWNER 0
#define GROUP 1
#define OTHERS 2
#define USER 0

#define READABLE 2
#define WRITEABLE 3
#define EXECUTEABLE 4
//460 - 22 - 22 = 416 ,右侧有字符被遮挡，再减去6px -_-;
#define TARGET_LABEL_WIDTH 410

PermissionsPropertiesPage::PermissionsPropertiesPage(const QStringList &uris, QWidget *parent) : PropertiesWindowTabIface(parent)
{
    m_uri = uris.first();
    QUrl url(m_uri);

    //note:请查看：BasicPropertiesPage::getFIleInfo(QString uri) - Look BasicPropertiesPage::getFIleInfo(QString uri)
    if (m_uri.startsWith("favorite://")) {
        m_uri = "file://" + url.path();
        url = QUrl(m_uri);
    }

    m_layout = new QVBoxLayout(this);
    m_layout->setContentsMargins(0,0,0,0);
    this->setLayout(m_layout);

    m_label = new QLabel(this);

    QString str = tr("Target: %1").arg(url.path());
    int fontSize = m_label->fontMetrics().width(str);

    if(fontSize > TARGET_LABEL_WIDTH) {
        m_label->setToolTip(str);
        str = m_label->fontMetrics().elidedText(str, Qt::ElideMiddle, TARGET_LABEL_WIDTH);
    }
    m_label->setText(str);

    m_label->setMinimumHeight(60);
    m_label->setContentsMargins(22,0,22,0);

    m_layout->addWidget(m_label);

    m_message = new QLabel(this);
    m_message->setAlignment(Qt::AlignCenter);

    this->initTableWidget();

    m_layout->addWidget(m_message);
    m_message->setVisible(false);

    m_watcher = std::make_shared<FileWatcher>(m_uri);
    connect(m_watcher.get(), &FileWatcher::locationChanged, this, &PermissionsPropertiesPage::queryPermissionsAsync);
    connect(this, &PermissionsPropertiesPage::checkBoxChanged, this, &PermissionsPropertiesPage::changePermission);

    queryPermissionsAsync(nullptr, m_uri);
}

PermissionsPropertiesPage::~PermissionsPropertiesPage()
{

}

void PermissionsPropertiesPage::initTableWidget()
{
    m_table = new QTableWidget(this);
    m_table->setRowCount(4);
    m_table->setColumnCount(5);
    m_table->verticalHeader()->setVisible(false);
    m_table->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_table->horizontalHeader()->setFrameShape(QFrame::NoFrame);
    m_table->setFrameShape(QFrame::NoFrame);
    m_table->horizontalHeader()->setSelectionMode(QTableWidget::NoSelection);
    m_table->setSelectionMode(QTableWidget::NoSelection);
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_table->setShowGrid(false);

    m_table->horizontalHeader()->setMinimumHeight(34);
    m_table->rowHeight(34);

    m_table->setAlternatingRowColors(true);

    auto l = QStringList();
    l<<tr("User or Group")<<tr("Type")<<tr("Read")<<tr("Write")<<tr("Executable");
    m_table->setHorizontalHeaderLabels(l);
    m_table->setEditTriggers(QTableWidget::NoEditTriggers);
    //开启手动设置宽度 - Enable manual width setting
    m_table->horizontalHeader()->setMinimumSectionSize(30);
    m_table->horizontalHeader()->setMaximumSectionSize(400);

    m_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Interactive);
    m_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Interactive);
    m_table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    m_table->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    m_table->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);
    m_table->horizontalHeaderItem(0)->setTextAlignment(Qt::AlignLeft);

    m_table->setColumnWidth(0, 150);
    m_layout->addWidget(m_table);   
}

void PermissionsPropertiesPage::queryPermissionsAsync(const QString &, const QString &uri)
{
    m_uri = uri;
    QUrl url = m_uri;
    m_label->setText(m_label->fontMetrics().elidedText(tr("Target: %1").arg(url.path()), Qt::ElideMiddle,TARGET_LABEL_WIDTH));
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
    auto info = g_file_query_info_finish(G_FILE(obj), res, &err);

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
            //auto owner = g_file_info_get_attribute_string(info, G_FILE_ATTRIBUTE_OWNER_USER_REAL);
            QString userString = user;
            QString groupName = g_file_info_get_attribute_string(info, G_FILE_ATTRIBUTE_OWNER_GROUP);
            QString userNameDisplayString = user;

            bool current_user_readable = g_file_info_get_attribute_boolean(info, G_FILE_ATTRIBUTE_ACCESS_CAN_READ);
            bool current_user_writeable = g_file_info_get_attribute_boolean(info, G_FILE_ATTRIBUTE_ACCESS_CAN_WRITE);
            bool current_user_executable = g_file_info_get_attribute_boolean(info, G_FILE_ATTRIBUTE_ACCESS_CAN_EXECUTE);

            p_this->m_has_unix_mode = g_file_info_has_attribute(info, G_FILE_ATTRIBUTE_UNIX_MODE);
            guint32 mode = 0;
            if (p_this->m_has_unix_mode)
                mode = g_file_info_get_attribute_uint32(info, G_FILE_ATTRIBUTE_UNIX_MODE);

            auto owner_readable  = mode & S_IRUSR;
            auto owner_writeable = mode & S_IWUSR;
            auto owner_executable = mode & S_IXUSR;

            //read
            p_this->m_permissions[0][0] = owner_readable;
            //write
            p_this->m_permissions[0][1] = owner_writeable;
            //executable
            p_this->m_permissions[0][2] = owner_executable;

            auto group_readable  = mode & S_IRGRP;
            auto group_writeable = mode & S_IWGRP;
            auto group_executable = mode & S_IXGRP;

            p_this->m_permissions[1][0] = group_readable;
            p_this->m_permissions[1][1] = group_writeable;
            p_this->m_permissions[1][2] = group_executable;

            auto other_readable  = mode & S_IROTH;
            auto other_writeable = mode & S_IWOTH;
            auto other_executable = mode & S_IXOTH;

            p_this->m_permissions[2][0] = other_readable;
            p_this->m_permissions[2][1] = other_writeable;
            p_this->m_permissions[2][2] = other_executable;

            qDebug()<<current_user_readable<<current_user_writeable<<current_user_executable;

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

                if (!isSelf && !isSameGroup) {
                    qDebug()<<"the uid not permit";
                    enable = false;
                }

                if (pw->pw_uid == 0) {
                    QFileInfo file("/usr/sbin/security-switch");
                    if(file.exists() == true) {
                        QProcess shProcess;
                        shProcess.start("security-switch --get");
                        if (!shProcess.waitForStarted()) {
                            qDebug()<<"wait get security state start timeout";
                        } else {
                            if (!shProcess.waitForFinished()) {
                                qDebug()<<"wait get security state finshed timeout";
                            } else {
                                QString secState = shProcess.readAllStandardOutput();
                                qDebug()<<"security-switch get test "<< secState;
                                if (secState.contains("strict")) {
                                    qDebug()<<"now it is in strict mode, so root is not super";
                                } else {
                                    qDebug()<<"pw uid is 0, it is super";
                                    enable = true;
                                }
                            }
                        }
                    } else {
                        qDebug()<<"security-switch is not support, so it is super";
                        enable = true;
                    }
                   /*
                    if (!kysec_is_disabled() && kysec_get_3adm_status()) {
                        qDebug()<<"now it is in strict mode, so root is not super";
                    } else {
                        qDebug()<<"pw uid is 0, it is super";
                        enable = true;
                    }*/
                }

            } else {
                enable = false;
            }

            if (enable) {
                table->setRowCount(3);
                //更新表格选中情况
                p_this->updateCheckBox();

                table->setItem(0, 0, nullptr);
                QTableWidgetItem* itemR0C0 = new QTableWidgetItem(QIcon::fromTheme("emblem-personal"), userNameDisplayString);
                table->setItem(0, 0, itemR0C0);

                table->setItem(1, 0, nullptr);
                QTableWidgetItem* itemR1C0 = new QTableWidgetItem(QIcon::fromTheme("emblem-people"), groupName);
                table->setItem(1, 0, itemR1C0);

                table->setItem(2, 0, nullptr);
                QTableWidgetItem* itemR2C0 = new QTableWidgetItem(QIcon::fromTheme("emblem-people"), tr("Others"));
                table->setItem(2, 0, itemR2C0);

                auto itemR0C1 = new QTableWidgetItem(tr("Owner"));
                itemR0C1->setTextAlignment(Qt::AlignCenter);
                auto itemR1C1 = new QTableWidgetItem(tr("Group"));
                itemR1C1->setTextAlignment(Qt::AlignCenter);
                auto itemR2C1 = new QTableWidgetItem(tr("Other"));
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
                        checkbox->setChecked(current_user_executable);
                        break;
                    }
                }
            }

            table->setEnabled(enable);
            //防止误修改
            p_this->m_enable = enable;
        }

        g_object_unref(info);
    }
    return nullptr;
}

void PermissionsPropertiesPage::changePermission(int row, int column, bool checked)
{
    if(!m_enable)
        return;

    m_permissions[row][column] = checked;

    this->thisPageChanged();

    this->updateCheckBox();
}

/*!
 * update file ermissions
 * \brief PermissionsPropertiesPage::savePermissions
 */
void PermissionsPropertiesPage::savePermissions()
{
    /*!
      \bug
      even though directory know the file's attributes have been changed, and
      model request updated the data, the view doesn't paint the current emblems correctly.
    */
    //FIXME: should use g_file_set_attribute() with mode info?
    if(!m_enable)
        return;

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

    if (m_has_unix_mode) {
        g_autoptr(GFile) pfile = g_file_new_for_uri(m_uri.toUtf8().constData());
        g_file_set_attribute_uint32(pfile, G_FILE_ATTRIBUTE_UNIX_MODE, (guint32)mod, G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS, nullptr, nullptr);
    }

    auto fileInfo = FileInfo::fromUri(m_uri);
    FileInfoJob *job = new FileInfoJob(fileInfo);
    job->setAutoDelete(true);
    job->querySync();
}

void PermissionsPropertiesPage::saveAllChange()
{
    if(this->m_thisPageChanged)
        this->savePermissions();
    qDebug() << "PermissionsPropertiesPage::saveAllChange()" << this->m_thisPageChanged;
}

void PermissionsPropertiesPage::thisPageChanged()
{
    this->m_thisPageChanged = true;
}

void PermissionsPropertiesPage::updateCheckBox()
{
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            m_table->setCellWidget(i, j + 2, nullptr);
            QWidget *w = new QWidget(m_table);
            QHBoxLayout *l = new QHBoxLayout(w);
            l->setMargin(0);
            w->setLayout(l);
            l->setAlignment(Qt::AlignCenter);
            auto checkbox = new QCheckBox(w);
            l->addWidget(checkbox);
            m_table->setCellWidget(i, j + 2, w);

            checkbox->setChecked(this->m_permissions[i][j]);

            //disable home path
            bool check_enable = true;
            QString uri = m_uri;

            if(uri.startsWith("filesafe:///")){
                QStringList list = uri.split("/");
                if(list.size()==4){
                    check_enable = false;
                }
            }

            QString homeUri = "file://" +  QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
            if (this->m_uri == homeUri || !check_enable)
                checkbox->setDisabled(true);
            else
                checkbox->setDisabled(false);

            connect(checkbox, &QCheckBox::clicked, this, [=]() {
                qDebug()<<"clicked"<<i<<j<<checkbox->isChecked();
                this->checkBoxChanged(i, j, checkbox->isChecked());
            });
        }
    }
}

QWidget *PermissionsPropertiesPage::createCellWidget(QWidget *parent, QIcon icon, QString text)
{
    QWidget *widget = new QWidget(parent);
    QHBoxLayout *layout = new QHBoxLayout(widget);
    layout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    layout->setContentsMargins(22, 0, 0, 0);
    //组件间距 - Widget spacing
    layout->setSpacing(9);

    QPushButton *cellIcon = new QPushButton(widget);
    cellIcon->setStyleSheet("QPushButton{"
                        "border-radius: 8px; "
                        "background-color: transparent;"
                        "max-width:16px;"
                        "max-height:16px;"
                        "min-width:16px;"
                        "min-height:16px;"
                        "}");
    cellIcon->setEnabled(false);

    cellIcon->setIcon(icon);
    cellIcon->setIconSize(QSize(16, 16));

    layout->addWidget(cellIcon);

    QLabel *label = new QLabel(widget);
    label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    QFontMetrics fontMetrics = label->fontMetrics();
    int fontSize = fontMetrics.width(text);
    QString str = text;
    //widget宽度200px;设计稿在左边空出22px;icon宽度16px，icon右侧9px;
    //200-22-16-9 = 153 , widget：剩下的3px给右侧留空 -_-；
    if(fontSize > 150) {
        widget->setToolTip(text);
        str = fontMetrics.elidedText(text, Qt::ElideRight, 150);
    }
    label->setText(str);

    layout->addWidget(label);

    return widget;
}
