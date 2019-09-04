#include "standard-view-proxy.h"

using namespace Peony;
using namespace Peony::DirectoryView;

StandardViewProxy::StandardViewProxy(DirectoryViewIface *view, QObject *parent) : DirectoryViewProxyIface(parent)
{
    m_view = view;
}

StandardViewProxy::~StandardViewProxy()
{
    m_back_stack.clear();
    m_forward_statck.clear();
}

//history
void StandardViewProxy::setDirectoryUri(const QString &uri, bool addHistory)
{
    if (addHistory) {
        m_forward_statck.clear();
        m_back_stack.push(uri);
    }
    m_view->setDirectoryUri(uri);
}

bool StandardViewProxy::canBack()
{
    return !m_back_stack.isEmpty();
}

void StandardViewProxy::goBack()
{
    if (canBack()) {
        auto uri = m_back_stack.pop();
        m_forward_statck.push(uri);
        m_view->setDirectoryUri(uri);
        m_view->beginLocationChange();
    }
}

bool StandardViewProxy::canForward()
{
    return !m_forward_statck.isEmpty();
}

void StandardViewProxy::goForward()
{
    if (canForward()) {
        auto uri = m_forward_statck.pop();
        m_back_stack.push(uri);
        m_view->setDirectoryUri(uri);
        m_view->beginLocationChange();
    }
}

//clipboard
//FIXME: implement clipboard
void StandardViewProxy::cutSelections()
{

}

void StandardViewProxy::copySelections()
{

}

void StandardViewProxy::pasteSelections()
{
    //selections might not be this view's selection
}

QStringList StandardViewProxy::getClipboardSelections()
{
    return QStringList();
}

bool StandardViewProxy::getClipboardSelectionsIsCut()
{
    return false;
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

//zoom
bool StandardViewProxy::canZoomIn()
{
    return m_view->canZoomIn();
}

bool StandardViewProxy::canZoomOut()
{
    return m_view->canZoomOut();
}

void StandardViewProxy::zoomIn()
{
    m_view->zoomIn();
}

void StandardViewProxy::zoomOut()
{
    m_view->zoomOut();
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
