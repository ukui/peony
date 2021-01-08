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

#include "properties-window.h"

#include "properties-window-tab-page-plugin-iface.h"

#include "basic-properties-page-factory.h"
#include "permissions-properties-page-factory.h"
#include "computer-properties-page-factory.h"
#include "recent-and-trash-properties-page-factory.h"

#include <QToolBar>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QProcess>
#include <QDebug>
#include <QStatusBar>
#include <QString>
#include <QHBoxLayout>

#include "file-info.h"

using namespace Peony;

//plugin manager

static PropertiesWindowPluginManager *global_instance = nullptr;

PropertiesWindowPluginManager *PropertiesWindowPluginManager::getInstance()
{
    if (!global_instance)
        global_instance = new PropertiesWindowPluginManager;
    return global_instance;
}

void PropertiesWindowPluginManager::release()
{
    deleteLater();
}

PropertiesWindowPluginManager::PropertiesWindowPluginManager(QObject *parent) : QObject (parent)
{
    //register internal factories.
    registerFactory(BasicPropertiesPageFactory::getInstance());
    registerFactory(PermissionsPropertiesPageFactory::getInstance());
    registerFactory(ComputerPropertiesPageFactory::getInstance());
    registerFactory(RecentAndTrashPropertiesPageFactory::getInstance());
}

PropertiesWindowPluginManager::~PropertiesWindowPluginManager()
{
    for (auto factory : m_factory_hash) {
        factory->closeFactory();
    }
    m_factory_hash.clear();
}


bool PropertiesWindowPluginManager::registerFactory(PropertiesWindowTabPagePluginIface *factory)
{
    m_mutex.lock();
    auto id = factory->name();
    if (m_factory_hash.value(id)) {
        m_mutex.unlock();
        return false;
    }

    m_factory_hash.insert(id, factory);
    m_sorted_factory_map.insert(-factory->tabOrder(), id);
    m_mutex.unlock();
    return true;
}

const QStringList PropertiesWindowPluginManager::getFactoryNames()
{
    QStringList l;
    for (auto factoryId : m_sorted_factory_map) {
        l<<factoryId;
    }
    return l;
}

PropertiesWindowTabPagePluginIface *PropertiesWindowPluginManager::getFactory(const QString &id)
{
    return m_factory_hash.value(id);
}

/*!
 * init PropertiesWindows`s static member
 * \brief PropertiesWindow::s_windowWidth
 */
const qint32 PropertiesWindow::s_windowWidth        = 460;
const qint32 PropertiesWindow::s_windowHeightFolder = 600;
const qint32 PropertiesWindow::s_windowHeightOther  = 652;
const QSize  PropertiesWindow::s_bottomButtonSize   = QSize(100,32);
const QSize  PropertiesWindow::s_topButtonSize      = QSize(65,30);

PropertiesWindow::PropertiesWindow(const QStringList &uris, QWidget *parent) : QMainWindow (parent)
{
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    m_uris = uris;
    qDebug() << "进入窗口创建!";

    if (uris.contains("computer:///"))
        gotoAboutComputer();
    else {

        this->setWindowTitleTextAndIcon();

        this->setAttribute(Qt::WA_DeleteOnClose);
        this->setContentsMargins(0, 5, 0, 0);

        this->setWindowFlags(Qt::WindowCloseButtonHint);

        if(this->notDir())
            //如果含有文件夹，那么高度是600，如果是其他文件，那么高度是652
            this->setFixedSize(PropertiesWindow::s_windowWidth,PropertiesWindow::s_windowHeightOther);
        else
            this->setFixedSize(PropertiesWindow::s_windowWidth,PropertiesWindow::s_windowHeightFolder);

        auto w = new PropertiesWindowPrivate(uris, this);
        this->setCentralWidget(w);

        this->initStatusBar();
    }
}

/*!
 * recent:///  : 最近
 * trash:///   : 回收站
 *
 * \brief PropertiesWindow::setWindowTitleText
 */
