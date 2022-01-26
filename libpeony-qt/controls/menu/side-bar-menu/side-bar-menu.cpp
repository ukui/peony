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

#include "side-bar-menu.h"
#include "side-bar-abstract-item.h"

#include "bookmark-manager.h"
#include "properties-window.h"

#include "menu-plugin-manager.h"

#include "file-utils.h"
#include "file-info.h"
#include "file-info-job.h"

#include <QAction>
#include <QModelIndex>
#include <format_dialog.h>

#include <QDebug>
#include <volume-manager.h>

using namespace Peony;

static const int FAVORITE_CAN_NOT_DELETE_URI_COUNT=7;

SideBarMenu::SideBarMenu(SideBarAbstractItem *item, SideBar *sideBar, QWidget *parent) : QMenu (parent)
{
    m_uri = item->uri();
    m_item = item;
    m_side_bar = sideBar;

    if (!item) {
        auto action = addAction(QIcon::fromTheme("preview-file"), tr("Properties"));
        action->setEnabled(false);
        return;
    }

    switch (item->type()) {
    case SideBarAbstractItem::FavoriteItem: {
        constructFavoriteActions();
        break;
    }
    case SideBarAbstractItem::PersonalItem: {
        constructPersonalActions();
        break;
    }
    case SideBarAbstractItem::FileSystemItem: {
        constructFileSystemItemActions();
        break;
    }
    case SideBarAbstractItem::NetWorkItem: {
        constructNetWorkItemActions();
        break;
    }
    default: {
        auto action = addAction(QIcon::fromTheme("preview-file"), tr("Properties"));
        action->setEnabled(false);
        break;
    }
    }
}

const QList<QAction *> SideBarMenu::constructFavoriteActions()
{
    QList<QAction *> l;

    l<<addAction(QIcon::fromTheme("window-close-symbolic"), tr("Delete Symbolic"), [=]() {
        BookMarkManager::getInstance()->removeBookMark(m_uri);
    });

    if (!m_item->firstColumnIndex().parent().isValid()) {
        l.last()->setEnabled(false);
    } else if (m_item->firstColumnIndex().row()<FAVORITE_CAN_NOT_DELETE_URI_COUNT) {
        l.last()->setEnabled(false);
    }
    else if (m_uri == "favorite:///data/usershare?schema=file" || m_uri == "kmre:///" || m_uri == "kydroid:///")
    {
        //fix bug#68431, can not delete option issue
        l.last()->setEnabled(false);
    }

    l<<addAction(QIcon::fromTheme("preview-file"), tr("Properties"), [=]() {
        PropertiesWindow *w = new PropertiesWindow(QStringList()<<m_uri);
        w->show();
    });
    if (!m_item->firstColumnIndex().parent().isValid()) {
        l.last()->setEnabled(false);
    }

    return l;
}

const QList<QAction *> SideBarMenu::constructPersonalActions()
{
    QList<QAction *> l;

    l<<addAction(QIcon::fromTheme("preview-file"), tr("Properties"), [=]() {
        PropertiesWindow *w = new PropertiesWindow(QStringList()<<m_uri);
        w->show();
    });

    return l;
}

