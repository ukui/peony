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
#include <fcntl.h>
#include <sys/types.h>
#include <glib/gstdio.h>
#include <gio/gdesktopappinfo.h>

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

#include <QMessageBox>
#include <QProcess>
#include <QStandardPaths>
#include <QUrl>

#include <QFileDialog>

#include "file-lauch-dialog.h"
#include "file-meta-info.h"
#include "generic-thumbnailer.h"

using namespace Peony;

BasicPropertiesPage::BasicPropertiesPage(const QStringList &uris, QWidget *parent) : PropertiesWindowTabIface(parent)
{
    //check file type and search fileinfo
    FileType l_fileType = this->checkFileType(uris);
    qDebug() << "BasicPropertiesPage:" <<uris.count() <<uris.first();
    //单个文件才能换icon
    if (l_fileType != BP_MultipleFIle) {
        m_watcher = std::make_shared<FileWatcher>(uris.first());
        m_watcher->connect(m_watcher.get(), &FileWatcher::locationChanged, this, &BasicPropertiesPage::onSingleFileChanged);
        m_watcher->startMonitor();

        m_thumbnail_watcher = std::make_shared<FileWatcher>("thumbnail:///");
        connect(m_thumbnail_watcher.get(), &FileWatcher::fileChanged, this, [=](const QString &uri){
            auto icon = ThumbnailManager::getInstance()->tryGetThumbnail(uri);
            m_iconButton->setIcon(icon);
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

    if(l_fileType != BP_MultipleFIle ) {

        this->initFloorThree(l_fileType);
        this->addSeparator();
        this->initFloorFour();
    }

    QLabel *l4 = new QLabel(this);
    m_layout->addWidget(l4, 1);
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

QLabel *BasicPropertiesPage::createFixedLabel(QWidget *parent)
{
    QLabel *l = new QLabel(parent);
    l->setFixedHeight(1);
    return l;
}

void BasicPropertiesPage::addOpenWithMenu(QWidget *parent)
{
    auto recommendActions = FileLaunchManager::getRecommendActions(m_info.get()->uri());
    if (m_openWithLayout && recommendActions.count() >= 1)
    {
        m_openWithLayout->setContentsMargins(0,0,56,0);
        //修改为打开方式及应用名称 加上图标
        QLabel *appName = new QLabel(parent);
        appName->setText(recommendActions.first()->getAppInfoDisplayName());
        m_openWithLayout->addWidget(appName);
        m_openWithLayout->addStretch(1);

        QPushButton *moreAppButton = new QPushButton(parent);
        moreAppButton->setText(tr("Change"));
        moreAppButton->setMinimumSize(70,32);
        m_openWithLayout->addWidget(moreAppButton);

        connect(moreAppButton,&QPushButton::clicked,this,[=](){
            FileLauchDialog d(m_info.get()->uri());
            d.exec();
        });
    }
}

/*!
 *
 * \brief BasicPropertiesPage::initFloorOne
 */
void BasicPropertiesPage::initFloorOne(const QStringList &uris,BasicPropertiesPage::FileType fileType)
{
    QFrame      *floor1  = new QFrame(this);
    QGridLayout *layout1 = new QGridLayout(floor1);
    layout1->setContentsMargins(22,16,0,16);

    floor1->setMinimumHeight(100);
    floor1->setLayout(layout1);

    m_iconButton      = new QPushButton(floor1);
    m_displayNameEdit = new QLineEdit(floor1);
    m_locationEdit    = new QLineEdit(floor1);

    m_iconButton->setFixedSize(QSize(60,60));
    m_iconButton->setIconSize(QSize(48,48));
    m_iconButton->setProperty("isIcon", true);

    auto form1 = new QFormLayout(floor1);
    layout1->addLayout(form1,0,0);
    form1->setContentsMargins(0,6,22,0);

    form1->addRow(m_iconButton);

    //icon area
    if (fileType != BP_MultipleFIle) {
        connect(m_iconButton, &QPushButton::clicked, this, &BasicPropertiesPage::changeFileIcon);
        this->onSingleFileChanged(nullptr, uris.first());
    } else {
        m_iconButton->setIcon(QIcon::fromTheme("text-x-generic"));
    }

    //text area
    auto form2 = new QFormLayout(floor1);
    layout1->addLayout(form2, 0, 1);

    m_displayNameEdit->setMinimumHeight(32);
    m_locationEdit->setMinimumHeight(32);

    form2->addRow(tr("Name"), m_displayNameEdit);
    form2->addRow(tr("Location"), m_locationEdit);

    //select multiplefiles
    if (fileType == BP_MultipleFIle)
        m_displayNameEdit->setReadOnly(true);

    //new thread get fileName
    FileNameThread *l_thread = new FileNameThread(uris);
    l_thread->start();

    connect(l_thread,&FileNameThread::fileNameReady,this,[=](QString fileName){
        m_displayNameEdit->setText(fileName);
        delete l_thread;
    });

    connect(m_displayNameEdit, &QLineEdit::textChanged, [=]() {
        if (!m_displayNameEdit->isReadOnly() && !m_displayNameEdit->text().isEmpty()) {
            this->thisPageChanged();
//            FileOperationUtils::rename(m_info->uri(), m_displayNameEdit->text(), true);
        }
    });

    m_locationEdit->setReadOnly(true);
    QUrl url = FileUtils::getParentUri(uris.first());
    m_locationEdit->setText(url.toDisplayString());

    //move -button
    if(fileType == BP_Folder) {
        m_moveButtonButton = new QPushButton(floor1);

        auto form3 = new QFormLayout(floor1);
        form3->setContentsMargins(0,2,16,0);

        m_moveButtonButton->setText(tr("move"));
        m_moveButtonButton->setFixedSize(70,32);

        QPushButton *hiddenButton = new QPushButton(floor1);
        hiddenButton->setFixedSize(70,32);
        //设置透明隐藏按钮
        QGraphicsOpacityEffect *op = new QGraphicsOpacityEffect(floor1);
        op->setOpacity(0);
        hiddenButton->setGraphicsEffect(op);

        form3->addRow(hiddenButton);
        form3->addRow(m_moveButtonButton);
        //home目录不支持移动和重命名
        //其他根目录文件夹虽然没有屏蔽，但是没有权限依旧无法改名
        if(m_info.get()->uri() == ("file://"+QStandardPaths::standardLocations(QStandardPaths::HomeLocation).first())) {
            m_displayNameEdit->setReadOnly(true);
            m_moveButtonButton->setDisabled(true);
        }

        connect(m_moveButtonButton,&QPushButton::clicked,this,&BasicPropertiesPage::moveFile);
        layout1->addLayout(form3,0,2);
    } else {
        auto form4 = new QFormLayout(floor1);
        form4->setContentsMargins(0,0,56,0);

        layout1->addLayout(form4,0,2);
    }
    //add floor1 to context
    m_layout->addWidget(floor1);
}

void BasicPropertiesPage::initFloorTwo(const QStringList &uris,BasicPropertiesPage::FileType fileType)
{
    QFrame      *floor2  = new QFrame(this);
    QFormLayout *layout2 = new QFormLayout(floor2);
    layout2->setContentsMargins(22,24,0,24);
    floor2->setLayout(layout2);

    if( fileType == BP_MultipleFIle )
        floor2->setMaximumHeight(180);
    else
        floor2->setMinimumHeight(160);

    m_fileTypeLabel      = new QLabel(floor2);
    m_fileSizeLabel      = new QLabel(floor2);
    m_fileTotalSizeLabel = new QLabel(floor2);

    layout2->addRow(tr("Type:"),m_fileTypeLabel);
    //FIX:重写文件类型获取函数
    layout2->addWidget(this->createFixedLabel(floor2));

    if(fileType != BP_MultipleFIle)
        m_fileTypeLabel->setText(m_info.get()->type());

    //根据文件类型添加组件
    switch (fileType) {
    case BP_Folder:
        m_folderContainLabel = new QLabel(floor2);
        layout2->addRow(this->createFixedLabel(76,0,tr("Include:"),floor2),m_folderContainLabel);
        layout2->addWidget(this->createFixedLabel(floor2));
        break;
    case BP_File:
        m_openWithLayout = new QHBoxLayout(floor2);
        layout2->addRow(this->createFixedLabel(76,0,tr("Open with:"),floor2),m_openWithLayout);
        layout2->addWidget(this->createFixedLabel(floor2));
        this->addOpenWithMenu(floor2);
        break;
    case BP_Application:
        m_descrptionLabel = new QLabel(floor2);
        layout2->addRow(this->createFixedLabel(76,0,tr("Description:"),floor2),m_descrptionLabel);
        layout2->addWidget(this->createFixedLabel(floor2));
        m_descrptionLabel->setText(m_info.get()->displayName());
        break;
    case BP_MultipleFIle:
        m_fileTypeLabel->setText("选中多个文件");
    default:
        break;
    }

    layout2->addRow(tr("Size:"),m_fileSizeLabel);
    layout2->addWidget(this->createFixedLabel(floor2));
    layout2->addRow(tr("Total size:"),m_fileTotalSizeLabel);

    this->countFilesAsync(uris);

    m_layout->addWidget(floor2);
}

void BasicPropertiesPage::initFloorThree(BasicPropertiesPage::FileType fileType)
{
    auto floor3 = new QFrame(this);
    QFormLayout *layout3 = new QFormLayout(floor3);
    floor3->setLayout(layout3);

    if(fileType == BP_Folder)
        floor3->setMaximumHeight(140);
    else
        floor3->setMinimumHeight(120);

    layout3->setContentsMargins(22,24,0,24);

    m_timeCreatedLabel  = new QLabel(floor3);

    layout3->addRow(this->createFixedLabel(76,0,tr("Time Created:"),floor3), m_timeCreatedLabel);

    switch (fileType) {
    case BP_File:
    case BP_Application:
        m_timeModifiedLabel = new QLabel(floor3);
        m_timeAccessLabel   = new QLabel(floor3);
        layout3->addWidget(createFixedLabel(floor3));
        layout3->addRow(this->createFixedLabel(76,0,tr("Time Modified:"),floor3), m_timeModifiedLabel);
        layout3->addWidget(createFixedLabel(floor3));
        layout3->addRow(this->createFixedLabel(76,0,tr("Time Access:"),floor3), m_timeAccessLabel);
        break;
    case BP_MultipleFIle:
    case BP_Folder:
    default:
        break;
    }

    m_layout->addWidget(floor3);

    updateInfo(m_info.get()->uri());
    connect(m_watcher.get(), &FileWatcher::locationChanged, [=](const QString&, const QString &uri) {
        this->updateInfo(uri);
    });

}

/*!
 * 底部隐藏多选框和只读选择框
 * \brief BasicPropertiesPage::initFloorFour
 */
void BasicPropertiesPage::initFloorFour()
{
    QFrame      *floor4  = new QFrame(this);
    QFormLayout *layout4 = new QFormLayout(floor4);
    layout4->setContentsMargins(22,24,0,24);
    m_readOnly = new QCheckBox(tr("Readonly"), floor4);
    m_hidden   = new QCheckBox(tr("Hidden"), floor4);

    QHBoxLayout *proLayout = new QHBoxLayout(floor4);
    proLayout->addWidget(m_readOnly);
    proLayout->addWidget(m_hidden);
    proLayout->addWidget(this->createFixedLabel(floor4));

    if(m_info.get()->canRead() && !m_info.get()->canWrite())
        m_readOnly->setCheckState(Qt::Checked);

    if(m_info.get()->displayName().startsWith("."))
        m_hidden->setCheckState(Qt::Checked);

    layout4->addRow(this->createFixedLabel(76,0,tr("FileProperty:"),floor4),proLayout);

    //确认被修改
    connect(m_readOnly,&QCheckBox::stateChanged,this,&BasicPropertiesPage::thisPageChanged);
    connect(m_hidden,&QCheckBox::stateChanged,this,&BasicPropertiesPage::thisPageChanged);

    m_layout->addWidget(floor4);
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

//异步获取数据
void BasicPropertiesPage::getFIleInfo(const QStringList &uris)
{
    //    if(oldUri != nullptr && (m_info.get()->uri() != newUri)) {
    //        m_info = FileInfo::fromUri(newUri);
    //        FileInfoJob *j = new FileInfoJob(m_info);
    //        j->setAutoDelete();
    //        j->querySync();
    //    }
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
    m_info = FileInfo::fromUri(newUri);
    FileInfoJob *j = new FileInfoJob(m_info);
    j->setAutoDelete();
    j->querySync();

    ThumbnailManager::getInstance()->createThumbnail(m_info.get()->uri(), m_thumbnail_watcher);
    auto icon = QIcon::fromTheme(m_info.get()->iconName(), QIcon::fromTheme("text-x-generic"));
    auto thumbnail = ThumbnailManager::getInstance()->tryGetThumbnail(m_info.get()->uri());

    m_iconButton->setIcon(thumbnail.isNull()? icon: thumbnail);
    m_displayNameEdit->setText(m_info.get()->displayName());

    if (thumbnail.isNull())
    {
        ThumbnailManager::getInstance()->createThumbnail(m_info.get()->uri(), m_thumbnail_watcher);
    }

    m_iconButton->setIcon(thumbnail.isNull()? icon : thumbnail);
    QUrl url = FileUtils::getParentUri(m_info.get()->uri());

    m_locationEdit->setText(url.toDisplayString());
}

void BasicPropertiesPage::countFilesAsync(const QStringList &uris)
{
    //old op will delete later
    if (m_countOp) {
        m_countOp->disconnect();
        m_countOp->cancel();
    }
    //clear old data
    m_folderContainFiles   = 0;
    m_folderContainFolders = 0;
    m_fileSizeCount        = 0;
    m_fileTotalSizeCount   = 0;

    m_countOp = new FileCountOperation(uris);
    m_countOp->setAutoDelete(true);

    connect(m_countOp, &FileOperation::operationPreparedOne, this, &BasicPropertiesPage::onFileCountOne, Qt::BlockingQueuedConnection);

    connect(m_countOp, &FileCountOperation::countDone, [=](quint64 file_count, quint64 hidden_file_count, quint64 total_size) {
        m_countOp = nullptr;
        m_folderContainFiles = file_count - m_folderContainFolders;
        m_fileSizeCount = total_size;
        this->updateCountInfo(true);
    });

    QThreadPool::globalInstance()->start(m_countOp);
}

void BasicPropertiesPage::onFileCountOne(const QString &uri, quint64 size)
{
    //...FIX:第一版本在当前位置对文件夹进行统计,慢
    std::shared_ptr<FileInfo> l_fileInfo = FileInfo::fromUri(uri);
    FileInfoJob *j = new FileInfoJob(l_fileInfo);
    j->setAutoDelete();
    j->querySync();
    bool a = l_fileInfo.get()->isDir();

    if(a)
        m_folderContainFolders ++;
    else
        m_folderContainFiles ++;

    m_fileSizeCount += size;
    m_fileDoneCount ++;
    //500 files update ui
    updateCountInfo((m_fileDoneCount % 500 == 0));
}

void BasicPropertiesPage::cancelCount()
{
    if (m_countOp)
        m_countOp->cancel();
}

void BasicPropertiesPage::moveFile(){
    auto newDirPath = QString(QFileDialog::getExistingDirectoryUrl(nullptr, tr("Choose a new folder:"),m_info.get()->uri()).toEncoded());
    qDebug() << "new path:" << newDirPath ;

    if(newDirPath.isEmpty())
        return;

    if(newDirPath == m_info.get()->uri()) {
        QMessageBox::critical(nullptr, tr("Error"), tr("cannot move a folder to itself !"), QMessageBox::Yes, QMessageBox::Yes);
        return;
    }

    if(newDirPath.startsWith(m_info.get()->uri())) {
        QMessageBox::critical(nullptr, tr("Error"), tr("cannot move a folder to itself !"), QMessageBox::Yes, QMessageBox::Yes);
        return;
    }

    qDebug() << "oldPath:" << m_info.get()->uri() << newDirPath;

    QStringList uriList;
    uriList << m_info.get()->uri();

    if(!FileOperationUtils::move(uriList,newDirPath,true)->hasError()){
        //move to peony
        //        QUrl url = uri;
        QProcess p;
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
        p.setProgram("peony");
        p.setArguments(QStringList()<<"--show-folders" << newDirPath);
        p.startDetached();
#else
        p.startDetached("peony", QStringList()<<"--show-folders"<< newDirPath);
#endif
        Q_EMIT this->requestCloseMainWindow();
    }
}
/*!
 * 响应窗口确定按钮
 * \brief BasicPropertiesPage::saveAllChange
 */
void BasicPropertiesPage::saveAllChange()
{
    //未发生修改
    if(!this->m_thisPageChanged)
        return;
    //拒绝修改home目录
    if(m_info.get()->uri() == ("file://"+QStandardPaths::standardLocations(QStandardPaths::HomeLocation).first())) {
        return;
    }

    if(m_readOnly) {
        mode_t mod = 0;
        if(m_readOnly->isChecked()) {
            mod |= S_IRUSR;
            mod |= S_IRGRP;
            mod |= S_IROTH;
        } else {
            mod |= S_IRUSR;
            mod |= S_IRGRP;
            mod |= S_IROTH;

            mod |= S_IWUSR;
            mod |= S_IWGRP;
            mod |= S_IWOTH;
        }
        QUrl url = m_info.get()->uri();
        g_chmod(url.path().toUtf8(), mod);

    }

    if(m_hidden) {
        QString l_reName = m_info.get()->displayName();

        bool isHidden = m_info.get()->displayName().startsWith(".");

        //以前没隐藏，并且选中隐藏框
        if(!isHidden && m_hidden->isChecked()) {
            FileOperationUtils::rename(m_info.get()->uri(), ("." + l_reName), true);

        } else if(isHidden && !m_hidden->isChecked()) {
            //以前已经隐藏，并且取消选中隐藏框
            l_reName = m_info.get()->displayName().mid(1,-1);
            FileOperationUtils::rename(m_info.get()->uri(), l_reName, true);
        }
    }

    //save displayName
    if (!m_displayNameEdit->isReadOnly() && !m_displayNameEdit->text().isEmpty()) {
        if(m_info.get()->displayName() != m_displayNameEdit->text())
            FileOperationUtils::rename(m_info.get()->uri(), m_displayNameEdit->text(), true);
    }

}

void BasicPropertiesPage::changeFileIcon()
{
    QUrl iconPathUrl;
    iconPathUrl.setPath("/usr/share/icons");
    auto picture = QFileDialog::getOpenFileName(nullptr, tr("Choose a custom icon"), "/usr/share/icons", "*.png *.jpg *.jpeg *.svg");

    qDebug() << "BasicPropertiesPage::changeFileIcon()" << picture << picture.isEmpty();

    if (!picture.isEmpty()) {
        auto metaInfo = FileMetaInfo::fromUri(m_info.get()->uri());
        QFileInfo fileInfo(picture);
        if (!QIcon::fromTheme(fileInfo.baseName()).isNull())
            metaInfo.get()->setMetaInfoString("custom-icon", fileInfo.baseName());
        else
            metaInfo.get()->setMetaInfoString("custom-icon", picture);

        ThumbnailManager::getInstance()->createThumbnail(m_info.get()->uri(), m_thumbnail_watcher, true);
    }
}

void BasicPropertiesPage::updateCountInfo(bool isDone)
{
    if(isDone) {

        //假的4k对齐 unReal
        qint64 a = m_fileSizeCount % 1024;
        qint64 b = m_fileSizeCount / 1024;
        qint64 cell4k = (a == 0) ? b : (b + 1);
        m_fileTotalSizeCount = cell4k * 1024;

        QString fileSizeText;
        QString fileTotalSizeText;

        qreal fileSizeKMGB = 0.0;

        // 1024 KB
        if(b < 1024) {
            fileSizeKMGB = b;
            fileSizeText = tr("%1 KB (%2 Bytes)").arg(QString::number(fileSizeKMGB,'f',2)).arg(m_fileSizeCount);
            fileSizeKMGB = (qreal)m_fileTotalSizeCount / (qreal)1024;
            fileTotalSizeText = tr("%1 KB (%2 Bytes)").arg(QString::number(fileSizeKMGB,'f',2)).arg(m_fileTotalSizeCount);
        } else {
            //1024 MB
            fileSizeKMGB = (qreal)m_fileSizeCount / (qreal)1048576;
            if(fileSizeKMGB < 1024) {
                fileSizeText = tr("%1 MB (%2 Bytes)").arg(QString::number(fileSizeKMGB,'f',2)).arg(m_fileSizeCount);
                fileSizeKMGB = (qreal)m_fileTotalSizeCount / (qreal)1048576;
                fileTotalSizeText = tr("%1 MB (%2 Bytes)").arg(QString::number(fileSizeKMGB,'f',2)).arg(m_fileTotalSizeCount);
            } else {
                fileSizeKMGB = (qreal)m_fileSizeCount / (qreal)1073741824;
                fileSizeText = tr("%1 GB (%2 Bytes)").arg(QString::number(fileSizeKMGB,'f',2)).arg(m_fileSizeCount);
                fileSizeKMGB = (qreal)m_fileTotalSizeCount / (qreal)1073741824;
                fileTotalSizeText = tr("%1 GB (%2 Bytes)").arg(QString::number(fileSizeKMGB,'f',2)).arg(m_fileTotalSizeCount);
            }
        }

        m_fileSizeLabel->setText(fileSizeText);
        m_fileTotalSizeLabel ->setText(fileTotalSizeText);

        if(m_folderContainLabel)
            m_folderContainLabel->setText(tr("%1 files, %2 folders").arg(m_folderContainFiles).arg(m_folderContainFolders));
    }
}

void BasicPropertiesPage::updateInfo(const QString &uri)
{
    //FIXME: replace BLOCKING api in ui thread.
    m_info = FileInfo::fromUri(uri);
    FileInfoJob *j = new FileInfoJob(m_info);
    j->setAutoDelete();
    j->querySync();

    //    m_info = FileInfo::fromUri(uri);

    GFile *file = g_file_new_for_uri(uri.toUtf8().constData());
    GFileInfo *info = g_file_query_info(file,
                                        "time::*",
                                        G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS,
                                        nullptr,
                                        nullptr);
    g_object_unref(file);

    m_timeCreated = g_file_info_get_attribute_uint64(info,
                                                     G_FILE_ATTRIBUTE_TIME_CREATED);
    //    m_timeCreated = g_file_info_get_attribute_uint64(info, "time::created");
    QDateTime date1 = QDateTime::fromMSecsSinceEpoch(m_timeCreated*1000);
    QString time1 = date1.toString(tr("yyyy-MM-dd, HH:mm:ss"));
    m_timeCreatedLabel->setText(time1);

    //    m_form3->itemAt(0, QFormLayout::LabelRole)->widget()->setVisible(m_timeCreated != 0);
    //    m_form3->itemAt(0, QFormLayout::FieldRole)->widget()->setVisible(m_timeCreated != 0);

    //    //folder don't show access time
    //    if (m_info->isDir())
    //    {
    //        m_form3->itemAt(2, QFormLayout::LabelRole)->widget()->setVisible(false);
    //        m_form3->itemAt(2, QFormLayout::FieldRole)->widget()->setVisible(false);
    //    }
    if(m_timeModifiedLabel) {
        m_timeModified = g_file_info_get_attribute_uint64(info,
                                                          "time::modified");
        QDateTime date2 = QDateTime::fromMSecsSinceEpoch(m_timeModified*1000);
        QString time2 = date2.toString(tr("yyyy-MM-dd, HH:mm:ss"));

        m_timeModifiedLabel->setText(time2);
    }
    if(m_timeAccessLabel) {
        m_timeAccess = g_file_info_get_attribute_uint64(info,
                                                        "time::access");
        QDateTime date3 = QDateTime::fromMSecsSinceEpoch(m_timeAccess*1000);
        QString time3 = date3.toString(tr("yyyy-MM-dd, HH:mm:ss"));
        m_timeAccessLabel->setText(time3);
    }

    g_object_unref(info);
}

QLabel *BasicPropertiesPage::createFixedLabel(quint64 minWidth, quint64 minHeight, QString text, QWidget *parent)
{
    QLabel *l = new QLabel(parent);
    if(minWidth != 0)
        l->setMinimumWidth(minWidth);
    if(minHeight != 0)
        l->setMinimumHeight(minHeight);
    l->setText(text);
    return l;
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

    Q_EMIT fileNameReady(fileName);
}
