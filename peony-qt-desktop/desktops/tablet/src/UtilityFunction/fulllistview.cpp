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

#include "fulllistview.h"
//#include "src/GroupListView/grouplistview.h"
#include "src/UtilityFunction/thumbnail.h"
#include "src/Interface/ukuimenuinterface.h"
#include <QDebug>
#include <syslog.h>
#include <src/Style/style.h>
#include <QDrag>
#include <QSettings>
#include <qabstractitemview.h>
#include <qlistview.h>
#include <QGraphicsDropShadowEffect>
#define TABLED_SCHEMA "org.ukui.SettingsDaemon.plugins.tablet-mode"
#define TABLET_MODE                  "tablet-mode"
/*初始化*/
FullListView::FullListView(QWidget *parent, int module):
    QListView(parent)
{
    QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setBlurRadius(15);
    shadowEffect->setColor(QColor(63, 63, 63, 180));
    shadowEffect->setOffset(0,0);
    this->setGraphicsEffect(shadowEffect);

    this->module=module;
    initWidget();
    pUkuiMenuInterface=new UkuiMenuInterface;
    //应用列表
    QString path=QDir::homePath()+"/.config/ukui/ukui-menu.ini";
    setting=new QSettings(path,QSettings::IniFormat);
    //禁用列表
    QString pathini=QDir::homePath()+"/.cache/ukui-menu/ukui-menu.ini";
    disableSetting=new QSettings(pathini,QSettings::IniFormat);

    //不可卸载列表
    QString syspath = QDir::homePath()+"/.config/ukui/menusysapplist.ini";
    syssetting = new QSettings(syspath,QSettings::IniFormat);


    tabletMode=new QGSettings(TABLED_SCHEMA);
    setDragEnabled(true);
    //翻页灵敏度时间调节
    time=new QTimer(this);
    connect(time,&QTimer::timeout,[=](){
        if(flat==false)
        {
            flat=true;
            time->stop();
        }
    });
}

FullListView::~FullListView()
{
    if(pUkuiMenuInterface)
        delete pUkuiMenuInterface;
    if(setting)
        delete setting;
    if(disableSetting)
        delete disableSetting;
    if(tabletMode)
        delete tabletMode;
    if(m_animation)
        delete m_animation;
    if(time)
        delete time;
    if(listmodel)
        delete listmodel;
    if(m_delegate) //can
        delete m_delegate;

    pUkuiMenuInterface=nullptr;
    setting=nullptr;
    disableSetting=nullptr;//        else
    //        {
    //            QPixmap mPixmap;
    //            if(bigIcon)
    //            {
    //                mPixmap = icon.pixmap((Style::AppListIconSize+12,Style::AppListIconSize+12));//wgx
    //                mPixmap = mPixmap.scaled(108,108);
    //                qDebug() << "pixmap = icon.pixmap((Style::AppListIconSize+12,Style::AppListIconSize+12),QIcon::Disabled,QIcon::Off);"<<mPixmap.size();
    //            }else {
    //                mPixmap = icon.pixmap((Style::AppListIconSize,Style::AppListIconSize));//wgx
    //            }
    //        icon = QIcon(mPixmap);
    //        }
    tabletMode=nullptr;
    m_animation=nullptr;
    time=nullptr;
    listmodel=nullptr;
    m_delegate=nullptr;

    if(syssetting)
        delete syssetting;
    syssetting=nullptr;
}

