#include "navigation-bar.h"

#include "navigation-tool-bar.h"
#include "advanced-location-bar.h"
#include "directory-view-container.h"

#include "preview-page-factory-manager.h"
#include "preview-page-plugin-iface.h"

#include <QDebug>

using namespace Peony;

NavigationBar::NavigationBar(QWidget *parent) : QToolBar(parent)
{
    setContentsMargins(0, 0, 0, 0);
    setFixedHeight(38);
    setMovable(false);
    setFloatable(false);
    m_left_control = new NavigationToolBar(this);
    m_left_control->setFixedWidth(m_left_control->sizeHint().width());

    m_left_control->setContentsMargins(0, 0, 0, 0);
    addWidget(m_left_control);
    addSeparator();

    m_center_control = new AdvancedLocationBar(this);
    m_center_control->setContentsMargins(0, 0, 0, 0);
    addWidget(m_center_control);
    addSeparator();

    connect(m_left_control, &NavigationToolBar::updateWindowLocationRequest,
            this, &NavigationBar::updateWindowLocationRequest);
    connect(m_left_control, &NavigationToolBar::refreshRequest,
            this, &NavigationBar::refreshRequest);
    connect(m_center_control, &AdvancedLocationBar::updateWindowLocationRequest,
            this, &NavigationBar::updateWindowLocationRequest);
    connect(m_center_control, &AdvancedLocationBar::refreshRequest,
            this, &NavigationBar::refreshRequest);

    auto manager = PreviewPageFactoryManager::getInstance();
    auto ids = manager->getPluginNames();
    QActionGroup *group = new QActionGroup(this);
    group->setExclusive(true);
    for (auto id : ids) {
        auto factory = manager->getPlugin(id);
        auto action = group->addAction(factory->icon(), factory->name());
        action->setCheckable(true);
        connect(action, &QAction::triggered, [=](){
            if (m_checked_preview_action == action) {
                action->setChecked(false);
                m_checked_preview_action = nullptr;
            } else {
                m_checked_preview_action = action;
                action->setChecked(true);
            }
            Q_EMIT this->switchPreviewPageRequest(m_checked_preview_action? m_checked_preview_action->text(): nullptr);
        });
    }
    addActions(group->actions());
}

void NavigationBar::bindContainer(DirectoryViewContainer *container)
{
    m_left_control->setCurrentContainer(container);
    m_left_control->updateActions();
    updateLocation(container->getCurrentUri());
}

void NavigationBar::updateLocation(const QString &uri)
{
    m_center_control->updateLocation(uri);
    m_left_control->updateActions();
}

void NavigationBar::setBlock(bool block)
{
    qDebug()<<"block"<<block;
    this->blockSignals(block);
    m_left_control->blockSignals(block);
    m_center_control->blockSignals(block);
    m_left_control->setDisabled(block);
    m_center_control->setDisabled(block);
}
