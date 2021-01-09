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

#include "basic-properties-page.h"
#include "thumbnail-manager.h"

#include <QVBoxLayout>
#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QFormLayout>

#include <QDateTime>

#include "file-info.h"
#include "file-info-job.h"
#include "file-utils.h"
#include "file-operation-utils.h"
#include "file-watcher.h"

#include "file-count-operation.h"

#include <QThreadPool>
#include <QFileInfo>

//#include <QMessageBox>

#include <QUrl>

#include <QFileDialog>
#include "file-meta-info.h"

#include "generic-thumbnailer.h"
#include <gio/gdesktopappinfo.h>

using namespace Peony;

BasicPropertiesPage::BasicPropertiesPage(const QStringList &uris, QWidget *parent) : QWidget(parent)
{
    //qDebug() << "BasicPropertiesPage:" <<uris.count() <<uris.first();
    if (uris.count() == 1) {
        m_watcher = std::make_shared<FileWatcher>(uris.first());
        m_watcher->connect(m_watcher.get(), &FileWatcher::locationChanged, this, &BasicPropertiesPage::onSingleFileChanged);
        m_watcher->startMonitor();

        m_thumbnail_watcher = std::make_shared<FileWatcher>("thumbnail:///");
        connect(m_thumbnail_watcher.get(), &FileWatcher::fileChanged, this, [=](const QString &uri){
            auto icon = ThumbnailManager::getInstance()->tryGetThumbnail(uri);
            m_icon->setIcon(icon);
            //QMessageBox::information(0, 0, "icon updated");
        });
    }

    //FIXME: complete the content
    QVBoxLayout *layout = new QVBoxLayout(this);
    m_layout = layout;
    layout->setMargin(0);
    layout->setSpacing(0);

    setLayout(layout);

    QFrame *f1 = new QFrame(this);
    auto l1 = new QGridLayout(f1);
    f1->setLayout(l1);
    QPushButton *icon = new QPushButton(f1);
    m_icon = icon;

    if (uris.count() == 1) {
        m_icon->connect(m_icon, &QPushButton::clicked, this, [=](){
            QUrl iconPathUrl;
            iconPathUrl.setPath("/usr/share/icons");
            auto picture = QFileDialog::getOpenFileName(nullptr, tr("Choose a custom icon"), "/usr/share/icons", "*.png *.jpg *.jpeg *.svg");
            auto metaInfo = FileMetaInfo::fromUri(uris.first());
            QFileInfo fileInfo(picture);
            if (!QIcon::fromTheme(fileInfo.baseName()).isNull())
                metaInfo.get()->setMetaInfoString("custom-icon", fileInfo.baseName());
            else
                metaInfo.get()->setMetaInfoString("custom-icon", picture);

            ThumbnailManager::getInstance()->createThumbnail(uris.first(), m_thumbnail_watcher, true);
        });
    }

    m_type = new QLabel(f1);
    QLineEdit *edit = new QLineEdit(f1);
    m_display_name = edit;
    QLabel *location = new QLabel(f1);
    m_location = location;

    icon->setFixedSize(QSize(72, 72));
    icon->setIconSize(QSize(64, 64));
    icon->setProperty("isIcon", true);
    bool singleUri = uris.count() == 1;
    if (singleUri) {
        this->onSingleFileChanged(nullptr, uris.first());
    } else {
        icon->setIcon(QIcon::fromTheme("text-x-generic"));
    }
    l1->addWidget(icon, 0, 0, 2, 1);

    auto form = new QFormLayout(f1);
    l1->addLayout(form, 0, 1);

    form->addRow(tr("Type:"), m_type);
    form->addRow(tr("Display Name:"), m_display_name);
    form->addRow(tr("Location:"), m_location);

    if (singleUri) {
        edit->setText(m_info->displayName());
    } else {
        QStringList l;
        for (auto uri : uris) {
            //FIXME: replace BLOCKING api in ui thread.
            l<<FileUtils::getFileDisplayName(uri);
        }
        auto text = l.join(",");
        edit->setText(text);
    }
    edit->setReadOnly(!singleUri);
    //edit->setContentsMargins(10, 10, 10, 5);
    //l1->addWidget(edit, 0, 1);

    connect(edit, &QLineEdit::returnPressed, [=]() {
        if (!edit->isReadOnly() && !edit->text().isEmpty()) {
            FileOperationUtils::rename(m_info->uri(), edit->text(), true);
        }
    });

    location->setTextInteractionFlags(Qt::TextSelectableByMouse);
    QUrl url = FileUtils::getParentUri(uris.first());
    location->setText(url.toDisplayString());
    //location->setContentsMargins(15, 5, 10, 10);
    //l1->addWidget(location, 1, 1);

    layout->addWidget(f1);
    addSeparator();

    /*
    QLabel *l2 = new QLabel("FIXME:\n"
                            "type\n"
                            "open\n"
                            "size\n", this);
                            */
    QFrame *f2 = new QFrame(this);
    QVBoxLayout *l2 = new QVBoxLayout(f2);
    f2->setLayout(l2);
    m_file_count_label = new QLabel(f2);
    m_total_size_label = new QLabel(f2);
    l2->addWidget(new QLabel(tr("Overview:")));
    l2->addWidget(m_file_count_label);
    l2->addWidget(m_total_size_label);

    layout->addWidget(f2);

    countFilesAsync(uris);

    if (uris.count() == 1) {
        addSeparator();
    }
    /*
    QLabel *l3 = new QLabel("FIXME:\n"
                            "created\n"
                            "modified\n"
                            "accessed", this);
                            */
    auto f3 = new QFrame(this);
    form = new QFormLayout(f3);
    m_form3 = form;
    f3->setLayout(form);
    m_time_created_label = new QLabel(f3);
    m_time_modified_label = new QLabel(f3);
    m_time_access_label = new QLabel(f3);
    form->addRow(tr("Time Created:"), m_time_created_label);
    form->addRow(tr("Time Modified:"), m_time_modified_label);
    form->addRow(tr("Time Access:"), m_time_access_label);
    layout->addWidget(f3);
    f3->setVisible(uris.count() == 1);
    updateInfo(uris.first());
    connect(m_watcher.get(), &FileWatcher::locationChanged, [=](const QString&, const QString &uri) {
        this->updateInfo(uri);
    });

    addSeparator();

    QLabel *l4 = new QLabel(this);
    layout->addWidget(l4, 1);
}

