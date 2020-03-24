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
#include "directory-view-widget.h"

#include "file-info.h"
#include "file-launch-manager.h"
#include "properties-window.h"

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
    m_tab_bar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_stack = new QStackedWidget(this);
    m_stack->setContentsMargins(0, 0, 0, 0);
    m_buttons = new PreviewPageButtonGroups(this);
    m_preview_page_container = new QStackedWidget(this);
    m_preview_page_container->setMinimumWidth(200);

    connect(m_buttons, &PreviewPageButtonGroups::previewPageButtonTrigger, [=](bool trigger, const QString &id){
        setTriggeredPreviewPage(trigger);
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
    m_tab_bar_bg = new QWidget(this);
    m_tab_bar_bg->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    QToolBar *previewButtons = new QToolBar(this);
    //previewButtons->setFixedHeight(m_tab_bar->height());
    t->setContentsMargins(0, 0, 5, 0);
    t->addWidget(m_tab_bar_bg);

    updateTabBarGeometry();

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
    for (auto action : group->actions()) {
        auto button = qobject_cast<QToolButton *>(previewButtons->widgetForAction(action));
        button->setFixedSize(20, 20);
        button->setIconSize(QSize(16, 16));
    }
    t->addWidget(previewButtons);

    //trash quick operate buttons
    QHBoxLayout *trash = new QHBoxLayout(this);
    trash->setContentsMargins(10, 5, 10, 0);
    QToolBar *trashButtons = new QToolBar(this);
    m_trash_bar = trashButtons;
    //trashButtons->setFloatable(true);
    QPushButton *closeButton = new QPushButton(QIcon::fromTheme("tab-close"), "");
    closeButton->setFixedHeight(20);
    closeButton->setFixedWidth(20);
    m_close_tab = closeButton;
    connect(closeButton, &QPushButton::clicked, [=]{
        updateTrashBarVisible();
    });

    QLabel *Label = new QLabel(tr("Trash"), trashButtons);
    Label->setFixedHeight(TRASH_BUTTON_HEIGHT);
    Label->setFixedWidth(TRASH_BUTTON_WIDTH);
    m_trash_label = Label;
    QPushButton *clearAll = new QPushButton(tr("Clear"), trashButtons);
    clearAll->setFixedWidth(TRASH_BUTTON_WIDTH);
    clearAll->setFixedHeight(TRASH_BUTTON_HEIGHT);
    m_clear_button = clearAll;
    QPushButton *recover = new QPushButton(tr("Recover"), trashButtons);
    recover->setFixedWidth(TRASH_BUTTON_WIDTH);
    recover->setFixedHeight(TRASH_BUTTON_HEIGHT);
    m_recover_button = recover;
    trash->addWidget(closeButton, Qt::AlignRight);
    trash->addSpacing(10);
    trash->addWidget(Label, Qt::AlignRight);
    trash->addWidget(trashButtons);
    trash->addWidget(recover, Qt::AlignLeft);
    trash->addSpacing(10);
    trash->addWidget(clearAll, Qt::AlignLeft);
    updateTrashBarVisible();

    connect(clearAll, &QPushButton::clicked, this, [=]()
    {
        Q_EMIT this->clearTrash();
    });

    connect(recover, &QPushButton::clicked, this, [=]()
    {
        Q_EMIT this->recoverFromTrash();
    });

    QWidget *w = new QWidget();
    w->setAttribute(Qt::WA_TranslucentBackground);
    auto vbox = new QVBoxLayout();
    vbox->setSpacing(0);
    vbox->setContentsMargins(0, 0, 0, 0);
    vbox->addLayout(t);
    vbox->addLayout(trash);
    QSplitter *s = new QSplitter(this);
    s->setChildrenCollapsible(false);
    s->setContentsMargins(0, 0, 0, 0);
    s->setHandleWidth(1);
    s->setStretchFactor(0, 1);
    s->addWidget(m_stack);
    s->addWidget(m_preview_page_container);
    m_preview_page_container->hide();
    vbox->addWidget(s);
    w->setLayout(vbox);
    setCentralWidget(w);

    //bind preview page
    connect(this, &TabWidget::activePageSelectionChanged, this, [=](){
        updatePreviewPage();
        Q_EMIT this->currentSelectionChanged();
    });

    connect(this, &TabWidget::activePageChanged, this, [=](){
        QTimer::singleShot(100, this, [=](){
            this->updatePreviewPage();
        });
    });
}

void TabWidget::updateTrashBarVisible(const QString &uri)
{
    bool visible = false;
    if (uri == "trash:///")
    {
       visible = true;
    }
    m_trash_bar->setVisible(visible);
    m_trash_label->setVisible(visible);
    m_close_tab->setVisible(visible);
    m_clear_button->setVisible(visible);
    m_recover_button->setVisible(visible);
}

Peony::DirectoryViewContainer *TabWidget::currentPage()
{
    return qobject_cast<Peony::DirectoryViewContainer *>(m_stack->currentWidget());
}

const QString TabWidget::getCurrentUri()
{
    return currentPage()->getCurrentUri();
}

const QStringList TabWidget::getCurrentSelections()
{
    return currentPage()->getCurrentSelections();
}

const QStringList TabWidget::getAllFileUris()
{
    return currentPage()->getAllFileUris();
}

const QStringList TabWidget::getBackList()
{
    return currentPage()->getBackList();
}

const QStringList TabWidget::getForwardList()
{
    return currentPage()->getForwardList();
}

bool TabWidget::canGoBack()
{
    return currentPage()->canGoBack();
}

bool TabWidget::canGoForward()
{
    return currentPage()->canGoForward();
}

bool TabWidget::canCdUp()
{
    return currentPage()->canCdUp();
}

int TabWidget::getSortType()
{
    return currentPage()->getSortType();
}

Qt::SortOrder TabWidget::getSortOrder()
{
    return currentPage()->getSortOrder();
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
        m_preview_page_container->removeWidget(m_preview_page_container->widget(0));
        m_preview_page->closePreviewPage();
    }

    m_preview_page = previewPage;

    if (m_preview_page) {
        previewPageWidget->setParent(m_preview_page_container);
        m_preview_page_container->addWidget(previewPageWidget);
        updatePreviewPage();
    }

    m_preview_page_container->blockSignals(!visible);
    m_preview_page_container->setVisible(visible);
}

