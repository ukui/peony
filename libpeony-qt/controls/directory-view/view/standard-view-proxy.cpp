#include "standard-view-proxy.h"

using namespace Peony;
using namespace Peony::DirectoryView;

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

QStringList StandardViewProxy::getSelections()
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
}

//other
//FIXME: open should not be here.
void StandardViewProxy::open(const QStringList &uris, bool newWindow)
{
    m_view->open(uris, newWindow);
}

void StandardViewProxy::close()
{
    m_view->close();
}
