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

#ifndef BASICPROPERTIESPAGE_H
#define BASICPROPERTIESPAGE_H

#include <QWidget>
#include "peony-core_global.h"

#include <QCheckBox>
#include <QFrame>
#include <QGridLayout>
#include <QThread>
#include <memory>

#include "properties-window-tab-page-plugin-save-iface.h"

class QVBoxLayout;
class QFormLayout;
class QPushButton;
class QLabel;
class QLineEdit;

namespace Peony {

class FileInfo;
class FileWatcher;
class FileCountOperation;

class FileNameThread : public QThread {
    Q_OBJECT
private:
    const QStringList &m_uris;
public:
    FileNameThread(const QStringList &uris) : m_uris(uris){}

Q_SIGNALS:
    void fileNameReady(QString fileName);

protected:
    void run();
};

/*!
 * \brief The BasicPropertiesPage class
 * \todo
 * handle special files, such as divice or remote server.
 */
class BasicPropertiesPage : public PropertiesWindowTabPagePluginSaveIface
{
    Q_OBJECT
public:

    enum FileType{
        BP_Folder = 1,
        BP_File,
        BP_Application,
        BP_MultipleFIle /*选中多个文件*/
    };

    explicit BasicPropertiesPage(const QStringList &uris, QWidget *parent = nullptr);
    ~BasicPropertiesPage();


    // PropertiesWindowTabPagePluginSaveIface interface
public:
    void saveAllChange();

protected:
    void addSeparator();
    QLabel *createFixedLable(QWidget *parent = nullptr);
    void addOpenWithMenu(QWidget *parent = nullptr);
    void initFloorOne(const QStringList &uris,BasicPropertiesPage::FileType fileType);
    void initFloorTwo(const QStringList &uris,BasicPropertiesPage::FileType fileType);
    void initFloorThree(BasicPropertiesPage::FileType fileType);
    void initFloorFour();
    FileType checkFileType(const QStringList &uris);
    void changeFileIcon();
    void moveFile();

protected Q_SLOTS:
    void getFIleInfo(const QStringList &uris);
    void onSingleFileChanged(const QString &oldUri, const QString &newUri);
    void countFilesAsync(const QStringList &uris);
    void onFileCountOne(const QString &uri, quint64 size);
    void cancelCount();

    void updateInfo(const QString &uri);

private:
    QVBoxLayout                 *m_layout = nullptr;
    std::shared_ptr<FileInfo>    m_info   = nullptr;
    std::shared_ptr<FileWatcher> m_watcher;
    std::shared_ptr<FileWatcher> m_thumbnail_watcher;

    void updateCountInfo(bool isDone = false);

    qint64 m_fileDoneCount     = 0;

    //floor1
    QPushButton *m_iconButton       = nullptr;    //文件图标
    //**new version
    QLineEdit   *m_displayNameEdit  = nullptr;    //文件名称
    QLineEdit   *m_locationEdit     = nullptr;    //文件路径
    QPushButton *m_moveButtonButton = nullptr;    //移动位置按钮

    //floor2  --  public
    QLabel *m_fileTypeLabel         = nullptr;    //文件类型
    QLabel *m_fileSizeLabel         = nullptr;    //文件大小
    QLabel *m_fileTotalSizeLabel    = nullptr;    //文件占用空间

    quint64 m_fileSizeCount        = 0;
    quint64 m_fileTotalSizeCount   = 0;

    //folder type
    QLabel *m_folderContainLabel = nullptr;       //文件夹下文件统计Label

    quint64 m_folderContainFiles   = 0;      //文件夹下文件数量
    quint64 m_folderContainFolders = 0;      //文件夹下文件夹数量

    //file , zip
    QHBoxLayout *m_openWithLayout = nullptr;    //文件打开方式

    //application
    QLabel *m_descrptionLabel = nullptr;       //应用程序描述

    //floor3
    QLabel *m_timeCreatedLabel  = nullptr;
    QLabel *m_timeModifiedLabel = nullptr;
    QLabel *m_timeAccessLabel   = nullptr;

    quint64 m_timeCreated  = 0;
    quint64 m_timeModified = 0;
    quint64 m_timeAccess   = 0;

    //floor4
    QCheckBox *m_readOnly = nullptr;
    QCheckBox *m_hidden   = nullptr;
    //
    FileCountOperation *m_countOp = nullptr;

};

}

#endif // BASICPROPERTIESPAGE_H