void FullListView::initWidget()
{
    this->setSelectionMode(QAbstractItemView::SingleSelection);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setAttribute(Qt::WA_TranslucentBackground);
  //  this->setWindowFlags(Qt::FramelessWindowHint);//无边框
    viewport()->setAttribute(Qt::WA_TranslucentBackground);
    this->viewport()->setUpdatesEnabled(true);
    this->setViewMode(QListView::IconMode);
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    this->setResizeMode(QListView::Adjust);
    this->setTextElideMode(Qt::ElideRight);
    this->setMouseTracking(true);
    this->setFocusPolicy(Qt::NoFocus);
    this->setMovement(QListView::Snap);
    this->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->setGridSize(QSize(Style::AppListItemSizeWidth,Style::AppListItemSizeHeight));
    m_animation = new QVariantAnimation(verticalScrollBar());
    m_animation->setEasingCurve(QEasingCurve::Linear);
    connect(m_animation, &QVariantAnimation::valueChanged, this, [=](const QVariant variant) {
        int value = variant.toInt();
        this->verticalScrollBar()->setValue(value);
//        qDebug()<<__FUNCTION__<<verticalScrollBar()->value()<<value<<"动画"<<verticalScrollBar()->minimum()<<verticalScrollBar()->maximum();
        if (value < this->verticalScrollBar()->minimum() || value > this->verticalScrollBar()->maximum())
            m_animation->stop();
    });

}
//添加数据
void FullListView::addData(QStringList data)
{
    if (listmodel) {
        listmodel->clear();
    } else {
        listmodel = new QStandardItemModel(this);
    }

    this->setModel(listmodel);
    Q_FOREACH(QString desktopfp,data)
    {
        QStandardItem* item=new QStandardItem;//        else
        //        {
        //            QPixmap mPixmap;
        //            if(bigIcon)
        //            {
        //                mPixmap = icon.pixmap((Style::AppListIconSize+12,Style::AppListIconSize+12));//wgx
        //                mPixmap = mPixmap.scaled(108,108);
        //                qDebug() << "pixmap = icon.pixmap((Style::AppListIconSize+12,Style::AppListIconSize+12),QIcon::Disabled,QIcon::Off);"<<mPixmap.size();
        //            }else {
        //                mPixmap = icon.pixmap((Style::AppListIconSize,Style::AppListIconSize));//wgx
        //            }
        //        icon = QIcon(mPixmap);
        //        }
        item->setData(QVariant::fromValue<QString>(desktopfp),Qt::DisplayRole);
        item->setData(QVariant::fromValue<bool>(0),Qt::UserRole);
        bool appDis=appDisable(desktopfp);
        item->setData(QVariant::fromValue<bool>(appDis),Qt::UserRole+1);
        listmodel->appendRow(item);
    }
    //空白位置补位
    qDebug() << "+++++++++++++(appPage,appColumn,appLine,appNum) = (" << Style::appPage
             << "," << Style::appColumn << "," << Style::appLine << "," << Style::appNum << ")";
    for(int i=0;i<Style::appPage* Style::appColumn* Style::appLine-Style::appNum;i++)//
    {
        QStandardItem* items=new QStandardItem;
        items->setEnabled(false);
        items->setBackground(Qt::NoBrush);
        listmodel->appendRow(items);
    }
    m_delegate= new FullItemDelegate(this,module);
    this->setItemDelegate(m_delegate);
}


void FullListView::updateData(QStringList data)
{
    listmodel->clear();
    Q_FOREACH(QString desktopfp,data)
    {
        QStandardItem* item=new QStandardItem;
        item->setData(QVariant::fromValue<QString>(desktopfp),Qt::DisplayRole);
        item->setData(QVariant::fromValue<bool>(0),Qt::UserRole);
        bool appDis=appDisable(desktopfp);
        item->setData(QVariant::fromValue<bool>(appDis),Qt::UserRole+1);
        listmodel->appendRow(item);
    }
    for(int i=0;i<Style::appPage* Style::appColumn* Style::appLine-Style::appNum;i++)//
    {
        QStandardItem* items=new QStandardItem;
        items->setEnabled(false);
        items->setBackground(Qt::NoBrush);
        listmodel->appendRow(items);
    }
    m_delegate= new FullItemDelegate(this,module);
    this->setItemDelegate(m_delegate);

}