BasicPropertiesPage::~BasicPropertiesPage()
{
    disconnect();
    cancelCount();
}

void BasicPropertiesPage::addSeparator()
{
    QFrame *separator = new QFrame(this);
    separator->setFrameShape(QFrame::HLine);
    m_layout->addWidget(separator);
}

QIcon generateThumbnail(const QString &uri)
{
    QUrl url = uri;
    if (! uri.startsWith("file:///")) {
        url = FileUtils::getTargetUri(uri);
    }

    auto _desktop_file = g_desktop_app_info_new_from_filename(url.path().toUtf8().constData());
    auto _icon_string = g_desktop_app_info_get_string(_desktop_file, "Icon");
    QIcon thumbnail = QIcon::fromTheme(_icon_string);
    QString string = _icon_string;
    if (thumbnail.isNull() && string.startsWith("/")) {
        thumbnail = GenericThumbnailer::generateThumbnail(_icon_string, true);
    }
    qDebug() <<"BasicPropertiesPage generateThumbnail thumbnail:" <<thumbnail <<_icon_string;
    g_free(_icon_string);
    g_object_unref(_desktop_file);

    return thumbnail;
}

void BasicPropertiesPage::onSingleFileChanged(const QString &oldUri, const QString &newUri)
{
    //QMessageBox::information(0, 0, "on single file changed");
    qDebug()<<"onSingleFileChanged:"<<oldUri<<newUri;
    //FIXME: replace BLOCKING api in ui thread.
    m_info = FileInfo::fromUri(newUri);
    FileInfoJob *j = new FileInfoJob(m_info);
    j->setAutoDelete();
    j->querySync();
    ThumbnailManager::getInstance()->createThumbnail(m_info->uri(), m_thumbnail_watcher);

    auto icon = QIcon::fromTheme(m_info->iconName(), QIcon::fromTheme("text-x-generic"));
    auto thumbnail = ThumbnailManager::getInstance()->tryGetThumbnail(m_info->uri());

    //qDebug() << "infoupdate set Icon:" <<thumbnail.isNull() <<thumbnail;
    m_icon->setIcon(thumbnail.isNull()? icon: thumbnail);
    m_display_name->setText(m_info->displayName());
    m_type->setText(m_info->fileType());
    //auto icon = QIcon::fromTheme(m_info->iconName(), QIcon::fromTheme("text-x-generic"));
    //auto thumbnail = ThumbnailManager::getInstance()->tryGetThumbnail(m_info->uri());
    if (thumbnail.isNull())
    {
        ThumbnailManager::getInstance()->createThumbnail(m_info->uri(), m_thumbnail_watcher);
    }
    //qDebug() << "set Icon:" <<thumbnail.isNull() <<thumbnail;
    m_icon->setIcon(thumbnail.isNull()? icon: thumbnail);
    QUrl url = FileUtils::getParentUri(m_info->uri());

    qDebug() << "=========================new uri:" << newUri << "  ---  " << oldUri << "  " << m_info->uri() << "  " << url.toDisplayString();
    m_location->setText(url.toDisplayString());
}

