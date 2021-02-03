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
 * Authors: Wenfei He <hewenfei@kylinos.cn>
 *
 */

#ifndef OPENWITHPROPERTIESPAGE_H
#define OPENWITHPROPERTIESPAGE_H

#include "properties-window-tab-iface.h"
#include "file-label-model.h"
#include "file-info-job.h"
#include "file-launch-action.h"

#include <QVBoxLayout>
#include <QListWidget>
#include <QHash>
#include <QDebug>
#include <QDialog>
#include <QDialogButtonBox>
#include <QtConcurrent>

namespace Peony {

class LaunchHashList {
public:
    static LaunchHashList *getAllLaunchHashList(const QString &uri, QWidget *parent = nullptr);
    LaunchHashList();
    LaunchHashList(const QString &uri, QWidget *parent = nullptr);
    ~LaunchHashList();
public:
    //保存该文件的全部打开方式 - Save all open methods of the file
    QListWidget *m_actionList = nullptr;
    //每个listItem对应的launchAction - Launch Action corresponding to each list Item
    QHash<QListWidgetItem*,FileLaunchAction*> *m_actionHash = nullptr;
};

class NewFileLaunchDialog : public QDialog
{
    Q_OBJECT
public:
    explicit NewFileLaunchDialog(const QString &uri, QWidget *parent = nullptr);

    virtual ~NewFileLaunchDialog();

    QSize sizeHint() const override {
        return QSize(400, 600);
    }
private:
    QVBoxLayout      *m_layout     = nullptr;
    QDialogButtonBox *m_button_box = nullptr;
    LaunchHashList *m_launchHashList = nullptr;
};

class AllFileLaunchDialog : public QDialog
{
Q_OBJECT
public:
    explicit AllFileLaunchDialog(const QString &uri, QWidget *parent = nullptr);

    virtual ~AllFileLaunchDialog();

    QSize sizeHint() const override {
        return QSize(400, 600);
    }
private:
    QVBoxLayout      *m_layout     = nullptr;
    QDialogButtonBox *m_button_box = nullptr;
    LaunchHashList *m_launchHashList = nullptr;
};

//open with page
class OpenWithPropertiesPage : public PropertiesWindowTabIface
{
    Q_OBJECT
public:
    /*!
     * \brief 创建默认打开方式的listWidget
     * \param uri
     * \param parent
     * \return
     */
    static QListWidget *createDefaultLaunchListWidget(const QString &uri, QWidget *parent = nullptr);
    /*!
     * \brief 创建该文件的全部打开方式listWidget
     * \param uri
     * \param parent
     * \return
     */
    static QListWidget *createAllLaunchListWidget(const QString &uri, QWidget *parent = nullptr);

public:
    void initFloorOne();

    void initFloorTwo();

    void initFloorThree();

    void addSeparator()
    {
        QFrame *separator = new QFrame(this);
        separator->setFrameShape(QFrame::HLine);
        m_layout->addWidget(separator);
    }

public:
    explicit OpenWithPropertiesPage(const QString &uri, QWidget *parent = nullptr);
    ~OpenWithPropertiesPage();

    void init();
    void saveAllChange() override;

private:
    QVBoxLayout *m_layout = nullptr;

    std::shared_ptr<FileInfo> m_fileInfo  = nullptr;
    QFutureWatcher<void> *m_futureWatcher = nullptr;
    //新的打开方式
    FileLaunchAction *m_newAction      = nullptr;
    LaunchHashList   *m_launchHashList = nullptr;
};


}


#endif //OPENWITHPROPERTIESPAGE_H