bool FullListView::appDisable(QString desktopfp)//判断是否是禁用应用(这个还有问题暂时不会用)
{
    QString str;
    //打开文件.desktop
    GError** error=nullptr;
    GKeyFileFlags flags=G_KEY_FILE_NONE;
    GKeyFile* keyfile=g_key_file_new ();
    QByteArray fpbyte=desktopfp.toLocal8Bit();
    char* filepath=fpbyte.data();
    g_key_file_load_from_file(keyfile,filepath,flags,error);
    char* name=g_key_file_get_locale_string(keyfile,"Desktop Entry","Exec", nullptr, nullptr);
    //取出value值
    QString execnamestr=QString::fromLocal8Bit(name);
    //处理value值
    str=execnamestr;
//    str = execnamestr.section(' ', 0, 0);
//    QStringList list = str.split('/');
//    str = list[list.size()-1];
   //关闭文件
    g_key_file_free(keyfile);

    QString desktopfp1=/*"application/"+*/str;
    //QString group=desktopfp.split('/').last();
    disableSetting->beginGroup("application");
    //判断
    bool bo=disableSetting->contains(desktopfp1.toLocal8Bit().data());// iskey
    bool bo1=disableSetting->QSettings::value(desktopfp1.toLocal8Bit().data()).toBool();//isvalue
    disableSetting->endGroup();
    //qDebug()<<bo<<bo1;
    if (bo && bo1==false)//都存在//存在并且为false
    {
        return 1;
    }
    return 0;
}
void FullListView::insertData(QString desktopfp)
{
//    QStandardItem* item=new QStandardItem;
//    item->setData(QVariant::fromValue<QString>(desktopfp),Qt::DisplayRole);
//    item->setData(QVariant::fromValue<bool>(0),Qt::UserRole);
//    listmodel->appendRow(item);

}
/*点击执行*/
void FullListView::onClicked(QModelIndex index)
{

    QVariant var = listmodel->data(index, Qt::DisplayRole);
    if(var.isValid())
    {
        QString desktopfp=var.value<QString>();
        QFileInfo fileInfo(desktopfp);
        QString desktopfn=fileInfo.fileName();
        setting->beginGroup("groupname");
        bool iscontain=setting->contains(desktopfn);
        setting->endGroup();

        if(desktopfn!="kylin-screenshot.desktop"&&!appDisable(desktopfp))
        {
            Q_EMIT sendHideMainWindowSignal();
        }
        if(iscontain)
        {
//            Q_EMIT sendGroupClickSignal(desktopfn);
        }
        else
        {
            Q_EMIT sendItemClickedSignal(desktopfp);
        }
    }
}
bool FullListView::uninstall(QString desktopfp)//判断是否可以卸载
{

    qDebug()<<"fulllistview"<<desktopfp;
    syssetting->beginGroup("ukui-menu-sysapplist");
    QStringList sysapplist=syssetting->allKeys();
    syssetting->sync();
    syssetting->endGroup();

    QString appstr = desktopfp.section(' ', 0, 0);
    QStringList strlist = appstr.split('/');
    appstr = strlist[strlist.size()-1];
    if(sysapplist.contains(appstr))
    {
        qDebug()<<"默认应用";
        return 0;
    }



//    qDebug()<<"uninstall"<<desktopfp;
//    QString path = QDir::homePath()+"/.config/ukui/desktop_applist";
//    //判断路径是否为空
//    if(path.isEmpty() == false)
//    {
//        //指定文件路径
//        QFile file(path);
//        //检查文件打开情况
//        bool isOk = file.open(QIODevice::ReadOnly |QIODevice::Text);
//        if(isOk == true)
//        {
//            QString appstr = desktopfp.section(' ', 0, 0);
//            QStringList strlist = appstr.split('/');
//            appstr = strlist[strlist.size()-1];
//            //appstr.chop(8);
//            qDebug()<<"11111111appstr"<<appstr;
//            //读文件 内容放到字节组
//            QByteArray array = file.readAll();
//            QString str = QString(array);
//            qDebug()<<"11111111str"<<str;
//            //显示到界面文本框
//            QStringList list = str.split("\n");
//            if(list.contains(appstr))
//            {
//                qDebug()<<"1111111true";
//                file.close();
//                return 1;
//            }
//        }
//        //文件关闭
//        file.close();
//    }
    return 1;
}

