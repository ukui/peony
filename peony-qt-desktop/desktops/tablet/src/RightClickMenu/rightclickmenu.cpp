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

#include "rightclickmenu.h"
#include "src/UtilityFunction/utility.h"
#include <QDebug>

RightClickMenu::RightClickMenu(QWidget *parent):
    QWidget(parent)
{
    QString path=QDir::homePath()+"/.config/ukui/ukui-menu.ini";
    m_setting=new QSettings(path,QSettings::IniFormat);

    m_cmdProc=new QProcess;
    connect(m_cmdProc , &QProcess::readyReadStandardOutput, this , &RightClickMenu::onReadOutput);

    sprintf(m_style, "QMenu{padding-left:2px;padding-top:6px;padding-right:2px;padding-bottom:6px;border:1px solid %s;border-radius:6px;background-color:%s;}\
            QMenu::item:selected{background-color:%s;border-radius:6px;}\
            QMenu::separator{height:1px;background-color:%s;margin-top:2px;margin-bottom:2px;}",
            RightClickMenuBorder ,RightClickMenuBackground,RightClickMenuSelected,RightClickMenuSeparator);
}

RightClickMenu::~RightClickMenu()
{
    if(m_cmdProc)
        delete m_cmdProc;
    if(m_setting)
        delete m_setting;
    m_cmdProc=nullptr;
    m_setting=nullptr;

}

QPixmap RightClickMenu::getIconPixmap(QString iconstr, int type)
{
    const auto ratio=devicePixelRatioF();
    QPixmap pixmap;

    if(type==0)
    {
        pixmap=loadSvg(iconstr,16*ratio);
        pixmap.setDevicePixelRatio(qApp->devicePixelRatio());
    }
    else
    {
        QIcon icon=QIcon::fromTheme(iconstr);
        QPixmap pixmap_1=icon.pixmap(QSize(16*ratio,16*ratio));
        pixmap=drawSymbolicColoredPixmap(pixmap_1);
    }
    return pixmap;
}

QPixmap RightClickMenu::drawSymbolicColoredPixmap(const QPixmap &source)
{
    QColor gray(128,128,128);
    QColor standard (31,32,34);
    QImage img = source.toImage();
    for (int x = 0; x < img.width(); x++) {
        for (int y = 0; y < img.height(); y++) {
            auto color = img.pixelColor(x, y);
            if (color.alpha() > 0) {
                if (qAbs(color.red()-gray.red())<20 && qAbs(color.green()-gray.green())<20 && qAbs(color.blue()-gray.blue())<20) {
                    color.setRed(255);
                    color.setGreen(255);
                    color.setBlue(255);
                    img.setPixelColor(x, y, color);
                }
                else if(qAbs(color.red()-standard.red())<20 && qAbs(color.green()-standard.green())<20 && qAbs(color.blue()-standard.blue())<20)
                {
                    color.setRed(255);
                    color.setGreen(255);
                    color.setBlue(255);
                    img.setPixelColor(x, y, color);
                }
                else
                {
                    img.setPixelColor(x, y, color);
                }
            }
        }
    }
    return QPixmap::fromImage(img);
}

void RightClickMenu::fixToAllActionTriggerSlot()
{
    m_actionNumber=1;
    QFileInfo fileInfo(m_desktopfp);
    QString desktopfn=fileInfo.fileName();
    m_setting->beginGroup("lockapplication");
    m_setting->setValue(desktopfn,m_setting->allKeys().size());
    m_setting->sync();
    m_setting->endGroup();
//    m_setting->beginGroup("application");//wgx
//    m_setting->remove(desktopfn);
//    m_setting->sync();
//    m_setting->endGroup();
//    m_setting->beginGroup("datetime");
//    m_setting->remove(desktopfn);
//    m_setting->sync();
//    m_setting->endGroup();
    m_setting->beginGroup("recentapp");
    m_setting->remove(desktopfn);
    m_setting->sync();
    m_setting->endGroup();
}

void RightClickMenu::unfixedFromAllActionTriggerSlot()
{
    m_actionNumber=2;
    QFileInfo fileInfo(m_desktopfp);
    QString desktopfn=fileInfo.fileName();
    m_setting->beginGroup("lockapplication");
    Q_FOREACH(QString desktop,m_setting->allKeys())
    {
        if(m_setting->value(desktop).toInt() > m_setting->value(desktopfn).toInt())
        {
            m_setting->setValue(desktop,m_setting->value(desktop).toInt()-1);
        }
    }
    m_setting->remove(desktopfn);
    m_setting->sync();
    m_setting->endGroup();
//    m_setting->beginGroup("application");
//    m_setting->remove(desktopfn);
//    m_setting->sync();
//    m_setting->endGroup();
//    m_setting->beginGroup("datetime");
//    m_setting->remove(desktopfn);
//    m_setting->sync();
//    m_setting->endGroup();
}

