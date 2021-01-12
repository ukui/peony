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

#include <QFrame>
#include <QGridLayout>
#include <QThread>
#include <memory>

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
class BasicPropertiesPage : public QWidget
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

protected:
    void addSeparator();
    void initFloorOne(const QStringList &uris,FileType fileType);
    void initFloorTwo(const QStringList &uris,FileType fileType);

    FileType checkFileType(const QStringList &uris);

protected Q_SLOTS:
    void onSingleFileChanged(const QString &oldUri, const QString &newUri);
    void countFilesAsync(const QStringList &uris);
    void onFileCountOne(const QString &uri, quint64 size);
    void cancelCount();

    void changeFileIcon();

    void updateInfo(const QString &uri);

private:
    QVBoxLayout                 *m_layout = nullptr;
    std::shared_ptr<FileInfo>    m_info;
    std::shared_ptr<FileWatcher> m_watcher;
    std::shared_ptr<FileWatcher> m_thumbnail_watcher;

    void updateCountInfo();

    //floor1
    QPushButton *m_icon;        //文件图标
    //**new version
    QLineEdit   *m_displayName; //文件名称
    QLineEdit   *m_location;    //文件路径
    QPushButton *m_moveButton;  //移动位置按钮

    //floor2  --  public
    QLabel *m_fileType;         //文件类型
    QLabel *m_fileSize;         //文件大小
    QLabel *m_fileTotalSize;    //文件占用空间
    quint64 m_fileSizeCount        = 0;
    quint64 m_fileTotalSizeCount   = 0;

    //folder type
    QLabel *m_folderContain;    //文件夹下文件统计Label

    quint64 m_folderContainFiles   = 0; //文件夹下文件数量
    quint64 m_folderContainFolders = 0; //文件夹下文件夹数量

    //file , zip
    QHBoxLayout *m_openWith;    //文件打开方式

    //application
    QLabel *m_descrption;       //应用程序描述
    /*

                auto recommendActions = FileLaunchManager::getRecommendActions(m_selections.first());
                if (recommendActions.count() > 1)
                {
                    auto openWithAction = addAction(tr("Open &with..."));
                    QMenu *openWithMenu = new QMenu(this);

                    for (auto action : recommendActions) {
                        action->setParent(openWithMenu);
                        openWithMenu->addAction(static_cast<QAction*>(action));
                    }
                    auto fallbackActions = FileLaunchManager::getFallbackActions(m_selections.first());
                    for (auto action : fallbackActions) {
                        action->setParent(openWithMenu);
                        openWithMenu->addAction(static_cast<QAction*>(action));
                    }
                    openWithMenu->addSeparator();
                    openWithMenu->addAction(tr("&More applications..."), [=]() {
                        FileLauchDialog d(m_selections.first());
                        d.exec();
                    });
                    openWithAction->setMenu(openWithMenu);
                }

     */


    //floor3
    QFormLayout *m_form3;
    QLabel *m_time_created_label;
    QLabel *m_time_modified_label;
    QLabel *m_time_access_label;

    quint64 m_time_created = 0;
    quint64 m_time_modified = 0;
    quint64 m_time_access = 0;

    //floor4

    //
    FileCountOperation *m_count_op = nullptr;
};

}

#endif // BASICPROPERTIESPAGE_H
