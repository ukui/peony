#include "fm-window.h"
#include "directory-view-plugin-iface.h"
#include "directory-view-factory-manager.h"

using namespace Peony;

FMWindow::FMWindow(const QString &uri, QWidget *parent) : QWidget(parent)
{
    auto manager = DirectoryViewFactoryManager::getInstance();
    auto viewIds = manager->getFactoryNames();
    if (!viewIds.isEmpty()) {
        //FIXME: add option for choosing default view
        auto factory = manager->getFactory(viewIds.first());
        auto view = factory->create();
        view->setDirectoryUri(uri);
        this->m_active_view_proxy = view->getProxy();
        //connect this in controls?
        m_view_list.prepend(view->getProxy());
    }
}

const QString FMWindow::getCurrentUri()
{
    if (m_active_view_proxy) {
        return m_active_view_proxy->getDirectoryUri();
    }
    return nullptr;
}

const QStringList FMWindow::getCurrentSelections()
{
    if (m_active_view_proxy) {
        return m_active_view_proxy->getSelections();
    }
    return QStringList();
}

void FMWindow::setActiveView(DirectoryViewProxyIface *view)
{
    if (view == m_active_view_proxy)
        return;

    if (!m_view_list.contains(view)) {
        m_view_list.prepend(view);
    }

    //disconnect the previous view.
    m_active_view_proxy->disconnect();

    //connect this in controls?
    m_active_view_proxy = view;
    Q_EMIT activeViewChanged(view);
    view->beginLocationChange();
}

void FMWindow::addNewTabs(const QStringList &uris)
{
    auto manager = DirectoryViewFactoryManager::getInstance();
    auto viewIds = manager->getFactoryNames();
    if (!viewIds.isEmpty()) {
        for (auto uri : uris) {
            //FIXME: add option for choosing default view
            auto factory = manager->getFactory(viewIds.first());
            auto view = factory->create();
            view->setDirectoryUri(uri);
            //connect this in controls?
            m_view_list.prepend(view->getProxy());
        }
    }
}

void FMWindow::beginLocationChange()
{
    if (m_active_view_proxy) {
        if (!m_active_view_proxy->getDirectoryUri().isEmpty()) {
            m_active_view_proxy->beginLocationChange();
        }
    }
}

void FMWindow::beginSwitchView(const QString &viewId)
{

}
