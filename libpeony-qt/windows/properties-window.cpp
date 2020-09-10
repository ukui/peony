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

PropertiesWindow::PropertiesWindow(const QStringList &uris, QWidget *parent) : QMainWindow (parent)
{
    setContextMenuPolicy(Qt::CustomContextMenu);
    m_uris = uris;
    if (uris.contains("computer:///"))
    {
        gotoAboutComputer();
    }
    else
    {
        setWindowIcon(QIcon::fromTheme("system-file-manager"));
        setWindowTitle(tr("Properties"));

        setAttribute(Qt::WA_DeleteOnClose);
        setContentsMargins(0, 15, 0, 0);
        setMinimumSize(360, 480);
        auto w = new PropertiesWindowPrivate(uris, this);
        setCentralWidget(w);

        QToolBar *buttonToolBar = new QToolBar(this);
        addToolBar(Qt::BottomToolBarArea, buttonToolBar);

        //comment close button in bottom
//        QDialogButtonBox *box = new QDialogButtonBox(QDialogButtonBox::Close,
//                buttonToolBar);
//        box->setContentsMargins(0, 5, 5, 5);
//        //get close button and do translate operation
//        box->button(QDialogButtonBox::Close)->setText(tr("Close"));
//        buttonToolBar->addWidget(box);

//        connect(box, &QDialogButtonBox::rejected, [=]() {
//            this->close();
//        });
    }
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

//properties window
PropertiesWindowPrivate::PropertiesWindowPrivate(const QStringList &uris, QWidget *parent) : QTabWidget(parent)
{
    setTabsClosable(false);
    setMovable(false);

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