void TabWidget::addPage(const QString &uri, bool jumpTo)
{
    QCursor c;
    c.setShape(Qt::WaitCursor);
    this->setCursor(c);

    m_tab_bar->addPage(uri, jumpTo);
    auto viewContainer = new Peony::DirectoryViewContainer(m_stack);
    viewContainer->setSortType(Peony::FileItemModel::FileName);
    viewContainer->setSortOrder(Qt::AscendingOrder);

    m_stack->addWidget(viewContainer);
    viewContainer->goToUri(uri, false, true);
    if (jumpTo) {
        m_stack->setCurrentWidget(viewContainer);
    }

    bindContainerSignal(viewContainer);
    updateTrashBarVisible(uri);
}

void TabWidget::goToUri(const QString &uri, bool addHistory, bool forceUpdate)
{
    currentPage()->goToUri(uri, addHistory, forceUpdate);
    m_tab_bar->updateLocation(m_tab_bar->currentIndex(), uri);
    updateTrashBarVisible(uri);
}

void TabWidget::updateTabPageTitle()
{
   m_tab_bar->updateLocation(m_tab_bar->currentIndex(), getCurrentUri());
}

void TabWidget::switchViewType(const QString &viewId)
{
    currentPage()->switchViewType(viewId);
}

void TabWidget::goBack()
{
    currentPage()->goBack();
}

void TabWidget::goForward()
{
    currentPage()->goForward();
}

void TabWidget::cdUp()
{
    currentPage()->cdUp();
}

void TabWidget::refresh()
{
    currentPage()->refresh();
}

void TabWidget::stopLoading()
{
    currentPage()->stopLoading();
}

void TabWidget::tryJump(int index)
{
    currentPage()->tryJump(index);
}

void TabWidget::clearHistory()
{
    currentPage()->clearHistory();
}

void TabWidget::setSortType(int type)
{
    currentPage()->setSortType(Peony::FileItemModel::ColumnType(type));
}

