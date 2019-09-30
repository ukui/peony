#include "navigation-tool-bar.h"
#include "file-utils.h"

using namespace Peony;

#include "directory-view-container.h"

NavigationToolBar::NavigationToolBar(QWidget *parent) : QToolBar(parent)
{
    m_back_action = addAction(QIcon::fromTheme("gtk-go-back-ltr"), tr("Go Back"), [=](){
        this->onGoBack();
    });

    m_forward_action = addAction(QIcon::fromTheme("gtk-go-forward-ltr"), tr("Go Forward"), [=](){
        this->onGoForward();
    });

    m_history_action = addAction(QIcon::fromTheme("gtk-go-down"), tr("History"), [=](){
        //FIXME:
    });

    m_cd_up_action = addAction(QIcon::fromTheme("gtk-go-up"), tr("Cd Up"), [=](){
        if (m_current_container) {
            m_current_container->cdUp();
        }
    });

    m_refresh_action = addAction(QIcon::fromTheme("gtk-refresh"), tr("Refresh"), [=](){
        Q_EMIT refreshRequest();
    });

    updateActions();
}

void NavigationToolBar::updateActions()
{
    m_back_action->setEnabled(canGoBack());
    m_forward_action->setEnabled(canGoForward());
    m_cd_up_action->setEnabled(canCdUp());
}

bool NavigationToolBar::canCdUp()
{
    if (!m_current_container)
        return false;
    return m_current_container->canCdUp();
}

bool NavigationToolBar::canGoBack()
{
    if (!m_current_container)
        return false;
    return m_current_container->canGoBack();
}

bool NavigationToolBar::canGoForward()
{
    if (!m_current_container)
        return false;
    return m_current_container->canGoForward();
}

void NavigationToolBar::onGoBack()
{
    if (canGoBack()) {
        m_current_container->goBack();
    }
}

void NavigationToolBar::onGoForward()
{
    if (canGoForward()) {
        m_current_container->goForward();
    }
}

void NavigationToolBar::onGoToUri(const QString &uri, bool addHistory)
{
    if (m_current_container) {
        m_current_container->goToUri(uri, addHistory);
        updateWindowLocationRequest(m_current_container->getCurrentUri(), false);
    }
}

void NavigationToolBar::clearHistory()
{

}

void NavigationToolBar::setCurrentContainer(DirectoryViewContainer *container)
{
    if (m_current_container == container)
        return;
    m_current_container = container;
    Q_EMIT updateWindowLocationRequest(container->getCurrentUri(), false);
}