void PropertiesWindow::setWindowTitleTextAndIcon()
{
    QString l_windowTitle = "";
    QString l_iconName = "system-file-manager";

    if(m_uris.contains("trash:///")) {
        l_windowTitle = tr("Trash");
        l_iconName = "trash";

    }else if(m_uris.contains("recent:///")) {
        l_windowTitle = tr("Recent");
        l_iconName = "recent";

    }else {
        qint32 l_fileNum = this->m_uris.count();

        if(l_fileNum > 1)
            //use default icon ***
            l_windowTitle = tr("Selected") + QString(tr(" %1 Files")).arg(l_fileNum);

        else {
            std::shared_ptr<FileInfo> l_fineInfo = FileInfo::fromUri(m_uris.at(0));
            qDebug() << "文件信息为空?" << (l_fineInfo.get() == nullptr);
            if(l_fineInfo) {
                l_windowTitle = l_fineInfo.get()->displayName();
                l_iconName = l_fineInfo.get()->iconName();
            }
        }
    }
    l_windowTitle += " " + tr("Properties");

    qDebug() << "图标名称:" << l_iconName << "";

    this->setWindowIcon(QIcon::fromTheme(l_iconName));
    this->setWindowTitle(l_windowTitle);
}

bool PropertiesWindow::notDir()
{
    for(QString uri : m_uris) {
        if(FileInfo::fromUri(uri).get()->isDir())
            return false;
    }
    return true;
}

void PropertiesWindow::show()
{
    if (m_uris.contains("computer:///"))
        close();
    else
        return QWidget::show();
}

void PropertiesWindow::gotoAboutComputer()
{
    QProcess p;
    p.setProgram("ukui-control-center");
    //-a para to show about computer infos
    p.setArguments(QStringList()<<"-a");
#if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
    p.startDetached();
#else
    p.startDetached("ukui-control-center", QStringList()<<"-a");
    p.waitForFinished(-1);
#endif
}

/*!
 *
 * \brief PropertiesWindow::initStatusBar
 */
void PropertiesWindow::initStatusBar(){
    QStatusBar *statusBar = new QStatusBar(this);

    statusBar->setFixedSize(PropertiesWindow::s_windowWidth,64);

    QHBoxLayout *bottomToolLayout = new QHBoxLayout(statusBar);

    QPushButton *okButton = new QPushButton(tr("Ok"),statusBar);
    QPushButton *cancelButton = new QPushButton(QObject::tr("Cancel"),statusBar);

    okButton->setFixedSize(PropertiesWindow::s_bottomButtonSize);
    cancelButton->setFixedSize(PropertiesWindow::s_bottomButtonSize);

    bottomToolLayout->addWidget(cancelButton);
    bottomToolLayout->addWidget(okButton);

    okButton->move(344,16);
    cancelButton->move(236,16);

    statusBar->setLayout(bottomToolLayout);

    this->setStatusBar(statusBar);

    //set cancelButton event process
    connect(cancelButton,&QPushButton::clicked,this,&QMainWindow::close);
}

/*!
 * save all changed settings when 'ok' is clicked
 * \brief PropertiesWindow::saveAllChanged
 */
void PropertiesWindow::saveAllChanged()
{

}

//properties window
PropertiesWindowPrivate::PropertiesWindowPrivate(const QStringList &uris, QWidget *parent) : QTabWidget(parent)
{
    setTabsClosable(false);
    setMovable(false);
    setContentsMargins(0,0,0,0);

    auto manager = PropertiesWindowPluginManager::getInstance();
    auto names = manager->getFactoryNames();
    for (auto name : names) {
        auto factory = manager->getFactory(name);
        if (factory->supportUris(uris)) {
            auto tabPage = factory->createTabPage(uris);
            tabPage->setParent(this);
            addTab(tabPage, factory->name());
        }
    }
}
