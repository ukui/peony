#include "tab-page.h"
#include "directory-view-container.h"
#include "directory-view-factory-manager.h"
#include "directory-view-plugin-iface.h"
#include "file-info.h"
#include "file-utils.h"

#include <QTabBar>

#include <QDebug>

using namespace Peony;

TabPage::TabPage(QWidget *parent) : QTabWidget(parent)
{
    setMovable(true);
    setDocumentMode(true);
    setElideMode(Qt::ElideRight);
    setTabsClosable(true);
    setUsesScrollButtons(true);
    tabBar()->setExpanding(false);
    tabBar()->setAutoHide(true);

    connect(this, &QTabWidget::tabCloseRequested, [=](int index){
        auto container = dynamic_cast<DirectoryViewContainer*>(widget(index));
        container->disconnect();
        container->deleteLater();
    });

    connect(this, &QTabWidget::currentChanged, [=](int index){
        Q_UNUSED(index)
        Q_EMIT this->currentActiveViewChanged();

        this->rebindContainer();
    });
}

DirectoryViewContainer *TabPage::getActivePage()
{
    return qobject_cast<DirectoryViewContainer*>(currentWidget());
}

void TabPage::addPage(const QString &uri)
{
    auto container = new DirectoryViewContainer(this);
    container->switchViewType(DirectoryViewFactoryManager::getInstance()->getDefaultViewId());
    container->getProxy()->setDirectoryUri(uri);
    container->getProxy()->beginLocationChange();

    container->connect(container->getProxy(), &Peony::DirectoryViewProxyIface::viewDoubleClicked, [=](const QString &uri){
        qDebug()<<"double clicked"<<uri;
        auto info = Peony::FileInfo::fromUri(uri);
        if (info->isDir() || info->isVolume() || uri.startsWith("network:")) {
            Q_EMIT this->updateWindowLocationRequest(uri);
        }
    });

    addTab(container,
           QIcon::fromTheme(FileUtils::getFileIconName(uri), QIcon::fromTheme("folder")),
           FileUtils::getFileDisplayName(uri));
}

void TabPage::rebindContainer()
{
    for (int i = 0; i < this->count(); i++) {
        this->widget(i)->disconnect();
    }

    auto container = getActivePage();
    container->connect(container->getProxy(), &Peony::DirectoryViewProxyIface::viewDoubleClicked, [=](const QString &uri){
        qDebug()<<"double clicked"<<uri;
        auto info = Peony::FileInfo::fromUri(uri);
        if (info->isDir() || info->isVolume() || uri.startsWith("network:")) {
            Q_EMIT this->updateWindowLocationRequest(uri);
        }
    });

    container->connect(container, &DirectoryViewContainer::updateWindowLocationRequest,
                       this, &TabPage::updateWindowLocationRequest);
    container->connect(container, &DirectoryViewContainer::directoryChanged,
                       this, &TabPage::currentLocationChanged);
    container->connect(container, &DirectoryViewContainer::selectionChanged,
                       this, &TabPage::currentSelectionChanged);
}

void TabPage::refreshCurrentTabText()
{
    auto uri = getActivePage()->getCurrentUri();
    setTabText(currentIndex(), FileUtils::getFileDisplayName(uri));
    setTabIcon(currentIndex(),
               QIcon::fromTheme(FileUtils::getFileIconName(uri),
                                QIcon::fromTheme("folder")));
}

void TabPage::stopLocationChange()
{
    auto view = getActivePage();
    //if (view)
}