void FullListView::mousePressEvent(QMouseEvent *event)
{
    if (m_animation->state() == QVariantAnimation::Running)
        return;
    pressedpos = event->pos();
    if(event->button() == Qt::LeftButton) {//左键
        if((this->indexAt(event->pos()).isValid())&&(pressedpos.x() % Style::AppListItemSizeWidth >= Style::AppLeftSpace &
           pressedpos.x() % Style::AppListItemSizeWidth <= Style::AppLeftSpace+ Style::AppListIconSize &
           pressedpos.y() % Style::AppListItemSizeHeight >= Style::AppTopSpace &
           pressedpos.y() % Style::AppListItemSizeHeight <= Style::AppTopSpace+ Style::AppListIconSize))

        {
            pressApp = listmodel->data(this->indexAt(pressedpos), Qt::DisplayRole);
            iconClick = true;
            startPos=event->pos();
            qDebug() << "void FullListView::mousePressEvent(QMouseEvent *event) pressedPos" << pressedpos;
            listmodel->setData(this->indexAt(pressedpos),1,Qt::UserRole+2);
            repaint();
            //this->model()->setData(this->indexAt(pressedpos),QVariant::fromValue<bool>(1),Qt::UserRole);
        }else{
            iconClick=false;
            qDebug() << "void FullListView::mousePressEvent(QMouseEvent *event) pressedPos 1111" << pressedpos;
            listmodel->setData(this->indexAt(pressedpos),QVariant::fromValue<bool>(0),Qt::UserRole+2);
            //this->model()->setData(this->indexAt(pressedpos),QVariant::fromValue<bool>(0),Qt::UserRole);
            pressedpos = moveing_pressedpos = event->pos();
            press_time = event->timestamp();

        }
    } else if(event->button() == Qt::RightButton) {//右键

        if((this->indexAt(event->pos()).isValid())&&(pressedpos.x() % Style::AppListItemSizeWidth >= Style::AppLeftSpace &
           pressedpos.x() % Style::AppListItemSizeWidth <= Style::AppLeftSpace+ Style::AppListIconSize &
           pressedpos.y() % Style::AppListItemSizeHeight >= Style::AppTopSpace &
           pressedpos.y() % Style::AppListItemSizeHeight <= Style::AppTopSpace+ Style::AppListIconSize))

        {
            pressApp = listmodel->data(this->indexAt(pressedpos), Qt::DisplayRole);
            right_iconClick=true;
            right_pressedpos=pressedpos;
            //this->model()->setData(this->indexAt(pressedpos),QVariant::fromValue<bool>(1),Qt::UserRole);
            this->selectionModel()->setCurrentIndex(this->indexAt(event->pos()),QItemSelectionModel::SelectCurrent);
            listmodel->setData(this->indexAt(pressedpos),QVariant::fromValue<bool>(1),Qt::UserRole + 2);
        }else{
            right_iconClick=false;
            //this->model()->setData(this->indexAt(pressedpos),QVariant::fromValue<bool>(0),Qt::UserRole);
            listmodel->setData(this->indexAt(pressedpos),QVariant::fromValue<bool>(0),Qt::UserRole + 2);
        }

    }
    event->accept();
}
void FullListView::mouseMoveEvent(QMouseEvent *event)
{
    if (m_animation->state() == QVariantAnimation::Running)
        return;
    if(event->buttons()&Qt::LeftButton & this->indexAt(event->pos()).isValid())
    {
        if(iconClick)
        {
            if((event->pos()-startPos).manhattanLength()>=QApplication::startDragDistance())
            {
                //this->setCursor(Qt::ClosedHandCursor);
                QModelIndex theDragIndex = indexAt(startPos);
                theDragRow = theDragIndex.row();
                //[1]把拖拽数据放在QMimeData容器中
                QString desktopfp = this->indexAt(event->pos()).data(Qt::DisplayRole).value<QString>();
                QByteArray itemData = desktopfp.toLocal8Bit();;

                QMimeData *mimeData  = new QMimeData;
                ThumbNail *dragImage = new ThumbNail(this);
                QDrag *drag          = new QDrag(this);

                mimeData->setData("INFO", itemData);
                //[2]设置拖拽时的缩略图
                dragImage->setupthumbnail(desktopfp);

                QPixmap pixmap = dragImage->grab();

                drag->setMimeData(mimeData);
                drag->setPixmap(pixmap);
                drag->setHotSpot(QPoint(pixmap.width() / 2, pixmap.height() / 2));

                m_isDraging = true;
                if(drag->exec(Qt::MoveAction) == Qt::MoveAction){
                }

                m_isDraging = false;
                delete drag; //setMimeData 使得mimeData归drag所有，不需要单独进行delele处理
                delete dragImage;
                drag      = nullptr;
                dragImage = nullptr;
            }
        }
    }
    if (event->buttons() & Qt::LeftButton && iconClick!=true) {
        int distance = (event->pos() - moveing_pressedpos).y();
        int x_distance = (event->pos() - moveing_pressedpos).x();
        //qDebug()<<"x:y"<<distance<<x_distance;
        if(qAbs(x_distance)>=qAbs(distance))
        {
            return;
        }
        //qDebug()<<"y:max"<<verticalScrollBar()->value() - distance<<verticalScrollBar()->maximum()<<verticalScrollBar()->value();

        int dis = Style::appLine*(Style::AppListItemSizeHeight);
//        qDebug()<<dis;

        if(distance>0)
        {
            qDebug()<<"11111";//上翻页
            if((verticalScrollBar()->value() - distance) <= verticalScrollBar()->minimum())
            {
                verticalScrollBar()->setValue(verticalScrollBar()->minimum());
            }
            else
            {
                verticalScrollBar()->setValue(verticalScrollBar()->value() - distance);
            }

        }else{
            qDebug()<<"22222";//下翻页
            qDebug()<<"verticalScrollBar()->value()"<<verticalScrollBar()->value()<<verticalScrollBar()->maximum()-dis;
            if((verticalScrollBar()->value() - distance) >= verticalScrollBar()->maximum())
            {
                verticalScrollBar()->setValue(verticalScrollBar()->maximum());
            }
            else
            {
                verticalScrollBar()->setValue(verticalScrollBar()->value() - distance);
            }
        }
//        if(verticalScrollBar()->value()>=verticalScrollBar()->maximum()-dis)
//            return ;
//        verticalScrollBar()->setValue(verticalScrollBar()->value() - distance);
        moveing_pressedpos = event->pos();
    }
     event->accept();
}
void FullListView::dragMoveEvent(QDragMoveEvent *event)
{
    //qDebug() << "-----" << "dragMoveEvent" << "-----";
    moveing_pressedpos=event->pos();
    //拖拽特效绘制

//添加拖动时的自动翻页
    if(flat == true)
    {
//        qDebug()<<"flat"<<flat;
        flat=false;
        time->start(500);
        if(event->pos().y() >= (Style::AppListItemSizeHeight*Style::appLine - 50) || event->pos().y() <= Style::AppListItemSizeHeight)
        {
            if(Style::nowpagenum != 1 && event->pos().y() <= 50)
            {
                 Style::nowpagenum = Style::nowpagenum-1;
                 Q_EMIT pagenumchanged();
                 this->verticalScrollBar()->setValue((Style::nowpagenum-1)*Style::appLine*(Style::AppListItemSizeHeight));//dis
            }
            if(Style::nowpagenum != Style::appPage && event->pos().y() >= (Style::AppListItemSizeHeight*Style::appLine - 50))
            {
                 Style::nowpagenum =Style::nowpagenum+1;
                 Q_EMIT pagenumchanged();
                 this->verticalScrollBar()->setValue((Style::nowpagenum-1)*Style::appLine*(Style::AppListItemSizeHeight));//dis
            }
        }
    }


//    if(moveing_pressedpos.x() % Style::AppListItemSizeWidth >= Style::AppLeftSpace &
//            moveing_pressedpos.x() % Style::AppListItemSizeWidth <= Style::AppLeftSpace+ Style::AppListIconSize &
//            moveing_pressedpos.y() % Style::AppListItemSizeHeight >= Style::AppTopSpace &
//            moveing_pressedpos.y() % Style::AppListItemSizeHeight <= Style::AppTopSpace+ Style::AppListIconSize)
//    {
//        this->model()->setData(this->indexAt(moveing_pressedpos),QVariant::fromValue<bool>(0),Qt::UserRole);
//    }
//    else
//    {
//        this->model()->setData(this->indexAt(moveing_pressedpos),QVariant::fromValue<bool>(0),Qt::UserRole);
//    }
}
void FullListView::dragEnterEvent(QDragEnterEvent *event)
{
    FullListView *source = qobject_cast<FullListView *>(event->source());
    if (source && source == this && iconClick) {
        event->setDropAction(Qt::MoveAction);
        event->accept();
    }
}
void FullListView::dropEvent(QDropEvent *event)
{
    //qDebug() << "-----" << "dropEvent" << "-----";
    FullListView *source = qobject_cast<FullListView *>(event->source());
    if (source && source == this)
    {
        dropPos=event->pos();
        if(dropPos.x() % Style::AppListItemSizeWidth >= Style::AppLeftSpace &
                dropPos.x() % Style::AppListItemSizeWidth <= Style::AppLeftSpace+ Style::AppListIconSize &
                dropPos.y() % Style::AppListItemSizeHeight >= Style::AppTopSpace &
                dropPos.y() % Style::AppListItemSizeHeight <= Style::AppTopSpace+ Style::AppListIconSize)

        {
//            mergeApplication(startPos,dropPos);
        }
        else
        {
            insertApplication(startPos,dropPos);
        }
    }
    this->model()->setData(this->indexAt(pressedpos),QVariant::fromValue<bool>(0),Qt::UserRole + 2);
}
void FullListView::mouseReleaseEvent(QMouseEvent *e)
{
    if (m_animation->state() == QVariantAnimation::Running)
        return;
    releasepos=e->pos();//释放的位置坐标
    if(iconClick)
    {
        if(e->button()==Qt::LeftButton)
        {
//            qDebug()<<"left";
            if(qAbs(releasepos.x()-pressedpos.x())<=5&&qAbs(releasepos.y()-pressedpos.y())<=5&&this->indexAt(releasepos)==this->indexAt(pressedpos))
            {
                //this->model()->setData(this->indexAt(pressedpos),QVariant::fromValue<bool>(0),Qt::UserRole);
                QEventLoop loop1;
                QTimer::singleShot(200, &loop1, SLOT(quit()));
                loop1.exec();
                listmodel->setData(this->indexAt(pressedpos),0,Qt::UserRole+2);
                QEventLoop loop;
                QTimer::singleShot(200, &loop, SLOT(quit()));
                loop.exec();

                Q_EMIT onClicked(this->indexAt(e->pos()));
                //if(desktopfn=="kylin-screenshot.desktop")
                //        {
                //            return;
                //        }
//                Q_EMIT sendHideMainWindowSignal();
                //this->model()->setData(this->indexAt(pressedpos),QVariant::fromValue<bool>(0),Qt::UserRole);
            }
        }
    }
    else
    {
        if(e->button()==Qt::LeftButton)
        {
//            qDebug() << "元素高" << Style::AppListItemSizeHeight << "每页应用行数" << Style::appLine << "页数" << Style::appPage;
//            verticalScrollBar()->setMaximum(Style::AppListItemSizeHeight * Style::appLine * (Style::appPage - 1));

            qDebug() << "滚动条最大值" << verticalScrollBar()->maximum();
//            qDebug()<<"else";
            dist =releasepos.y()-pressedpos.y();
            int dist_y = releasepos.y() - pressedpos.y();
            int dist_x = releasepos.x() - pressedpos.x();
            int dis = Style::appLine * (Style::AppListItemSizeHeight);
            //int dis = this->height() - Style::AppTopSpace;
            int ll = this->verticalScrollBar()->maximum();
            if(((pressedpos-releasepos).manhattanLength() <= QApplication::startDragDistance() || pressedpos==releasepos) && (!tabletMode->get(TABLET_MODE).toBool())/*qAbs(releasepos.x()-pressedpos.x())<=5&&qAbs(releasepos.y()-pressedpos.y())*/)
            {
                 qDebug()<<"sendHideMainWindowSignal";
                 Q_EMIT sendHideMainWindowSignal();
                //this->hide();
            }else{
                int pag = verticalScrollBar()->value() / dis;
                qDebug() << "翻页前滚动条值" << verticalScrollBar()->value() << "页面高度" << dis;
                if (2 * qAbs(dist_y) >= qAbs(dist_x)) {//翻页处理
                     ulong disttime = e->timestamp() - press_time;
                     qDebug()<<dist_x<<dist_y<<dis<<pag<<verticalScrollBar()->value()<<disttime<<__FUNCTION__<<this->rect()<<Style::AppListItemSizeHeight<<Style::appLine;
                    if (qAbs(dist_y) >= 80) {//翻页 (disttime < 400 && qAbs(dist_y) >= 80) ||
                        if ((dist_y >=80)/*&&(Style::nowpagenum!=1)*/) {//下翻
                         //   m_animation->stop();
                            if((pag * dis) !=  verticalScrollBar()->value())
                            {
                                m_animation->setStartValue(verticalScrollBar()->value());
                                m_animation->setEndValue(pag * dis);
                                m_animation->setDuration(350);
                                m_animation->start();
                            }
                            Style::nowpagenum = pag + 1;
                            Q_EMIT pagenumchanged();

                        } else if((dist_y <=-80)/*&&Style::nowpagenum!=Style::appPage*/) {//上翻
                        //    m_animation->stop();
                            if(((pag + 1) * dis) != verticalScrollBar()->value())
                            {
                                m_animation->setStartValue(verticalScrollBar()->value());
                                m_animation->setEndValue((pag + 1) * dis);
    //                            if(verticalScrollBar()->value()==verticalScrollBar()->maximum())
    //                                m_animation->setEndValue(pag*dis);
                                m_animation->setDuration(350);
                                m_animation->start();
                            }
                            Style::nowpagenum = pag + 2;
                            if(Style::nowpagenum > Style::appPage)
                            {
                                Style::nowpagenum = Style::appPage;
                            }
                            Q_EMIT pagenumchanged();

                        }
                    }
                    else
                    {//归位
                        m_animation->setStartValue(verticalScrollBar()->value());
                        m_animation->setEndValue((Style::nowpagenum - 1) * dis);
                        m_animation->setDuration(150);
                        m_animation->start();
                    }

                } else {//专注模式处理
                    if(dist_x<=-50 && Style::ScreenRotation==false) //向左翻
                    {
                        Q_EMIT pageCollapse();

                    } else if(dist_x>=50 && Style::ScreenRotation==false) //向右翻
                    {
                        Q_EMIT pageSpread();
                    }
                    else
                    {
                        m_animation->setStartValue(verticalScrollBar()->value());
                        m_animation->setEndValue((Style::nowpagenum - 1) * dis);
                        m_animation->setDuration(150);
                        m_animation->start();
                    }
                }
            }
            e->accept();
        }
        if(right_iconClick)
        {
//            qDebug()<<""<<right_iconClick;
            this->selectionModel()->setCurrentIndex(this->indexAt(right_pressedpos),QItemSelectionModel::SelectCurrent);
            Q_EMIT customContextMenuRequested(right_pressedpos);
            //return ;
        }
    }
    //this->model()->setData(this->indexAt(pressedpos),QVariant::fromValue<bool>(0),Qt::UserRole);
    listmodel->setData(this->indexAt(pressedpos),QVariant::fromValue<bool>(0),Qt::UserRole+2);
    iconClick=false;//是否点钟图标
    right_iconClick=false;//是否右键点中图标
    theDragRow = -1;
    this->setCursor(Qt::ArrowCursor);
}
void FullListView::wheelEvent(QWheelEvent *e)
{
    if (m_animation->state() == QVariantAnimation::Running)
        return;

//    qDebug()<<event->angleDelta()<<event->delta()<<event->pixelDelta()<<event->angleDelta();
    int dis = Style::appLine*(Style::AppListItemSizeHeight);
//    int dis1 = this->height();
    int pag; verticalScrollBar()->value() / dis;
//    qDebug()<<Style::appLine<<Style::AppListItemSizeHeight<<dis<<pag<<verticalScrollBar()->value();
    if (qAbs(e->angleDelta().y()) > qAbs(e->angleDelta().x())) {
        if ((e->angleDelta().y() >= 120) ) { //上翻
            verticalScrollBar()->setValue(verticalScrollBar()->value()-80);
            pag = verticalScrollBar()->value() / dis;
            if(verticalScrollBar()->value() != pag * dis)
            {
                m_animation->setStartValue(verticalScrollBar()->value());
                m_animation->setEndValue(pag * dis);
//                if (verticalScrollBar()->value() == verticalScrollBar()->maximum())
//                    m_animation->setEndValue(pag  * dis);
                m_animation->setDuration(350);
                m_animation->start();
            }
            Style::nowpagenum = pag + 1;
            Q_EMIT pagenumchanged();
        } else if ((e->angleDelta().y() <= -120) ) { //下翻
            verticalScrollBar()->setValue(verticalScrollBar()->value()+80);
            pag = verticalScrollBar()->value() / dis;
            if(verticalScrollBar()->value() != (pag + 1) * dis)
            {
                m_animation->setStartValue(verticalScrollBar()->value());
                m_animation->setEndValue((pag + 1) * dis);
    //            if(verticalScrollBar()->value()==verticalScrollBar()->maximum())
    //                m_animation->setEndValue(pag*dis);
                m_animation->setDuration(350);
                m_animation->start();
            }
            Style::nowpagenum = pag + 2;
            Q_EMIT pagenumchanged();
        }
        e->accept();
    }

}



