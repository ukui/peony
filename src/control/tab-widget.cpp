#include "tab-widget.h"
//#include "navigation-tab-bar.h"

#include "preview-page-factory-manager.h"
#include "preview-page-plugin-iface.h"

#include <QStackedWidget>
#include <QToolButton>
#include <QHBoxLayout>
#include <QDockWidget>
#include <QToolBar>

#include <QAction>

#include <QTimer>

#include "directory-view-container.h"

TabWidget::TabWidget(QWidget *parent) : QMainWindow(parent)
{
    setAttribute(Qt::WA_TranslucentBackground);

    m_tab_bar = new NavigationTabBar(this);
    m_tab_bar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_stack = new QStackedWidget(this);
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

    QToolBar *t = new QToolBar(this);
    t->setContextMenuPolicy(Qt::CustomContextMenu);
    t->setContentsMargins(0, 0, 0, 0);
    t->setMovable(false);
    t->addWidget(m_tab_bar);
    auto manager = Peony::PreviewPageFactoryManager::getInstance();
    auto pluginNames = manager->getPluginNames();
    for (auto name : pluginNames) {
        auto plugin = manager->getPlugin(name);
        auto action = t->addAction(plugin->icon(), plugin->name());
        action->setCheckable(true);
        auto button = qobject_cast<QToolButton *>(t->widgetForAction(action));
        button->setIcon(plugin->icon());
        button->setToolTip(plugin->name());
        button->setWhatsThis(plugin->description());
        button->setFixedSize(QSize(26, 26));
        button->setIconSize(QSize(16, 16));
        //m_buttons->addButton(button);

        connect(action, &QAction::triggered, this, [=](bool checked){
            Q_EMIT m_buttons->previewPageButtonTrigger(checked, plugin->name());
        });
    }

    addToolBar(t);

    addDockWidget(Qt::RightDockWidgetArea, m_preview_page_container);
    m_preview_page_container->hide();

    auto c = new QDockWidget(this);
    c->setTitleBarWidget(new QWidget(this));
    c->setWidget(m_stack);
    addDockWidget(Qt::LeftDockWidgetArea, c);
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
