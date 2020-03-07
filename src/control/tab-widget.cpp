/*
 * Peony-Qt
 *
 * Copyright (C) 2020, Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Authors: Yue Lan <lanyue@kylinos.cn>
 *
 */

#include "tab-widget.h"
//#include "navigation-tab-bar.h"

#include "preview-page-factory-manager.h"
#include "preview-page-plugin-iface.h"

#include <QStackedWidget>
#include <QToolButton>
#include <QHBoxLayout>
#include <QDockWidget>
#include <QToolBar>
#include <QSplitter>

#include <QAction>

#include <QTimer>

#include "directory-view-container.h"

#include "peony-main-window-style.h"

#include <QDebug>

TabWidget::TabWidget(QWidget *parent) : QMainWindow(parent)
{
    setStyle(PeonyMainWindowStyle::getStyle());

    setAttribute(Qt::WA_TranslucentBackground);

    m_tab_bar = new NavigationTabBar(this);
    m_tab_bar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_stack = new QStackedWidget(this);
    m_stack->setContentsMargins(0, 0, 0, 0);
    m_buttons = new PreviewPageButtonGroups(this);
    m_preview_page_container = new QDockWidget(this);
    m_preview_page_container->setTitleBarWidget(new QWidget);
    m_preview_page_container->setFeatures(QDockWidget::NoDockWidgetFeatures);

    connect(m_buttons, &PreviewPageButtonGroups::previewPageButtonTrigger, [=](bool trigger, const QString &id){
        if (trigger) {
            auto plugin = Peony::PreviewPageFactoryManager::getInstance()->getPlugin(id);
            setPreviewPage(plugin->createPreviewPage());
        } else {
            setPreviewPage(nullptr);
        }
    });

    connect(m_tab_bar, &QTabBar::currentChanged, this, &TabWidget::changeCurrentIndex);
    connect(m_tab_bar, &QTabBar::tabMoved, this, &TabWidget::moveTab);
    connect(m_tab_bar, &QTabBar::tabCloseRequested, this, &TabWidget::removeTab);
    connect(m_tab_bar, &NavigationTabBar::addPageRequest, this, &TabWidget::addPage);

    connect(m_tab_bar, &NavigationTabBar::closeWindowRequest, this, &TabWidget::closeWindowRequest);

    QHBoxLayout *t = new QHBoxLayout(this);
    QActionGroup *group = new QActionGroup(this);
    group->setExclusive(true);
    QToolBar *previewButtons = new QToolBar(this);
    previewButtons->setFixedHeight(m_tab_bar->height());
    t->setContentsMargins(0, 0, 5, 0);
    t->addWidget(m_tab_bar);
    auto manager = Peony::PreviewPageFactoryManager::getInstance();
    auto pluginNames = manager->getPluginNames();
    for (auto name : pluginNames) {
        auto factory = manager->getPlugin(name);
        auto action = group->addAction(factory->icon(), factory->name());
        action->setCheckable(true);
        connect(action, &QAction::triggered, [=](/*bool checked*/){
            if (!m_current_preview_action) {
                m_current_preview_action = action;
                action->setChecked(true);
                Q_EMIT m_buttons->previewPageButtonTrigger(true, factory->name());
            } else {
                if (m_current_preview_action == action) {
                    m_current_preview_action = nullptr;
                    action->setChecked(false);
                    Q_EMIT m_buttons->previewPageButtonTrigger(false, factory->name());
                } else {
                    m_current_preview_action = action;
                    action->setChecked(true);
                    Q_EMIT m_buttons->previewPageButtonTrigger(true, factory->name());
                }
            }
        });
    }
    previewButtons->addActions(group->actions());
    t->addWidget(previewButtons);

    QWidget *w = new QWidget();
    w->setAttribute(Qt::WA_TranslucentBackground);
    auto vbox = new QVBoxLayout();
    vbox->setSpacing(0);
    vbox->setContentsMargins(0, 0, 0, 0);
    vbox->addLayout(t);
    QSplitter *s = new QSplitter(this);
    s->setContentsMargins(0, 0, 0, 0);
    s->setHandleWidth(1);
    s->addWidget(m_stack);
    s->addWidget(m_preview_page_container);
    m_preview_page_container->hide();
    vbox->addWidget(s);
    w->setLayout(vbox);
    setCentralWidget(w);
}

void TabWidget::setCurrentIndex(int index)
{
    m_tab_bar->setCurrentIndex(index);
    m_stack->setCurrentIndex(index);
}

void TabWidget::setPreviewPage(Peony::PreviewPageIface *previewPage)
{
    bool visible = false;
    auto previewPageWidget = dynamic_cast<QWidget *>(previewPage);
    if (previewPageWidget)
        visible = true;

    if (m_preview_page) {
        m_preview_page_container->setWidget(nullptr);
        m_preview_page->closePreviewPage();
    }
    m_preview_page = previewPage;
    m_preview_page_container->setWidget(previewPageWidget);

    m_preview_page_container->blockSignals(!visible);
    m_preview_page_container->setVisible(visible);
}

void TabWidget::addPage(const QString &uri, bool jumpTo)
{
    m_tab_bar->addPage(uri, jumpTo);
    auto viewContainer = new Peony::DirectoryViewContainer(m_stack);
    m_stack->addWidget(viewContainer);
    viewContainer->goToUri(uri, true, true);
    if (jumpTo) {
        m_stack->setCurrentWidget(viewContainer);
    }
}

void TabWidget::changeCurrentIndex(int index)
{
    m_tab_bar->setCurrentIndex(index);
    m_stack->setCurrentIndex(index);
    Q_EMIT currentIndexChanged(index);
}

void TabWidget::moveTab(int from, int to)
{
    auto w = m_stack->widget(from);
    if (!w)
        return;
    m_stack->removeWidget(w);
    m_stack->insertWidget(to, w);
    Q_EMIT tabMoved(from, to);
}

void TabWidget::removeTab(int index)
{
    m_tab_bar->removeTab(index);
    m_stack->removeWidget(m_stack->widget(index));
}

PreviewPageButtonGroups::PreviewPageButtonGroups(QWidget *parent) : QButtonGroup(parent)
{
    setExclusive(true);
}