//拖拽移动的时候，如果不是应用的话，就交换位置
void FullListView::insertApplication(QPoint pressedpos,QPoint releasepos)
{
//    QVariant var1 = listmodel->data(this->indexAt(releasepos), Qt::DisplayRole);
    QVariant var2 =pressApp;//= listmodel->data(this->indexAt(pressedpos), Qt::DisplayRole);

    QString desktopfp2=var2.value<QString>();//drag_desktopfp;//点击位置的应用
    QFileInfo fileInfo2(desktopfp2);
    QString desktopfn2=fileInfo2.fileName();
//    qDebug()<<"2pre"<<desktopfn2;
    releasepos.setX(releasepos.x()+70);
    QVariant var3 = listmodel->data(this->indexAt(releasepos), Qt::DisplayRole);//释放位置右侧有应用
    QString desktopfp3=var3.value<QString>();//释放位置的应用
    QFileInfo fileInfo3(desktopfp3);
    QString desktopfn3=fileInfo3.fileName();
    releasepos.setX(releasepos.x()-140);
    QVariant var4 = listmodel->data(this->indexAt(releasepos), Qt::DisplayRole);//右侧没有左侧有
    QString desktopfp4=var4.value<QString>();//释放位置的应用
    QFileInfo fileInfo4(desktopfp4);
    QString desktopfn4=fileInfo4.fileName();
    //qDebug()<<"4rel"<<desktopfn4;

    if(var3.isValid()&&desktopfp3!=desktopfp2)
    {

        setting->beginGroup("application");
        int indexPre=setting->value(desktopfn2).toInt();
        int indexRel=setting->value(desktopfn3).toInt();
        QStringList keyList=setting->allKeys();
        if(indexPre>indexRel)
        {
            qDebug()<<">";
            for(int i=0;i<keyList.count();i++)
            {
                if(setting->value(keyList.at(i)).toInt()>=indexRel&&setting->value(keyList.at(i)).toInt()<indexPre)
                {
                    setting->setValue(keyList.at(i),setting->value(keyList.at(i)).toInt()+1);
                }
            }
            setting->setValue(desktopfn2,indexRel);

//                listmodel->removeRow(this->indexAt(pressedpos).row());
//                listmodel->insertRow(this->indexAt(releasepos).row());
//                listmodel->setData(this->indexAt(releasepos),var2);


        }
        else if(indexPre<indexRel){

            qDebug()<<"<";
            for(int i=0;i<keyList.count();i++)
            {
                if(setting->value(keyList.at(i)).toInt()>indexPre&&setting->value(keyList.at(i)).toInt()<indexRel)
                {
                    setting->setValue(keyList.at(i),setting->value(keyList.at(i)).toInt()-1);
                }
            }
            setting->setValue(desktopfn2,indexRel-1);
//                listmodel->insertRow(this->indexAt(releasepos).row());
//                listmodel->setData(this->indexAt(releasepos),var2);
//                listmodel->removeRow(this->indexAt(pressedpos).row());
        }

        setting->sync();
        setting->endGroup();

    }
    else if(var4.isValid()&&desktopfp4!=desktopfp2)//最后一个
    {
        setting->beginGroup("application");
        int indexPre=setting->value(desktopfn2).toInt();
        int indexRel=setting->value(desktopfn4).toInt();
        QStringList keyList=setting->allKeys();

        if(indexPre<indexRel){
//         qDebug()<<"4 <";
            for(int i=0;i<keyList.count();i++)
            {
                if(setting->value(keyList.at(i)).toInt()>indexPre&&setting->value(keyList.at(i)).toInt()<=indexRel)
                {
                    setting->setValue(keyList.at(i),setting->value(keyList.at(i)).toInt()-1);
                }
            }
            setting->setValue(desktopfn2,indexRel);
//                listmodel->insertRow(this->indexAt(relpos).row());
//                listmodel->setData(this->indexAt(relpos),var2);
//                listmodel->removeRow(this->indexAt(prepos).row());
        }
        setting->sync();
        setting->endGroup();

    }

//    }

     Q_EMIT sendUpdateAppListSignal();
//     qDebug() << "sendUpdateAppListSignal---------------------";
}


