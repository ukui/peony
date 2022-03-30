/*
 * Peony-Qt
 *
 * Copyright (C) 2021, KylinSoft Co., Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

#ifndef FULLLISTVIEW_H
#define FULLLISTVIEW_H
#include "src/Interface/ukuimenuinterface.h"
#include "fullitemdelegate.h"
#include "src/Style/style.h"
#include "thumbnail.h"

#include <QListView>
#include <QEvent>
#include <QScrollBar>
#include <QToolTip>
#include <QStandardItemModel>
#include <QAbstractListModel>
#include <QLabel>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QVariantAnimation>

class TabletMenu;

class FullListView : public QListView
{
    friend class TabletMenu;
    Q_OBJECT
public:
    FullListView(QWidget *parent,int module);
    ~FullListView();

    void updateData(QStringList data);
    bool appDisable(QString desktopfp);//判断是否是禁用的应用
    bool isDraging(){return m_isDraging;}
    QStandardItemModel* listmodel=nullptr;

protected:
   void initWidget();
   void mouseReleaseEvent(QMouseEvent *e);
   void mousePressEvent(QMouseEvent *event);
   void mouseMoveEvent(QMouseEvent *event);

   void dropEvent(QDropEvent *event);
   void dragEnterEvent(QDragEnterEvent *event) override;
   void dragMoveEvent(QDragMoveEvent *event) override;

   void mergeApplication(QPoint pressedpos,QPoint releasepos);
   void showContextMenu(const QPoint &pos);

private:
    /**
     * @brief 将dragApp的索引修改为释放位置下的app的索引.
     * 将被拖动app和释放位置的app之间的apps的索引向被拖动app的索引方向整体移动。完成后更新app list
     * @param dragAppName 被拖动的app的名称 xxx.desktop
     */
    void updateAppOrder(const QString& dragAppName);
    /**
     * @brief 计算x是否在a与b之间
     * @param a 开始
     * @param b 结束
     * @param x 需要判断的值
     * @return
     */
    static bool rangeCheck(float a, float b, float x);

private:
    FullItemDelegate* m_delegate=nullptr;
    QStringList data;
    UkuiMenuInterface* pUkuiMenuInterface=nullptr;
    int module=0;

    int appLine;//null
    int appColumn;//null
    int pageNum;//null

    QPoint  pressedpos; //鼠标按下的位置
    QPoint  pressedGlobalPos; //鼠标按下的位置
    QPoint  releasepos;  //鼠标释放的位置
    QPoint  releaseGlobalPos;  //鼠标释放的位置
    QPoint  moveing_pressedpos;// 鼠标移动的位置
    QPoint  m_lastPressPoint;// 鼠标移动的位置
    QPoint  right_pressedpos;// 右键点击的位置
    bool m_MoveDisabled = false;

    QPoint startPos;//开始点击的位置
    QVariant pressApp;//点击位置的app想、
    QPoint dropPos;//dropPos的位置

    //拖动
    int theDragRow = -1;
    bool iconClick=false;//是否点钟图标
    bool right_iconClick=false;//是否右键点中图标

    ulong press_time = -1;
    ulong move_time = -1;
    ulong release_time = -1;

    QSettings *setting=nullptr;//应用列表settings
    QSettings *disableSetting=nullptr;//禁用的settings
    QSettings *syssetting= nullptr;//不可卸载列表

    //鼠标滚轮灵密度限制
    bool flat=true;

    bool m_isDraging = false;

private Q_SLOTS:
    void onClicked(QModelIndex index);//点击item

Q_SIGNALS:
    void sendItemClickedSignal(QString arg);//发送item点击信号
    void sendGroupClickSignal(QString desktopfn);//发送组合框点击信号
    void sendHideMainWindowSignal();//界面隐藏信号
    void sendUpdateAppListSignal();//界面更新信号
    void pagenumchanged(qint32 signal, bool hide = false);//页面数改变信号
    void pageCollapse(); //左滑页面收纳
    void pageSpread(); //右滑页面展开

    void moveRequest(qint32 length);

};

#endif // FULLLISTVIEW_H
