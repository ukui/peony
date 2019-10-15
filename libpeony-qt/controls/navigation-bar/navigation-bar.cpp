#include "navigation-bar.h"

#include "navigation-tool-bar.h"
#include "advanced-location-bar.h"
#include "directory-view-container.h"

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

    //FIXME: preview plugins
    addAction(QIcon::fromTheme("preview-file", QIcon::fromTheme("gtk-missing-image")),
              tr("Preview"));
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
