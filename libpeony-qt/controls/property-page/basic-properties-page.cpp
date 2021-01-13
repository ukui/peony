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
#include <file-launch-action.h>

#include <QDateTime>

#include "file-info.h"
#include "file-info-job.h"
#include "file-utils.h"
#include "file-operation-utils.h"
#include "file-watcher.h"

#include "file-count-operation.h"

#include <QThreadPool>
#include <QFileInfo>
#include <QGraphicsOpacityEffect>
#include <file-launch-manager.h>

//#include <QMessageBox>

#include <QUrl>

#include <QFileDialog>
#include "file-meta-info.h"

#include "generic-thumbnailer.h"
#include <gio/gdesktopappinfo.h>

using namespace Peony;

BasicPropertiesPage::BasicPropertiesPage(const QStringList &uris, QWidget *parent) : QWidget(parent)
{
    //check file type and search fileinfo
    FileType l_fileType = this->checkFileType(uris);
    //qDebug() << "BasicPropertiesPage:" <<uris.count() <<uris.first();
    if (l_fileType != BP_MultipleFIle) {
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
    m_layout = new QVBoxLayout(this);
    m_layout->setMargin(0);
    m_layout->setSpacing(0);

    this->setLayout(m_layout);

    this->initFloorOne(uris,l_fileType);
    this->addSeparator();

    this->initFloorTwo(uris,l_fileType);
    this->addSeparator();

    //    countFilesAsync(uris);

    //    if (uris.count() == 1) {
    //        addSeparator();
    //    }

    //    auto f3 = new QFrame(this);
    //    QFormLayout *form = new QFormLayout(f3);
    //    m_form3 = form;
    //    f3->setLayout(form);
    //    m_time_created_label = new QLabel(f3);
    //    m_time_modified_label = new QLabel(f3);
    //    m_time_access_label = new QLabel(f3);
    //    form->addRow(tr("Time Created:"), m_time_created_label);
    //    form->addRow(tr("Time Modified:"), m_time_modified_label);
    //    form->addRow(tr("Time Access:"), m_time_access_label);
    //    m_layout->addWidget(f3);
    //    f3->setVisible(uris.count() == 1);
    //    updateInfo(uris.first());
    //    connect(m_watcher.get(), &FileWatcher::locationChanged, [=](const QString&, const QString &uri) {
    //        this->updateInfo(uri);
    //    });

    //    addSeparator();

    //    QLabel *l4 = new QLabel(this);
    //    m_layout->addWidget(l4, 1);
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

void BasicPropertiesPage::addOpenWithMenu(QWidget *parent)
{
    auto recommendActions = FileLaunchManager::getRecommendActions(m_info.get()->uri());
    if (m_openWith && recommendActions.count() >= 1)
    {
        QLabel *appName = new QLabel(parent);
        appName->setText(recommendActions.first()->getAppInfoDisplayName());
        m_openWith->addWidget(appName);
    }
}

/*!
 *
 * \brief BasicPropertiesPage::initFloorOne
 */
void BasicPropertiesPage::initFloorOne(const QStringList &uris,FileType fileType)
{
    QFrame      *floor1  = new QFrame(this);
    QGridLayout *layout1 = new QGridLayout(floor1);
    floor1->setMaximumHeight(100);
    floor1->setLayout(layout1);

    m_icon = new QPushButton(floor1);
    m_displayName = new QLineEdit(floor1);
    m_location    = new QLineEdit(floor1);

    m_icon->setFixedSize(QSize(60,60));
    m_icon->setIconSize(QSize(32,32));
    m_icon->setProperty("isIcon", true);

    auto form1 = new QFormLayout(floor1);
    form1->setContentsMargins(8,6,8,0);
    layout1->addLayout(form1,0,0);

    form1->addRow(m_icon);

    //icon area
    if (fileType != BP_MultipleFIle) {
        connect(m_icon, &QPushButton::clicked, this, &BasicPropertiesPage::changeFileIcon);
        this->onSingleFileChanged(nullptr, uris.first());
    } else {
        m_icon->setIcon(QIcon::fromTheme("text-x-generic"));
    }

    //text area
    auto form2 = new QFormLayout(floor1);
    layout1->addLayout(form2, 0, 1);

    m_displayName->setMinimumHeight(32);
    m_location->setMinimumHeight(32);

    form2->addRow(tr("Display Name"), m_displayName);
    form2->addRow(tr("Location"), m_location);

    if (fileType == BP_MultipleFIle)
        m_displayName->setReadOnly(true);

    //new thread get fileName
    FileNameThread *l_thread = new FileNameThread(uris);
    l_thread->start();

    connect(l_thread,&FileNameThread::fileNameReady,this,[=](QString fileName){
        m_displayName->setText(fileName);
        delete l_thread;
    });

    connect(m_displayName, &QLineEdit::returnPressed, [=]() {
        if (!m_displayName->isReadOnly() && !m_displayName->text().isEmpty()) {
            FileOperationUtils::rename(m_info->uri(), m_displayName->text(), true);
        }
    });

    m_location->setReadOnly(true);
    QUrl url = FileUtils::getParentUri(uris.first());
    m_location->setText(url.toDisplayString());

    //move -button
    if(fileType == BP_Folder) {
        m_moveButton = new QPushButton(floor1);

        auto form3 = new QFormLayout(floor1);
        form3->setContentsMargins(0,2,0,0);

        m_moveButton->setText("移动");
        m_moveButton->setFixedSize(70,32);

        QPushButton *hiddenButton = new QPushButton(floor1);
        hiddenButton->setFixedSize(70,32);
        //设置透明隐藏按钮
        QGraphicsOpacityEffect *op = new QGraphicsOpacityEffect(floor1);
        op->setOpacity(0);
        hiddenButton->setGraphicsEffect(op);

        form3->addRow(hiddenButton);
        form3->addRow(m_moveButton);

        layout1->addLayout(form3,0,2);
    }
    //add floor1 to context
    m_layout->addWidget(floor1);
}

void BasicPropertiesPage::initFloorTwo(const QStringList &uris,FileType fileType)
{
    QFrame      *floor2  = new QFrame(this);
    QFormLayout *layout2 = new QFormLayout(floor2);
    layout2->setContentsMargins(16,16,0,0);
    floor2->setLayout(layout2);

    m_fileType      = new QLabel(floor2);
    m_fileSize      = new QLabel(floor2);
    m_fileTotalSize = new QLabel(floor2);

    layout2->addRow(tr("Type:"),m_fileType);

    //根据文件类型添加组件
    switch (fileType) {
    case BP_Folder:
        m_fileType->setText("文件夹");
        m_folderContain = new QLabel(floor2);
        layout2->addRow(tr("Include:"),m_folderContain);
        break;
    case BP_File:
        m_openWith = new QHBoxLayout(floor2);
        layout2->addRow(tr("Open with:"),m_openWith);
        this->addOpenWithMenu(floor2);
        break;
    case BP_Application:
        m_fileType->setText("应用程序");
        m_descrption = new QLabel(floor2);
        layout2->addRow(tr("Descrption:"),m_descrption);
        m_descrption->setText(m_info.get()->displayName());
        break;
    case BP_MultipleFIle:
        m_fileType->setText("选中多个文件");
    default:
        break;
    }

    layout2->addRow(tr("Size:"),m_fileSize);
    layout2->addRow(tr("Total size:"),m_fileTotalSize);

    this->countFilesAsync(uris);

    m_layout->addWidget(floor2);
}

BasicPropertiesPage::FileType BasicPropertiesPage::checkFileType(const QStringList &uris)
{
    if(uris.count() != 1) {
        return BP_MultipleFIle;
    } else {
        std::shared_ptr<FileInfo> fileInfo = FileInfo::fromUri(uris.first());
        FileInfoJob *j = new FileInfoJob(fileInfo);
        j->setAutoDelete();
        j->querySync();

        m_info = fileInfo;

        if(fileInfo.get()->isDir())
            return BP_Folder;
        if(fileInfo.get()->isDesktopFile())
            return BP_Application;
        return BP_File;
    }
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
    //    //FIXME: replace BLOCKING api in ui thread.
    if(oldUri != nullptr){
        m_info = FileInfo::fromUri(newUri);
        FileInfoJob *j = new FileInfoJob(m_info);
        j->setAutoDelete();
        j->querySync();
    }

    ThumbnailManager::getInstance()->createThumbnail(m_info.get()->uri(), m_thumbnail_watcher);
    auto icon = QIcon::fromTheme(m_info.get()->iconName(), QIcon::fromTheme("text-x-generic"));
    auto thumbnail = ThumbnailManager::getInstance()->tryGetThumbnail(m_info.get()->uri());

    m_icon->setIcon(thumbnail.isNull()? icon: thumbnail);
    m_displayName->setText(m_info.get()->displayName());

    if (thumbnail.isNull())
    {
        ThumbnailManager::getInstance()->createThumbnail(m_info.get()->uri(), m_thumbnail_watcher);
    }

    m_icon->setIcon(thumbnail.isNull()? icon : thumbnail);
    QUrl url = FileUtils::getParentUri(m_info.get()->uri());

    m_location->setText(url.toDisplayString());
}

void BasicPropertiesPage::countFilesAsync(const QStringList &uris)
{
//    return;
    //old op will delete later
    if (m_count_op) {
        m_count_op->disconnect();
        m_count_op->cancel();
    }
    //clear old data
    m_folderContainFiles   = 0;
    m_folderContainFolders = 0;
    m_fileSizeCount        = 0;
    m_fileTotalSizeCount   = 0;

    m_count_op = new FileCountOperation(uris);
    m_count_op->setAutoDelete(true);

    connect(m_count_op, &FileOperation::operationPreparedOne, this, &BasicPropertiesPage::onFileCountOne, Qt::BlockingQueuedConnection);

    //old version
//    connect(m_count_op, &FileCountOperation::countDone, [=](quint64 file_count, quint64 hidden_file_count, quint64 total_size) {
    //new version
    connect(m_count_op, &FileCountOperation::countDoneTwo, [=](quint64 file_count, quint64 total_size, quint64 folder_count) {
        m_count_op = nullptr;
        m_folderContainFiles = file_count;
        m_folderContainFolders = folder_count;
        m_fileSizeCount = total_size;
        this->updateCountInfo();
    });

    QThreadPool::globalInstance()->start(m_count_op);
}

void BasicPropertiesPage::onFileCountOne(const QString &uri, quint64 size)
{
//    return;
    qDebug() << "统计到一个文件，大小："<<uri << size;
        m_folderContainFiles++;
        m_fileSizeCount += size;
        updateCountInfo();
}

void BasicPropertiesPage::cancelCount()
{
    if (m_count_op)
        m_count_op->cancel();
}

void BasicPropertiesPage::changeFileIcon(){
    QUrl iconPathUrl;
    iconPathUrl.setPath("/usr/share/icons");
    auto picture = QFileDialog::getOpenFileName(nullptr, tr("Choose a custom icon"), "/usr/share/icons", "*.png *.jpg *.jpeg *.svg");
    auto metaInfo = FileMetaInfo::fromUri(m_info.get()->uri());
    QFileInfo fileInfo(picture);
    if (!QIcon::fromTheme(fileInfo.baseName()).isNull())
        metaInfo.get()->setMetaInfoString("custom-icon", fileInfo.baseName());
    else
        metaInfo.get()->setMetaInfoString("custom-icon", picture);

    ThumbnailManager::getInstance()->createThumbnail(m_info.get()->uri(), m_thumbnail_watcher, true);
}

void BasicPropertiesPage::updateCountInfo()
{
//    return;
    QString fileSizeStr = QString::number(m_fileSizeCount/1024/1024) + " MB" + "(" + QString::number(m_fileSizeCount) + " 字节)";
    m_fileSize->setText(fileSizeStr);

    if(m_folderContain){
        QString fileNumStr = tr("%1 files, %2 folders").arg(m_folderContainFiles).arg((m_folderContainFolders ==0 ? 0 : (m_folderContainFolders - 1)));
        m_folderContain->setText(fileNumStr);
    }

//    m_file_count_label->setText(tr("%1 files (include root files), %2 hidden").arg(m_file_count).arg(m_hidden_file_count));
//    // auto format = g_format_size(m_total_size);

//    //Calculated by 1024 bytes
//    auto format = g_format_size_full(m_total_size,G_FORMAT_SIZE_IEC_UNITS);
//    m_total_size_label->setText(tr("%1 total").arg(format));
//    g_free(format);
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

void FileNameThread::run()
{

    QString fileName = "";
    if (m_uris.count() == 1) {
        std::shared_ptr<FileInfo> fileInfo = FileInfo::fromUri(m_uris.first());
        FileInfoJob *j = new FileInfoJob(fileInfo);
        j->setAutoDelete();
        j->querySync();
        fileName = fileInfo.get()->displayName();
    } else {
        QStringList l;
        for (auto uri : m_uris) {
            //FIXME: replace BLOCKING api in ui thread.(finish) **
            std::shared_ptr<FileInfo> fileInfo = FileInfo::fromUri(uri);
            FileInfoJob *j = new FileInfoJob(fileInfo);
            j->setAutoDelete();
            j->querySync();
            l<< fileInfo.get()->displayName();
        }
        auto text = l.join(",");
        fileName = QString(text);
    }

    this->fileNameReady(fileName);
}
