#include "standard-view-proxy.h"
#include "directory-view-factory-manager.h"

using namespace Peony;
using namespace Peony::DirectoryView;

StandardViewProxy::StandardViewProxy(QObject *parent) : DirectoryViewProxyIface(parent)
{

}

StandardViewProxy::StandardViewProxy(DirectoryViewIface *view, QObject *parent) : DirectoryViewProxyIface(parent)
{
    m_view = view;
}

StandardViewProxy::~StandardViewProxy()
{

}

//history
void StandardViewProxy::setDirectoryUri(const QString &uri)
{
    m_view->setDirectoryUri(uri);
}

void StandardViewProxy::setCutFiles(const QStringList &uri)
{
    m_view->setCutFiles(uri);
}

//selection
void StandardViewProxy::setSelections(const QStringList &uris)
{
    m_view->setSelections(uris);
}

const QStringList StandardViewProxy::getSelections()
{
    return m_view->getSelections();
}

void StandardViewProxy::invertSelections()
{
    m_view->invertSelections();
}

void StandardViewProxy::scrollToSelection(const QString &uri)
{
    m_view->scrollToSelection(uri);
}

//location
const QString StandardViewProxy::getDirectoryUri()
{
    return m_view->getDirectoryUri();
}

void StandardViewProxy::beginLocationChange()
{
    m_view->beginLocationChange();
}

void StandardViewProxy::stopLocationChange()
{
    m_view->stopLocationChange();
    //NOTE:
    //although we stop/cancel the location change,
    //we should assume that we have end a location
    //change operation in FMWindow.
    //FIXME:
    //maybe i should use another signal to distinguish
    //that wether location change was done or force stopped.
    Q_EMIT this->viewDirectoryChanged();
}

//other
//FIXME: open should not be here.
void StandardViewProxy::open(const QStringList &uris, bool newWindow)
{
    m_view->open(uris, newWindow);
}

void StandardViewProxy::closeProxy()
{
    this->deleteLater();
}

void StandardViewProxy::switchView(DirectoryViewIface *view)
{
    if (m_view == view)
        return;

    auto old_view = m_view;
    if (old_view)
        old_view->closeView();

    m_view = view;
}

const QStringList StandardViewProxy::getAllFileUris()
{
    return m_view->getAllFileUris();
}

void StandardViewProxy::editUri(const QString &uri)
{
    m_view->editUri(uri);
}

void StandardViewProxy::editUris(const QStringList uris)
{
    m_view->editUris(uris);
}