//拖拽移动的时候，如果是应用，就组成组合框
void FullListView::mergeApplication(QPoint pressedpos,QPoint releasepos)
{
    QVariant var1 = listmodel->data(this->indexAt(releasepos), Qt::DisplayRole);
    QVariant var2 = listmodel->data(this->indexAt(pressedpos), Qt::DisplayRole);

    if(var1!=var2)
    {

        QString desktopfp1=var1.value<QString>();  //鼠标释放位置的应用
        QFileInfo fileInfo1(desktopfp1);
        QString desktopfn1=fileInfo1.fileName();

        QString desktopfp2=var2.value<QString>();   //鼠标点击位置的应用
        QFileInfo fileInfo2(desktopfp2);
        QString desktopfn2=fileInfo2.fileName();


        setting->beginGroup("groupname");
        bool isgroup=setting->contains(desktopfn2);
        setting->endGroup();

        if(!isgroup)
        {
//            qDebug() <<"拖拽";
            //合并
            setting->beginGroup("groupname");
            QStringList groupList=setting->allKeys();
            bool iscontain=setting->contains(desktopfn1);
            setting->endGroup();

            if(iscontain) //组存在,把应用加入组中，同时去掉position里的键值。
            {
                qDebug() << "组存在，找到这个组，把应用加入里边";
                setting->beginGroup(desktopfn1);
                setting->setValue(desktopfn2,setting->allKeys().count());
                setting->sync();
                setting->endGroup();

                setting->beginGroup("application");
                int indexPre=setting->value(desktopfn2).toInt();


                QStringList appList=setting->allKeys();
                for(int i=0;i<appList.count();i++)
                {
                    if(setting->value(appList.at(i)).toInt()>=indexPre)
                    {
                        setting->setValue(appList.at(i),setting->value(appList.at(i)).toInt()-1);

                    }
                }

                setting->remove(desktopfn2);
                setting->sync();
                setting->endGroup();

                listmodel->removeRow(this->indexAt(pressedpos).row());

                this->viewport()->update();

            }
            else //如果组不存在的话
            {
                setting->beginGroup("groupname");
                QString group=QString("group%1").arg(groupList.count()+1);
                QString groupname=QString("group%1").arg(groupList.count()+1);
                setting->setValue(group,groupname);
                setting->sync();
                setting->endGroup();


                setting->beginGroup(group);
                setting->setValue(desktopfn2,0);
                setting->setValue(desktopfn1,1);
                setting->sync();
                setting->endGroup();

                setting->beginGroup("application");
                int indexPre=setting->value(desktopfn2).toInt();
                int indexRel=setting->value(desktopfn1).toInt();
                setting->setValue(groupname,indexRel);
                QStringList appList=setting->allKeys();
                for(int i=0;i<appList.count();i++)
                {
                    if(setting->value(appList.at(i)).toInt()>=indexPre)
                    {
                        setting->setValue(appList.at(i),setting->value(appList.at(i)).toInt()-1);
                    }
                }

                setting->remove(desktopfn1);
                setting->remove(desktopfn2);
                setting->sync();
                setting->endGroup();

                listmodel->removeRow(this->indexAt(pressedpos).row());
                listmodel->removeRow(this->indexAt(releasepos).row());
                listmodel->insertRow(this->indexAt(releasepos).row());
                QVariant variant= QVariant::fromValue<QString>(QString("usr/share/application/%1").arg(group));
                listmodel->setData(this->indexAt(releasepos),variant);
            }

        }
 }

}



