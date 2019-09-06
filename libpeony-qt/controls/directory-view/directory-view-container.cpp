#include "directory-view-container.h"
#include "directory-view-plugin-iface.h"
#include "file-utils.h"

using namespace Peony;

DirectoryViewContainer::DirectoryViewContainer(QWidget *parent) : QWidget(parent)
{

}

DirectoryViewContainer::~DirectoryViewContainer()
{
    for (auto proxy : m_proxys) {
        //delete proxy->getView();
        proxy->close();
    }
}

bool DirectoryViewContainer::canGoBack()
{
    return !m_back_stack.isEmpty();
}

void DirectoryViewContainer::goBack()
{
    if (!canGoBack())
        return;

    auto uri = m_back_stack.pop();
    m_forward_stack.push(uri);
    setDirectoryUri(uri, false);
}

bool DirectoryViewContainer::canGoForward()
{
    return !m_forward_stack.isEmpty();
}

void DirectoryViewContainer::goForward()
{
    if (!canGoForward())
        return;

    auto uri = m_forward_stack.pop();
    m_back_stack.push(uri);
    setDirectoryUri(uri, false);
}

bool DirectoryViewContainer::canCdUp()
{
    return !FileUtils::getParentUri(m_active_view_prxoy->getDirectoryUri()).isNull();
}

void DirectoryViewContainer::cdUp()
{
    auto uri = FileUtils::getParentUri(m_active_view_prxoy->getDirectoryUri());
    if (uri.isNull())
        return;

    setDirectoryUri(uri, true);
}

void DirectoryViewContainer::setDirectoryUri(const QString &uri, bool addHistory)
{
    if (addHistory) {
        m_forward_stack.clear();
        m_back_stack.push(uri);
    }
    //m_active_view_prxoy->setDirectoryUri(uri);
}

void DirectoryViewContainer::switchViewType(const QString &viewId)
{
    //if (viewId == proxy->viewId())
    //  return
    //remove current view from container, add old proxy to m_proxys
    //add new view
    //set current uri and selection
    //clear old proxys (close)
}

void DirectoryViewContainer::refresh()
{
    //m_active_view_prxoy->setDirectoryUri(currentUri)
}

void DirectoryViewContainer::bindNewProxy(DirectoryViewProxyIface *proxy)
{
    //disconnect old proxy
    //connect new proxy
}