#include "file-enumerator.h"
const QList<QAction *> SideBarMenu::constructFileSystemItemActions()
{
    QList<QAction *> l;
    /* 卸载 */
    bool isWayland = qApp->property("isWayland").toBool(); // related to #105070
    if (isWayland) {
        if (m_item->isUnmountable()) {
            l<<addAction(QIcon::fromTheme("media-eject-symbolic"), tr("Unmount"), [=]() {
                m_item->unmount();
            });
            l.last()->setEnabled(m_item->isMounted());
        }
    } else {
        /*  可用的U盘、外接移动硬盘、外接移动光盘, 右键菜单里不允许有“卸载”选项，bug#83206 */
        if (!(m_item->isEjectable() || m_item->isStopable()) && m_item->isUnmountable()) {
            l<<addAction(QIcon::fromTheme("media-eject-symbolic"), tr("Unmount"), [=]() {
                m_item->unmount();
            });
            l.last()->setEnabled(m_item->isMounted());
        }
    }

    /* 弹出 */
    if (m_item->isEjectable()||m_item->isStopable()) {
        l<<addAction(QIcon::fromTheme("media-eject-symbolic"), tr("Eject"), [=](){
            m_item->eject(G_MOUNT_UNMOUNT_NONE);
        });

        //l.last()->setEnabled(m_item->isMounted());
    }


    QString unixDevice = m_item->getDevice();
    QString uri;
    if(m_uri=="file:///") /* 文件系统特殊处理 */
        uri = "computer:///root.link";
    else if(!unixDevice.isEmpty())/* 由于格式化、属性、插件（例如：发送到移动设备）等未重构，还是需要用之前的computer uri */
        uri = getComputerUriFromUnixDevice(unixDevice);
    else
        uri=m_uri;

    //not allow format data block, fix bug#66471，66479
    QString targetUri = FileUtils::getTargetUri(m_uri);
    bool isData = m_uri == "file:///data" || targetUri == "file:///data"
            || (m_uri.startsWith("file:///media") && m_uri.endsWith("/data"))
            || (targetUri.startsWith("file:///media") && targetUri.endsWith("/data"));

    /* 光盘暂时没有格式化功能以及文件系统、手机要求不能格式化 */
    /* 没有uri的item不能格式化，FormatDialog需要uri走流程，否则会导致崩溃问题 */
    //fix bug#92380, file system has format option issue
    bool showFormatDialog = m_uri!="file:///" && m_uri != "computer:///root.link"
            && (!unixDevice.isNull() && ! unixDevice.contains("/dev/sr"))
            &&!unixDevice.startsWith("/dev/bus/usb")
            && (m_item->isVolume()) && !m_item->uri().isEmpty();
    if(showFormatDialog)
    {
        if (!isWayland && isData) {
            // skip
        } else {
            l<<addAction(QIcon::fromTheme("preview-file"), tr("format"), [=]() {
                auto info = FileInfo::fromUri(uri);
                if (info->targetUri ().isEmpty ()) {
                    FileInfoJob job (uri, this);
                    job.querySync ();
                }
                Format_Dialog *fd  = new Format_Dialog(uri,m_item);
                fd->show();
            });
        }
    }

    /* 插件 */
    if (!isWayland && isData) {
        //skip
    } else {
        if(0 != QString::compare(m_uri, "filesafe:///")) {
            auto mgr = MenuPluginManager::getInstance();
            auto ids = mgr->getPluginIds();
            for (auto id : ids) {
                auto factory = mgr->getPlugin(id);
                //qDebug()<<id;
                auto tmp = factory->menuActions(MenuPluginInterface::SideBar, uri, QStringList()<<uri);
                addActions(tmp);
                for (auto action : tmp) {
                    action->setParent(this);
                }
                l<<tmp;
            }
        }
    }

    /* 属性 */
    l<<addAction(QIcon::fromTheme("preview-file"), tr("Properties"), [=]() {
        //fix computer show properties crash issue, link to bug#77789
        if (m_uri == "computer:///" || m_uri == "//")
        {
            gotoAboutComputer();
        }
        else{
            PropertiesWindow *w = new PropertiesWindow(QStringList()<<uri);
            w->show();
        }
    });
    if ((0 != QString::compare(m_uri, "computer:///")) &&
        (0 != QString::compare(m_uri, "filesafe:///"))
            &&(m_item->isVolume())) {
        l.last()->setEnabled(m_item->isMounted());
    }

    return l;
}

void SideBarMenu::gotoAboutComputer()
{
    QProcess p;
    p.setProgram("ukui-control-center");
    //-m About para to show about computer infos, related to bug#88258
    p.setArguments(QStringList()<<"-m" << "About");
#if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
    p.startDetached();
#else
    p.startDetached("ukui-control-center", QStringList()<<"-m" << "About");
#endif
    p.waitForFinished(-1);
}

const QList<QAction *> SideBarMenu::constructNetWorkItemActions()
{
    QList<QAction *> l;
    static const QString netWorkUri="network:///";

    /* 共享文件夹无右键菜单'卸载' */
    if (!m_uri.startsWith("file://")) {
        l<<addAction(QIcon::fromTheme("media-eject-symbolic"), tr("Unmount"), [=]() {
            m_item->unmount();
        });       
        l.last()->setEnabled(m_item->isMounted());
    }
    if(netWorkUri != m_uri){
        l<<addAction(QIcon::fromTheme("preview-file"), tr("Properties"), [=]() {
            if((m_item->isVolume())){
                /* 远程服务器 */
                FileEnumerator e;
                e.setEnumerateDirectory("computer:///");
                e.enumerateSync();
                for (auto fileInfo : e.getChildren()) {
                    FileInfoJob infoJob(fileInfo);
                    infoJob.querySync();

                    /* 由远程服务器的targeturi获取uri来调用属性窗口, */
                    QUrl targetUrl(fileInfo.get()->targetUri());
                    QUrl sourceUrl(m_uri);

                    if(sourceUrl.scheme()==targetUrl.scheme() && sourceUrl.host()==targetUrl.host()){/* 相同scheme和host，但port不同时怎么处理呢？ */
                        QString uri = fileInfo.get()->uri();
                        PropertiesWindow *w = new PropertiesWindow(QStringList()<<uri);
                        w->show();
                        break;
                    }
                }
            }else{
                /* 共享文件夹 */
                PropertiesWindow *w = new PropertiesWindow(QStringList()<<m_uri);
                w->show();
            }
        });
        if(m_item->isVolume())
            l.last()->setEnabled(m_item->isMounted());
    }

    return l;

}

QString SideBarMenu::getComputerUriFromUnixDevice(const QString &unixDevice){
    /* volume item,遍历方式获取uri */
    FileEnumerator e;
    e.setEnumerateDirectory("computer:///");
    e.enumerateSync();
    QString uri;
    for (auto fileInfo : e.getChildren()) {
        FileInfoJob infoJob(fileInfo);
        infoJob.querySync();
        /* 由volume的unixDevice获取computer uir */
        auto info = infoJob.getInfo();
        QString device = fileInfo.get()->unixDeviceFile();
        if(device==unixDevice){
            uri = fileInfo.get()->uri();
            break;
        }
    }
    return uri;
}



