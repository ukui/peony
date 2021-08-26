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

class FullListView : public QListView
{
    Q_OBJECT
public:
    FullListView(QWidget *parent,int module);
    ~FullListView();

    void addData(QStringList data);//字母排序模块添加数据
    void updateData(QStringList data);
    void insertData(QString desktopfp);
    bool appDisable(QString desktopfp);//判断是否是禁用的应用
    bool isDraging(){return m_isDraging;}
    QStandardItemModel* listmodel=nullptr;
//    void paintPixmap(const QModelIndex &index,QPoint position);

protected:
   void initWidget();
   void mouseReleaseEvent(QMouseEvent *e);
   void mousePressEvent(QMouseEvent *event);
   void wheelEvent(QWheelEvent *e);
   void mouseMoveEvent(QMouseEvent *event);
   void paintGroupItem(const QModelIndex &index,QString group);

   void dropEvent(QDropEvent *event);
   void dragEnterEvent(QDragEnterEvent *event) override;
//   void dragLeaveEvent(QDragLeaveEvent *event) override;
   void dragMoveEvent(QDragMoveEvent *event) override;

   void insertApplication(QPoint pressedpos,QPoint releasepos);
   void mergeApplication(QPoint pressedpos,QPoint releasepos);


private:
    QVariantAnimation *m_animation=nullptr; //翻页动画
    FullItemDelegate* m_delegate=nullptr;
    QStringList data;
    UkuiMenuInterface* pUkuiMenuInterface=nullptr;
    int module=0;


    int appLine;//null
    int appColumn;//null
    int pageNum;//null


    /*鼠标事件的参数变量*/
    int dist;//翻页的鼠标移动长度

    QPoint  pressedpos; //鼠标按下的位置
    QPoint  releasepos;  //鼠标释放的位置
    QPoint  moveing_pressedpos;// 鼠标移动的位置
    QPoint  right_pressedpos;// 右键点击的位置


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
    QTimer *time;
    bool flat=true;

    QGSettings *tabletMode=nullptr;

    bool m_isDraging = false;

private Q_SLOTS:
    void onClicked(QModelIndex index);//点击item
    bool uninstall(QString desktopfp);

Q_SIGNALS:
    void sendItemClickedSignal(QString arg);//发送item点击信号
    void sendGroupClickSignal(QString desktopfn);//发送组合框点击信号
    void sendHideMainWindowSignal();//界面隐藏信号
    void sendUpdateAppListSignal();//界面更新信号
    void pagenumchanged();//页面数改变信号
    void pageCollapse(); //左滑页面收纳
    void pageSpread(); //右滑页面展开

};

#endif // FULLLISTVIEW_H