void TabWidget::setSortOrder(Qt::SortOrder order)
{
    currentPage()->setSortOrder(order);
}

void TabWidget::setSortFilter(int FileTypeIndex, int FileMTimeIndex, int FileSizeIndex)
{
    currentPage()->setSortFilter(FileTypeIndex, FileMTimeIndex, FileSizeIndex);
}

void TabWidget::setShowHidden(bool showHidden)
{
    currentPage()->setShowHidden(showHidden);
}

void TabWidget::setUseDefaultNameSortOrder(bool use)
{
    currentPage()->setUseDefaultNameSortOrder(use);
}

void TabWidget::setSortFolderFirst(bool folderFirst)
{
    currentPage()->setSortFolderFirst(folderFirst);
}

void TabWidget::setCurrentSelections(const QStringList &uris)
{
    currentPage()->getView()->setSelections(uris);
}

void TabWidget::editUri(const QString &uri)
{
    currentPage()->getView()->editUri(uri);
}

void TabWidget::editUris(const QStringList &uris)
{
    currentPage()->getView()->editUris(uris);
}

void TabWidget::onViewDoubleClicked(const QString &uri)
{
    qDebug()<<"double clicked"<<uri;
    auto info = Peony::FileInfo::fromUri(uri, false);
    if (info->uri().startsWith("trash://")) {
        auto w = new Peony::PropertiesWindow(QStringList()<<uri);
        w->show();
        return;
    }
    if (info->isDir() || info->isVolume() || info->isVirtual()) {
        Q_EMIT this->updateWindowLocationRequest(uri, true);
    } else {
        Peony::FileLaunchManager::openAsync(uri);
    }
}

void TabWidget::changeCurrentIndex(int index)
{
    m_tab_bar->setCurrentIndex(index);
    m_stack->setCurrentIndex(index);
    Q_EMIT currentIndexChanged(index);
    Q_EMIT activePageChanged();
}

int TabWidget::count()
{
    return m_stack->count();
}

int TabWidget::currentIndex()
{
    return m_stack->currentIndex();
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
    auto widget = m_stack->widget(index);
    m_stack->removeWidget(widget);
    widget->deleteLater();
    if (m_stack->count() > 0)
        Q_EMIT activePageChanged();
}

void TabWidget::bindContainerSignal(Peony::DirectoryViewContainer *container)
{
    connect(container, &Peony::DirectoryViewContainer::updateWindowLocationRequest, this, &TabWidget::updateWindowLocationRequest);
    connect(container, &Peony::DirectoryViewContainer::directoryChanged, this, &TabWidget::activePageLocationChanged);
    connect(container, &Peony::DirectoryViewContainer::selectionChanged, this, &TabWidget::activePageSelectionChanged);
    connect(container, &Peony::DirectoryViewContainer::viewTypeChanged, this, &TabWidget::activePageViewTypeChanged);
    connect(container, &Peony::DirectoryViewContainer::viewDoubleClicked, this, &TabWidget::onViewDoubleClicked);
    connect(container, &Peony::DirectoryViewContainer::menuRequest, this, &TabWidget::menuRequest);
}

void TabWidget::updatePreviewPage()
{
    if (!m_preview_page)
        return;
    auto selection = getCurrentSelections();
    m_preview_page->cancel();
    if (selection.isEmpty())
        return ;
    m_preview_page->prepare(selection.first());
    m_preview_page->startPreview();
}

void TabWidget::resizeEvent(QResizeEvent *e)
{
    QMainWindow::resizeEvent(e);
    updateTabBarGeometry();
}

void TabWidget::updateTabBarGeometry()
{
    m_tab_bar->setGeometry(0, 4, m_tab_bar_bg->width(), m_tab_bar->height());
    m_tab_bar_bg->setFixedHeight(m_tab_bar->height());
    m_tab_bar->raise();
}

PreviewPageContainer::PreviewPageContainer(QWidget *parent) : QStackedWidget(parent)
{

}

PreviewPageButtonGroups::PreviewPageButtonGroups(QWidget *parent) : QButtonGroup(parent)
{
    setExclusive(true);
}
