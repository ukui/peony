#include "directory-view-container.h"
#include "directory-view-plugin-iface.h"
#include "directory-view-factory-manager.h"
#include "standard-view-proxy.h"
#include "file-utils.h"

#include <QVBoxLayout>

using namespace Peony;

DirectoryViewContainer::DirectoryViewContainer(QWidget *parent) : QWidget(parent)
{
    m_proxy = new DirectoryView::StandardViewProxy;

    setContentsMargins(0, 0, 0, 0);
    m_layout = new QVBoxLayout(this);
    m_layout->setMargin(0);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(0);
    setLayout(m_layout);

    connect(m_proxy, &DirectoryViewProxyIface::viewDirectoryChanged,
            this, &DirectoryViewContainer::directoryChanged);

    connect(m_proxy, &DirectoryViewProxyIface::viewSelectionChanged,
            this, &DirectoryViewContainer::selectionChanged);
}

DirectoryViewContainer::~DirectoryViewContainer()
{
    m_proxy->closeProxy();
    if (m_proxy->getView())
        m_proxy->getView()->closeView();
}

const QStringList DirectoryViewContainer::getBackList()
{
    QStringList l;
    for (auto uri : m_back_stack) {
        l<<uri;
    }
    return l;
}

const QStringList DirectoryViewContainer::getForwardList()
{
    QStringList l;
    for (auto uri : l) {
        l<<uri;
    }
    return l;
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
    m_forward_stack.push(getCurrentUri());
    Q_EMIT updateWindowLocationRequest(uri, false);
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
    m_back_stack.push(getCurrentUri());

    Q_EMIT updateWindowLocationRequest(uri, false);
}

bool DirectoryViewContainer::canCdUp()
{
    return !FileUtils::getParentUri(m_proxy->getDirectoryUri()).isNull();
}

void DirectoryViewContainer::cdUp()
{
    if (!canCdUp())
        return;

    auto uri = FileUtils::getParentUri(m_proxy->getDirectoryUri());
    if (uri.isNull())
        return;

    Q_EMIT updateWindowLocationRequest(uri, true);
}

void DirectoryViewContainer::goToUri(const QString &uri, bool addHistory)
{
    if (uri.isNull())
        return;

    if (getCurrentUri() == uri)
        return;

    if (addHistory) {
        m_forward_stack.clear();
        m_back_stack.push(getCurrentUri());
    }

    m_proxy->setDirectoryUri(uri);
    m_proxy->beginLocationChange();
    //m_active_view_prxoy->setDirectoryUri(uri);

    m_current_uri = uri;
}

void DirectoryViewContainer::switchViewType(const QString &viewId)
{
    if (!m_proxy)
        return;

    if (m_proxy->getView()) {
        if (viewId == m_proxy->getView()->viewId())
            return;
    }

    auto viewManager = DirectoryViewFactoryManager::getInstance();
    auto factory = viewManager->getFactory(viewId);
    if (!factory)
        return;

    auto view = factory->create();
    //connect the signal.
    view->setProxy(m_proxy);

    auto oldView = m_proxy->getView();
    if (oldView) {
        m_layout->removeWidget(dynamic_cast<QWidget*>(oldView));
    }
    m_proxy->switchView(view);
    m_layout->addWidget(dynamic_cast<QWidget*>(view), Qt::AlignBottom);

    Q_EMIT viewTypeChanged();
}

void DirectoryViewContainer::refresh()
{
    m_proxy->beginLocationChange();
}

void DirectoryViewContainer::bindNewProxy(DirectoryViewProxyIface *proxy)
{
    //disconnect old proxy
    //connect new proxy
}

const QStringList DirectoryViewContainer::getCurrentSelections()
{
    if (getProxy()) {
        return getProxy()->getSelections();
    }
    return QStringList();
}

const QString DirectoryViewContainer::getCurrentUri()
{
    if (getProxy()) {
        return getProxy()->getDirectoryUri();
    }
    return nullptr;
}