void BasicPropertiesPage::countFilesAsync(const QStringList &uris)
{
    //old op will delete later
    if (m_count_op) {
        m_count_op->disconnect();
        m_count_op->cancel();
    }
    //clear old data
    m_file_count = 0;
    m_hidden_file_count = 0;
    m_total_size = 0;
    m_count_op = new FileCountOperation(uris);
    m_count_op->setAutoDelete(true);
    connect(m_count_op, &FileOperation::operationPreparedOne, this, &BasicPropertiesPage::onFileCountOne, Qt::BlockingQueuedConnection);
    connect(m_count_op, &FileCountOperation::countDone, [=](quint64 file_count, quint64 hidden_file_count, quint64 total_size) {
        m_count_op = nullptr;
        m_file_count = file_count;
        m_hidden_file_count = hidden_file_count;
        m_total_size = total_size;
        this->updateCountInfo();
    });

    QThreadPool::globalInstance()->start(m_count_op);
}

void BasicPropertiesPage::onFileCountOne(const QString &uri, quint64 size)
{
    m_file_count++;
    if (uri.contains("/.")) {
        m_hidden_file_count++;
    }
    m_total_size += size;
    updateCountInfo();
}

void BasicPropertiesPage::cancelCount()
{
    if (m_count_op)
        m_count_op->cancel();
}

void BasicPropertiesPage::updateCountInfo()
{
    m_file_count_label->setText(tr("%1 files (include root files), %2 hidden").arg(m_file_count).arg(m_hidden_file_count));
   // auto format = g_format_size(m_total_size);
   
   //Calculated by 1024 bytes
    auto format = g_format_size_full(m_total_size,G_FORMAT_SIZE_IEC_UNITS);
    m_total_size_label->setText(tr("%1 total").arg(format));
    g_free(format);
}

void BasicPropertiesPage::updateInfo(const QString &uri)
{
    //FIXME: replace BLOCKING api in ui thread.
    m_info = FileInfo::fromUri(uri);

    GFile *file = g_file_new_for_uri(uri.toUtf8().constData());
    GFileInfo *info = g_file_query_info(file,
                                        "time::*",
                                        G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS,
                                        nullptr,
                                        nullptr);
    g_object_unref(file);

    m_time_created = g_file_info_get_attribute_uint64(info,
                     G_FILE_ATTRIBUTE_TIME_CREATED);
    QDateTime date1 = QDateTime::fromMSecsSinceEpoch(m_time_created*1000);
    QString time1 = date1.toString(Qt::SystemLocaleShortDate);
    m_time_created_label->setText(time1);
    m_form3->itemAt(0, QFormLayout::LabelRole)->widget()->setVisible(m_time_created != 0);
    m_form3->itemAt(0, QFormLayout::FieldRole)->widget()->setVisible(m_time_created != 0);

    //folder don't show access time
    if (m_info->isDir())
    {
        m_form3->itemAt(2, QFormLayout::LabelRole)->widget()->setVisible(false);
        m_form3->itemAt(2, QFormLayout::FieldRole)->widget()->setVisible(false);
    }

    m_time_modified = g_file_info_get_attribute_uint64(info,
                      "time::modified");
    QDateTime date2 = QDateTime::fromMSecsSinceEpoch(m_time_modified*1000);
    QString time2 = date2.toString(Qt::SystemLocaleShortDate);
    m_time_modified_label->setText(time2);

    m_time_access = g_file_info_get_attribute_uint64(info,
                    "time::access");
    QDateTime date3 = QDateTime::fromMSecsSinceEpoch(m_time_access*1000);
    QString time3 = date3.toString(Qt::SystemLocaleShortDate);
    m_time_access_label->setText(time3);

    g_object_unref(info);
}
