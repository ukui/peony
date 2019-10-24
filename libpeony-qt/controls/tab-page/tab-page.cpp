#include "tab-page.h"
#include "directory-view-container.h"
#include "directory-view-factory-manager.h"
#include "directory-view-plugin-iface.h"
#include "file-info.h"
#include "file-utils.h"

#include "file-launch-manager.h"

#include <QTabBar>

#include <QUrl>

#include <QDebug>

using namespace Peony;

TabPage::TabPage(QWidget *parent) : QTabWidget(parent)
{
    m_double_click_limiter.setSingleShot(true);

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

    addTab(container,
           QIcon::fromTheme(FileUtils::getFileIconName(uri), QIcon::fromTheme("folder")),
           FileUtils::getFileDisplayName(uri));

    rebindContainer();
}

void TabPage::rebindContainer()
{
    for (int i = 0; i < this->count(); i++) {
        this->widget(i)->disconnect();
    }

    auto container = getActivePage();
    container->connect(container->getProxy(), &Peony::DirectoryViewProxyIface::viewDoubleClicked, [=](const QString &uri){
        if (m_double_click_limiter.isActive())
            return;

        m_double_click_limiter.start(500);

        qDebug()<<"double clicked"<<uri;
        auto info = Peony::FileInfo::fromUri(uri);
        if (info->uri().startsWith("trash://")) {
            //FIXME: open properties window
            return;
        }
        if (info->isDir() || info->isVolume() || uri.startsWith("network:")) {
            Q_EMIT this->updateWindowLocationRequest(uri);
        } else {
            FileLaunchManager::openAsync(uri);
        }
    });

    container->connect(container, &DirectoryViewContainer::updateWindowLocationRequest,
                       this, &TabPage::updateWindowLocationRequest);
    container->connect(container, &DirectoryViewContainer::directoryChanged,
                       this, &TabPage::currentLocationChanged);
    container->connect(container, &DirectoryViewContainer::selectionChanged,
                       this, &TabPage::currentSelectionChanged);
    container->connect(container, &DirectoryViewContainer::menuRequest,
                       this, &TabPage::menuRequest);

    container->connect(container, &DirectoryViewContainer::viewTypeChanged,
                       this, &TabPage::viewTypeChanged);
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
    view->stopLoading();
}