void RightClickMenu::fixToTaskbarActionTriggerSlot()
{
    QDBusInterface iface("com.ukui.panel.desktop",
                         "/",
                         "com.ukui.panel.desktop",
                         QDBusConnection::sessionBus());
    QDBusReply<QVariant> ret=iface.call("AddToTaskbar",m_desktopfp);
    //qDebug()<<m_desktopfp<<;
    m_actionNumber=3;
}

void RightClickMenu::unfixedFromTaskbarActionTriggerSlot()
{
    QDBusInterface iface("com.ukui.panel.desktop",
                         "/",
                         "com.ukui.panel.desktop",
                         QDBusConnection::sessionBus());
    QDBusReply<QVariant> ret=iface.call("RemoveFromTaskbar",m_desktopfp);
    m_actionNumber=4;
}

void RightClickMenu::addToDesktopActionTriggerSlot()
{
    QString path=QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    QFileInfo fileInfo(m_desktopfp);
    QString desktopfn=fileInfo.fileName();
    QFile file(m_desktopfp);
    QString newname=QString(path+"/"+desktopfn);
    bool ret=file.copy(QString(path+"/"+desktopfn));
    if(ret)
    {
        char command[200];
        sprintf(command,"chmod a+x %s",newname.toLocal8Bit().data());
        QProcess::startDetached(QString::fromLocal8Bit(command));
    }
    m_actionNumber=5;
}

void RightClickMenu::uninstallActionTriggerSlot()
{
    QString cmd=QString("dpkg -S "+m_desktopfp);
    m_cmdProc->setReadChannel(QProcess::StandardOutput);
    m_cmdProc->start("sh",QStringList()<<"-c"<<cmd);
    m_cmdProc->waitForFinished();
    m_cmdProc->waitForReadyRead();
    m_cmdProc->close();
    m_actionNumber=6;
}

void RightClickMenu::onReadOutput()
{
    QString packagestr=QString::fromLocal8Bit(m_cmdProc->readAllStandardOutput().data());
    QString packageName=packagestr.split(":").at(0);
    char command[100];
    sprintf(command,"ubuntu-kylin-software-center -remove %s",packageName.toLocal8Bit().data());
    QProcess::startDetached(command);
}

void RightClickMenu::attributeActionTriggerSlot()
{
    char command[100];
    sprintf(command,"ukui-menu-attr %s",m_desktopfp.toLocal8Bit().data());
    QProcess::startDetached(command);
    m_actionNumber=7;
}
int RightClickMenu::showAppBtnMenu(QString desktopfp,bool isinstall)
{
    m_actionNumber=0;
    this->m_desktopfp.clear();
    this->m_desktopfp=desktopfp;
    QMenu menu;
    //添加菜单项，指定图标、名称、响应函数
//    m_setting->beginGroup("lockapplication");
//    QFileInfo fileInfo(desktopfp);
//    QString desktopfn=fileInfo.fileName();
//    if(!m_setting->contains(desktopfn))
//        menu.addAction(QIcon(getIconPixmap(":/data/img/mainviewwidget/fixed.svg",0)),tr("Pin to all"),
//                       this,SLOT(fixToAllActionTriggerSlot()));
//    else
//        menu.addAction(QIcon(getIconPixmap(":/data/img/mainviewwidget/unfixed.svg",0)),tr("Unpin from all"),
//                       this,SLOT(unfixedFromAllActionTriggerSlot()));
//    m_setting->endGroup();
    QDBusInterface iface("com.ukui.panel.desktop",
                         "/",
                         "com.ukui.panel.desktop",
                         QDBusConnection::sessionBus());

    QDBusReply<bool> ret=iface.call("CheckIfExist",desktopfp);
    if(!ret)
        menu.addAction(QIcon(getIconPixmap(":/img/fixed.svg",0)),tr("Pin to taskbar"),
                       this,SLOT(fixToTaskbarActionTriggerSlot()));
    else
        menu.addAction(QIcon(getIconPixmap(":/img/unfixed.svg",0)),tr("Unpin from taskbar"),
                       this,SLOT(unfixedFromTaskbarActionTriggerSlot()));

//    menu.addAction(QIcon(getIconPixmap(":/img/fixed.svg",0)),tr("Add to desktop shortcuts"),
//                   this,SLOT(addToDesktopActionTriggerSlot()));

    QAction *clickaction=new QAction(this);
    clickaction->setText(tr("Add to desktop shortcuts"));
    clickaction->setIcon(QIcon(getIconPixmap(":/img/fixed.svg",0)));
    connect(clickaction,&QAction::triggered,this,&RightClickMenu::addToDesktopActionTriggerSlot);
    menu.addAction(clickaction);
    qDebug()<<"desktopfp"<<desktopfp;
    QFileInfo fileInfo(desktopfp);
    QString desktopfn=fileInfo.fileName();
    QString desktopfp1=QDir::homePath()+"/桌面/"+desktopfn;
    QFileInfo fileInfo1(desktopfp1);
    if(fileInfo1.exists())
    {
        clickaction->setEnabled(false);
    }else
    {
        clickaction->setEnabled(true);
    }
    menu.addSeparator();
    if(isinstall)
    {
    menu.addAction(QIcon(getIconPixmap(":/img/uninstall.svg",0)),tr("Uninstall"),
                   this,SLOT(uninstallActionTriggerSlot()));
    }

    menu.setAttribute(Qt::WA_TranslucentBackground);
    menu.exec(QCursor::pos());
    return m_actionNumber;
}

