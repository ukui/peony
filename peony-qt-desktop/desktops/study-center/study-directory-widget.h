/*
 * Copyright (C) 2019 Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/&gt;.
 *
 */

#ifndef STUDYDIRECTORYWIDGET_H
#define STUDYDIRECTORYWIDGET_H

#include <QWidget>
#include <QSettings>
#include <QHBoxLayout>
#include <QTableWidget>
#include <QScrollBar>
#include <QLabel>
#include <QPushButton>
#include <QDir>
#include <QProcess>
//#include <QSvgRenderer>
#include <QHeaderView>
#include <QListView>
#include <QStringList>
#include <QScrollArea>

//#include "../tablet/src/Interface/ukuimenuinterface.h"
//#include "../tablet/src/RightClickMenu/rightclickmenu.h"
//#include "../tablet/src/Style/style.h"
namespace Peony {

class TabletAppEntity;


class StudyDirectoryWidget : public QWidget
{
    Q_OBJECT
public:
    explicit StudyDirectoryWidget(QStringList &strListTitleStyle, QMap<QString, QList<TabletAppEntity*>> &subtitleMap, int mode = 0, QWidget *parent = nullptr);
    ~StudyDirectoryWidget();

private:
//    UkuiMenuInterface* pUkuiMenuInterface=nullptr;

//    QSettings *m_setting=nullptr;
    //主界面
    QVBoxLayout*  m_mainLayout=nullptr;
    QVBoxLayout*  m_scrollAreaWidLayout=nullptr;
    QScrollArea*  m_scrollArea=nullptr;
    QWidget*      m_scrollareawid=nullptr;
    QMap<QString, QList<TabletAppEntity*>> m_studyCenterDataMap;
    int m_iMode;
//    QListView* m_listview=nullptr;
//    QStringList m_strListData;

//    QFileSystemWatcher* fileWatcher=nullptr;//监控文件夹状态
//    RightClickMenu* menu=nullptr;
//public:
    //QMap<QString, QList<TabletAppEntity*>> getStudyCenterData();

protected:
    void initWidget(QStringList &strListTitleStyle);
    //void getUseAppList(QString &strModuleName);
    //初始化应用列表界面
    void initAppListWidget();

private Q_SLOTS:
//    /**
//     * @brief 更新单个item槽函数
//     * @param desktopfp为desktop文件路径
//     * @param type为0时表示固定，为1时表示取消固定
//     */
//    void updateListViewSlot(QString desktopfp,int type);
    void execApplication(QModelIndex appname);//执行应用程序
    QString getExecPath(QString &strAppName);//获取应用程序执行路径
//    void removeListItemSlot(QString desktopfp);//删除单个item
//    //void removeListAllItemSlot();//删除所有非固定item
//    //void updateListViewAllSlot();//更新应用列表槽函数

//Q_SIGNALS:
//    void sendUpdateAppListSignal(QString desktopfp,int type);//向CommonUseWidget发送更新应用列表信号
//    void sendHideMainWindowSignal();//向MainViewWidget发送隐藏主窗口信号
//    void removeListItemSignal(QString desktopfp);
//    //void removeListAllItemSignal();
};
}

#endif // STUDYDIRECTORYWIDGET_H
