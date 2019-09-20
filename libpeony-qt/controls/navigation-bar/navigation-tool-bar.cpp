#include "navigation-tool-bar.h"
#include "file-utils.h"

using namespace Peony;

NavigationToolBar::NavigationToolBar(QWidget *parent) : QToolBar(parent)
{

}

void NavigationToolBar::onGoBack()
{
    if (canGoBack()) {
        auto uri = m_back_stack.pop();
        m_forward_stack.push(m_current_uri);
        m_current_uri = uri;
    }
}

void NavigationToolBar::onGoForward()
{
    if (canGoForward()) {
        auto uri = m_forward_stack.pop();
        m_back_stack.push(m_current_uri);
        m_current_uri = uri;
    }
}

void NavigationToolBar::onGoToUri(const QString &uri, bool addHistory)
{
    if (uri == m_current_uri)
        return;

    if (!addHistory) {
        if (m_back_stack.contains(uri)) {
            return;
        } else if (m_forward_stack.contains(uri)) {
            return;
        } else {
            return;
        }
    } else {
        m_forward_stack.clear();
        m_back_stack.push(m_current_uri);
        m_current_uri = uri;
        return;
    }
}

bool NavigationToolBar::canCdUp()
{
    if (FileUtils::getParentUri(m_current_uri).isNull())
        return false;
    return true;
}

void NavigationToolBar::clearHistory()
{
    m_back_stack.clear();
    m_forward_stack.clear();
}