//int RightClickMenu::showShutdownMenu()
//{
//    m_actionNumber=0;
//    QMenu menu;
//    menu.addAction(QIcon(getIconPixmap("stock-people-symbolic",1)),tr("Switch User"),
//                   this,SLOT(switchUserActionTriggerSlot()));
//    if(QGSettings::isSchemaInstalled(QString("org.ukui.session").toLocal8Bit()))
//    {
//        QGSettings* gsetting=new QGSettings(QString("org.ukui.session").toLocal8Bit());
//        if(gsetting->keys().contains(QString("canhibernate")))
//        {
//            if(gsetting->get("canhibernate").toBool())
//                menu.addAction(QIcon(getIconPixmap("kylin-hebernate-symbolic",1)),tr("Hibernate"),
//                               this,SLOT(hibernateActionTriggerSlot()));
//        }
//    }
//    menu.addAction(QIcon(getIconPixmap("kylin-sleep-symbolic",1)),tr("Sleep"),
//                   this,SLOT(sleepActionTriggerSlot()));
//    menu.addAction(QIcon(getIconPixmap("system-lock-screen-symbolic",1)),tr("Lock Screen"),
//                   this,SLOT(lockScreenActionTriggerSlot()));
//    menu.addAction(QIcon(getIconPixmap("system-logout-symbolic",1)),tr("Log Out"),
//                   this,SLOT(logoutActionTriggerSlot()));
//    menu.addAction(QIcon(getIconPixmap("system-restart-symbolic",1)),tr("Restart"),
//                   this,SLOT(rebootActionTriggerSlot()));
//    menu.addAction(QIcon(getIconPixmap("exit-symbolic",1)),tr("Power Off"),
//                   this,SLOT(shutdownActionTriggerSlot()));
//    menu.setAttribute(Qt::WA_TranslucentBackground);
////    menu.setStyleSheet(m_style);
//    menu.exec(QCursor::pos());
//    return m_actionNumber;
//}

//int RightClickMenu::showOtherMenu(QString desktopfp)
//{
//    m_actionNumber=0;
//    this->m_desktopfp.clear();
//    this->m_desktopfp=desktopfp;

//    QMenu menu;
//    QDBusInterface iface("com.ukui.panel.desktop",
//                         "/",
//                         "com.ukui.panel.desktop",
//                         QDBusConnection::sessionBus());

//    QDBusReply<bool> ret=iface.call("CheckIfExist",this->m_desktopfp);
//    if(!ret)
//        menu.addAction(QIcon(getIconPixmap(":/data/img/sidebarwidget/fixed.svg",0)),tr("Pin to taskbar"),
//                       this,SLOT(fixToTaskbarActionTriggerSlot()));
//    else
//        menu.addAction(QIcon(getIconPixmap(":/data/img/sidebarwidget/unfixed.svg",0)),tr("Unpin from taskbar"),
//                       this,SLOT(unfixedFromTaskbarActionTriggerSlot()));
//    menu.addAction(QIcon(getIconPixmap(":/data/img/sidebarwidget/setting.svg",0)),tr("Personalize this list"),
//                   this,SLOT(otherListActionTriggerSlot()));
//    menu.setAttribute(Qt::WA_TranslucentBackground);
////    menu.setStyleSheet(m_style);
//    menu.exec(QCursor::pos());
//    return m_actionNumber;
//}
