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
#include "search-vfs-uri-parser.h"
#include "properties-window.h"
#include "file-enumerator.h"

#include <QStackedWidget>
#include <QToolButton>
#include <QHBoxLayout>
#include <QDockWidget>
#include <QToolBar>
#include <QSplitter>
#include <QStringListModel>
#include <QFileDialog>

#include <QAction>

#include <QTimer>

#include "directory-view-container.h"
#include "file-utils.h"
#include "peony-main-window-style.h"

#include "directory-view-factory-manager.h"
#include "global-settings.h"
#include "main-window.h"
#include "volume-manager.h"

#include "file-info-job.h"

#include <QApplication>
#include <QStandardPaths>

#include <QMessageBox>

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

    //status bar
    m_status_bar = new TabStatusBar(this, this);
    connect(this, &TabWidget::zoomRequest, m_status_bar, &TabStatusBar::onZoomRequest);
    connect(m_status_bar, &TabStatusBar::zoomLevelChangedRequest, this, &TabWidget::handleZoomLevel);
    //setStatusBar(m_status_bar);

    connect(m_buttons, &PreviewPageButtonGroups::previewPageButtonTrigger, [=](bool trigger, const QString &id) {
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
    connect(m_tab_bar, &NavigationTabBar::locationUpdated, this, &TabWidget::updateSearchPathButton);

    connect(m_tab_bar, &NavigationTabBar::closeWindowRequest, this, &TabWidget::closeWindowRequest);

    QHBoxLayout *t = new QHBoxLayout();
    QActionGroup *group = new QActionGroup(this);
    group->setExclusive(true);
    m_tab_bar_bg = new QWidget(this);
    m_tab_bar_bg->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    QToolBar *previewButtons = new QToolBar(this);
    m_tool_bar = previewButtons;
    //previewButtons->setFixedHeight(m_tab_bar->height());
    t->setContentsMargins(0, 0, 5, 0);
    t->addWidget(m_tab_bar_bg);

    auto spacer = new QWidget(this);
    spacer->setFixedWidth(qApp->style()->pixelMetric(QStyle::PM_ToolBarItemSpacing) * 2 + 36);
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    t->addWidget(spacer);
    auto addPageButton = new QToolButton(this);
    addPageButton->setIcon(QIcon::fromTheme("list-add-symbolic"));
    spacer->setVisible(false);
    addPageButton->setVisible(false);
    addPageButton->setFixedSize(m_tab_bar->height() + 2, m_tab_bar->height() + 2);
    addPageButton->setProperty("useIconHighlightEffect", true);
    addPageButton->setProperty("iconHighlightEffectMode", 1);
    addPageButton->setProperty("fillIconSymbolicColor", true);

    connect(m_tab_bar, &NavigationTabBar::floatButtonVisibleChanged, addPageButton, [=](bool visible, int yoffset){
        spacer->setVisible(!visible);
        addPageButton->setVisible(!visible);
        addPageButton->raise();
        if (!visible)
            addPageButton->move(m_tab_bar->width() + qApp->style()->pixelMetric(QStyle::PM_ToolBarItemSpacing), yoffset + 4);
    });

    connect(addPageButton, &QPushButton::clicked, this, [=](){
        m_tab_bar->addPageRequest(m_tab_bar->tabData(m_tab_bar->currentIndex()).toString(), true);
    });

    updateTabBarGeometry();

    auto manager = Peony::PreviewPageFactoryManager::getInstance();
    auto pluginNames = manager->getPluginNames();
    for (auto name : pluginNames) {
        auto factory = manager->getPlugin(name);
        auto action = group->addAction(factory->icon(), factory->name());
        action->setCheckable(true);
        connect(action, &QAction::triggered, [=](/*bool checked*/) {
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
//        auto button = qobject_cast<QToolButton *>(previewButtons->widgetForAction(action));
//        button->setFixedSize(26, 26);
//        button->setIconSize(QSize(16, 16));
//        button->setProperty("useIconHighlightEffect", true);
//        button->setProperty("iconHighlightEffectMode", 1);
//        button->setProperty("fillIconSymbolicColor", true);

        //use theme buttons
        auto button = new QPushButton(this);
        button->setFixedSize(QSize(26, 26));
        button->setIconSize(QSize(16, 16));
        button->setFlat(true);
        button->setProperty("isWindowButton", 1);
        button->setProperty("useIconHighlightEffect", 2);
        button->setProperty("isIcon", true);
    }
    t->addWidget(previewButtons);

    //trash quick operate buttons
    QHBoxLayout *trash = new QHBoxLayout();
    m_trash_bar_layout = trash;
    QToolBar *trashButtons = new QToolBar(this);
    m_trash_bar = trashButtons;

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
    //hide trash button to fix bug 31322, according to designer advice
    m_recover_button->hide();

    //trash->addSpacing(10);
    trash->addWidget(Label, Qt::AlignLeft);
    trash->setContentsMargins(10, 0, 10, 0);
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

    //advance search ui init
    initAdvanceSearch();

    QWidget *w = new QWidget();
    w->setAttribute(Qt::WA_TranslucentBackground);
    auto vbox = new QVBoxLayout();
    m_top_layout = vbox;
    vbox->setSpacing(0);
    vbox->setContentsMargins(0, 0, 0, 0);
    vbox->addLayout(t);
    vbox->addLayout(trash);
    vbox->addLayout(m_search_bar_layout);
    QSplitter *s = new QSplitter(this);
    s->setChildrenCollapsible(false);
    s->setContentsMargins(0, 0, 0, 0);
    s->setHandleWidth(1);

    s->addWidget(m_stack);
    m_stack->installEventFilter(this);
    //s->addWidget(m_preview_page_container);
    m_preview_page_container->hide();

    s->setStretchFactor(0, 3);
    s->setStretchFactor(1, 2);

    vbox->addWidget(s);
    w->setLayout(vbox);
    setCentralWidget(w);

    //bind preview page
    connect(this, &TabWidget::activePageSelectionChanged, this, [=]() {
        updatePreviewPage();
        m_status_bar->update();
        Q_EMIT this->currentSelectionChanged();
    });

    connect(this, &TabWidget::activePageChanged, this, [=]() {
        QTimer::singleShot(100, this, [=]() {
            m_status_bar->update();
            this->updatePreviewPage();
        });
    });

    connect(this, &TabWidget::activePageLocationChanged, m_status_bar, [=]() {
        if (m_first_add_page) {
            previewButtons->setEnabled(true);
            s->addWidget(m_preview_page_container);
            m_first_add_page = false;
        }
        m_status_bar->update();
    });

    previewButtons->setEnabled(false);
}

void TabWidget::initAdvanceSearch()
{
    //advance search bar
    QHBoxLayout *search = new QHBoxLayout();
    m_search_bar_layout = search;
    QToolBar *searchButtons = new QToolBar(this);
    m_search_bar = searchButtons;
    QPushButton *closeButton = new QPushButton(QIcon::fromTheme("window-close-symbolic"), "", searchButtons);
    m_search_close = closeButton;
    closeButton->setFixedHeight(20);
    closeButton->setFixedWidth(20);
    closeButton->setToolTip(tr("Close advance search."));

    connect(closeButton, &QPushButton::clicked, [=]()
    {
        updateSearchBar(false);
        Q_EMIT this->closeSearch();
    });

    QLabel *title = new QLabel(tr("Search"), searchButtons);
    m_search_title = title;
    title->setFixedWidth(TRASH_BUTTON_WIDTH);
    title->setFixedHeight(TRASH_BUTTON_HEIGHT);

    QPushButton *tabButton = new QPushButton(searchButtons);
    m_search_path = tabButton;
    tabButton->setFixedHeight(TRASH_BUTTON_HEIGHT);
    tabButton->setFixedWidth(TRASH_BUTTON_WIDTH * 2);
    tabButton->setToolTip(tr("Choose other path to search."));
    connect(tabButton, &QPushButton::clicked, this, &TabWidget::browsePath);

    QPushButton *childButton = new QPushButton(searchButtons);
    m_search_child = childButton;
    childButton->setFixedHeight(TRASH_BUTTON_HEIGHT);
    childButton->setFixedWidth(TRASH_BUTTON_HEIGHT);
    //qDebug() << QIcon(":/custom/icons/child-folder").name();
    childButton->setIcon(QIcon(":/custom/icons/child-folder"));
    childButton->setToolTip(tr("Search recursively"));
    connect(childButton, &QPushButton::clicked, this, &TabWidget::searchChildUpdate);
    //set default select recursive
    m_search_child_flag = true;
    Q_EMIT this->searchRecursiveChanged(m_search_child_flag);
    m_search_child->setCheckable(m_search_child_flag);
    m_search_child->setChecked(m_search_child_flag);
    m_search_child->setDown(m_search_child_flag);;

//    QPushButton *moreButton = new QPushButton(tr("more options"),searchButtons);
//    m_search_more = moreButton;
//    moreButton->setFixedHeight(TRASH_BUTTON_HEIGHT);
//    moreButton->setFixedWidth(TRASH_BUTTON_WIDTH *2);
//    moreButton->setToolTip(tr("Show/hide advance search"));

//    connect(moreButton, &QPushButton::clicked, this, &TabWidget::updateSearchList);

    search->addWidget(closeButton, Qt::AlignLeft);
    search->addSpacing(10);
    search->addWidget(title, Qt::AlignLeft);
    search->addSpacing(10);
    search->addWidget(tabButton, Qt::AlignLeft);
    search->addSpacing(10);
    search->addWidget(childButton, Qt::AlignLeft);
//    search->addSpacing(10);
//    search->addWidget(moreButton, Qt::AlignLeft);
    search->addSpacing(10);
    search->addWidget(searchButtons);
    search->setContentsMargins(10, 0, 10, 0);
    searchButtons->setVisible(false);
    tabButton->setVisible(false);
    closeButton->setVisible(false);
    title->setVisible(false);
    childButton->setVisible(false);
//    moreButton->setVisible(false);
}

//search conditions changed, update filter
void TabWidget::searchUpdate()
{
    qDebug() <<"searchUpdate:" <<m_search_child_flag;
    auto currentUri = getCurrentUri();
    if (! currentUri.startsWith("search:///"))
    {
        qDebug() << "searchUpdate is not in search path";
        return;
    }

    QString targetUri = currentUri;
    if (m_search_child_flag)
    {
        targetUri = currentUri.replace("&recursive=0", "&recursive=1");
    }
    else
        targetUri = currentUri.replace("&recursive=1", "&recursive=0");

    qDebug() <<"searchUpdate targetUri:" <<targetUri;
    goToUri(targetUri, false, true);
}

void TabWidget::searchChildUpdate()
{
    m_search_child_flag = ! m_search_child_flag;
    m_search_child->setCheckable(m_search_child_flag);
    m_search_child->setChecked(m_search_child_flag);
    m_search_child->setDown(m_search_child_flag);
    searchUpdate();

    Q_EMIT this->searchRecursiveChanged(m_search_child_flag);
}

void TabWidget::browsePath()
{
    QString target_path = QFileDialog::getExistingDirectory(this, tr("Select path"), getCurrentUri(), QFileDialog::ShowDirsOnly);
    qDebug()<<"browsePath Opened:"<<target_path;
    //add root prefix
    if (! target_path.contains("file://") && target_path != "")
        target_path = "file://" + target_path;

    if (target_path != "" && target_path != getCurrentUri())
    {
        updateSearchPathButton(target_path);
        Q_EMIT this->updateSearch(target_path);
    }
}

void TabWidget::addNewConditionBar()
{
    QHBoxLayout *layout = new QHBoxLayout();
    m_layout_list.append(layout);

    QToolBar *optionBar = new QToolBar(this);
    m_search_bar_list.append(optionBar);

    QComboBox *conditionCombox = new QComboBox(optionBar);
    m_conditions_list.append(conditionCombox);
    conditionCombox->setFixedHeight(TRASH_BUTTON_HEIGHT);
    conditionCombox->setFixedWidth(TRASH_BUTTON_WIDTH *2);
    auto conditionModel = new QStringListModel(optionBar);
    conditionModel->setStringList(m_option_list);
    conditionCombox->setModel(conditionModel);
    auto index = m_search_bar_count;
    if (index > m_option_list.count()-1)
        index = m_option_list.count()-1;
    conditionCombox->setCurrentIndex(index);

    //qDebug() << "addNewConditionBar:" <<index;

    QLabel *linkLabel = new QLabel(tr("is"));
    m_link_label_list.append(linkLabel);
    linkLabel->setFixedHeight(TRASH_BUTTON_HEIGHT);
    linkLabel->setFixedWidth(TRASH_BUTTON_HEIGHT);

    QComboBox *classifyCombox = new QComboBox(optionBar);
    m_classify_list.append(classifyCombox);
    classifyCombox->setFixedHeight(TRASH_BUTTON_HEIGHT);
    classifyCombox->setFixedWidth(TRASH_BUTTON_WIDTH *2);
    auto classifyModel = new QStringListModel(optionBar);
    auto list = getCurrentClassify(index);
    classifyModel->setStringList(list);
    classifyCombox->setModel(classifyModel);

    QLineEdit *inputBox = new QLineEdit(optionBar);
    m_input_list.append(inputBox);
    inputBox->setFixedHeight(TRASH_BUTTON_HEIGHT);
    inputBox->setFixedWidth(TRASH_BUTTON_WIDTH *4);
    inputBox->setPlaceholderText(tr("Please input key words..."));
    inputBox->setText("");

    QPushButton *addButton = new QPushButton(QIcon::fromTheme("add"), "", optionBar);
    m_add_button_list.append(addButton);
    addButton->setFixedHeight(20);
    addButton->setFixedWidth(20);
    connect(addButton, &QPushButton::clicked, this, &TabWidget::addNewConditionBar);

    QPushButton *removeButton = new QPushButton(QIcon::fromTheme("remove"), "", optionBar);
    m_remove_button_list.append(removeButton);
    removeButton->setFixedHeight(20);
    removeButton->setFixedWidth(20);
    //mapper for button clicked parse index
    auto signalMapper = new QSignalMapper(this);
    connect(removeButton, SIGNAL(clicked()), signalMapper, SLOT(map()));
    signalMapper->setMapping(removeButton, index);
    connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(removeConditionBar(int)));
    m_remove_mapper_list.append(signalMapper);

    layout->addWidget(addButton, Qt::AlignRight);
    layout->addSpacing(10);
    layout->addWidget(removeButton, Qt::AlignRight);
    layout->addSpacing(10);
    layout->addSpacing(TRASH_BUTTON_WIDTH - 20);
    layout->addWidget(conditionCombox, Qt::AlignLeft);
    layout->addSpacing(10);
    layout->addWidget(linkLabel, Qt::AlignLeft);
    layout->addSpacing(10);
    layout->addWidget(classifyCombox, Qt::AlignLeft);
    layout->addWidget(inputBox, Qt::AlignLeft);
    layout->addWidget(optionBar);
    layout->setContentsMargins(10, 0, 10, 5);

    if (index%4 >= 3)
    {
        classifyCombox->hide();
        linkLabel->setText(tr("contains"));
        //adjust label width to language
        QLocale locale;
        if (locale.language() == QLocale::Chinese)
            linkLabel->setFixedWidth(TRASH_BUTTON_HEIGHT);
        else
            linkLabel->setFixedWidth(TRASH_BUTTON_WIDTH);
    }
    else
    {
       inputBox->hide();
    }


    connect(conditionCombox, &QComboBox::currentTextChanged, [=]()
    {
        auto cur = conditionCombox->currentIndex();
        if (cur%4 >= 3)
        {
            classifyCombox->hide();
            inputBox->show();
            linkLabel->setText(tr("contains"));
            //adjust label width to language
            QLocale locale;
            if (locale.language() == QLocale::Chinese)
                linkLabel->setFixedWidth(TRASH_BUTTON_HEIGHT);
            else
                linkLabel->setFixedWidth(TRASH_BUTTON_WIDTH);
        }
        else
        {
            classifyCombox->show();
            inputBox->hide();
            linkLabel->setFixedWidth(TRASH_BUTTON_HEIGHT);
            linkLabel->setText(tr("is"));
            auto classifyList = getCurrentClassify(cur);
            classifyModel->setStringList(classifyList);
            classifyCombox->setModel(classifyModel);
            classifyCombox->setCurrentIndex(0);
        }
    });

    connect(classifyCombox, &QComboBox::currentTextChanged, this, &TabWidget::updateAdvanceConditions);
    connect(inputBox, &QLineEdit::textChanged, this, &TabWidget::updateAdvanceConditions);

    m_top_layout->insertLayout(m_top_layout->count()-1, layout);
    m_search_bar_count++;
    updateAdvanceConditions();
    updateButtons();
}

void TabWidget::removeConditionBar(int index)
{
    //disconnect signals after index search bars
    for(int cur=0; cur<m_layout_list.count(); cur++)
    {
        disconnect(m_add_button_list[cur], &QPushButton::clicked, this, &TabWidget::addNewConditionBar);
        disconnect(m_remove_button_list[cur], SIGNAL(clicked()), m_remove_mapper_list[cur], SLOT(map()));
        disconnect(m_remove_mapper_list[cur], SIGNAL(mapped(int)), this, SLOT(removeConditionBar(int)));
    }

    //qDebug() << "removeConditionBar:" <<index <<m_conditions_list.count();
    m_layout_list[index]->deleteLater();
    m_conditions_list[index]->deleteLater();
    m_link_label_list[index]->deleteLater();
    m_classify_list[index]->deleteLater();
    m_input_list[index]->deleteLater();
    m_search_bar_list[index]->deleteLater();
    m_add_button_list[index]->deleteLater();
    m_remove_button_list[index]->deleteLater();
    m_remove_mapper_list[index]->deleteLater();

    m_layout_list.removeAt(index);
    m_conditions_list.removeAt(index);
    //qDebug() << "removeConditionBar:"<<m_conditions_list.count();
    m_link_label_list.removeAt(index);
    m_classify_list.removeAt(index);
    m_input_list.removeAt(index);
    m_search_bar_list.removeAt(index);
    m_add_button_list.removeAt(index);
    m_remove_button_list.removeAt(index);
    m_remove_mapper_list.removeAt(index);

    //reconnect signals after index search bars
    for(int cur=0; cur<m_layout_list.count(); cur++)
    {
        connect(m_add_button_list[cur], &QPushButton::clicked, this, &TabWidget::addNewConditionBar);
        connect(m_remove_button_list[cur], SIGNAL(clicked()), m_remove_mapper_list[cur], SLOT(map()));
        m_remove_mapper_list[cur]->setMapping(m_remove_button_list[cur], cur);
        connect(m_remove_mapper_list[cur], SIGNAL(mapped(int)), this, SLOT(removeConditionBar(int)));
    }
    m_search_bar_count--;
    updateAdvanceConditions();
    updateButtons();
}

QStringList TabWidget::getCurrentClassify(int rowCount)
{
    QStringList currentList;
    currentList.clear();

    switch (rowCount%4) {
    case 0:
        return m_file_type_list;
    case 1:
        return m_file_size_list;
    case 2:
        return m_file_mtime_list;
    default:
        break;
    }

    return currentList;
}

void TabWidget::updateTrashBarVisible(const QString &uri)
{
    bool visible = false;
    m_trash_bar_layout->setContentsMargins(10, 0, 10, 0);
    if (uri.indexOf("trash:///") >= 0)
    {
        visible = true;
        m_trash_bar_layout->setContentsMargins(10, 5, 10, 5);
    }

    m_trash_bar->setVisible(visible);
    m_trash_label->setVisible(visible);
    m_clear_button->setVisible(visible);
    //m_recover_button->setVisible(visible);

    if (uri.startsWith("trash://") || uri.startsWith("recent://"))
        m_tool_bar->setVisible(false);
    else
        m_tool_bar->setVisible(true);
}

void TabWidget::handleZoomLevel(int zoomLevel)
{
    currentPage()->getView()->clearIndexWidget();

    int currentViewZoomLevel = currentPage()->getView()->currentZoomLevel();
    int currentViewMimZoomLevel = currentPage()->getView()->minimumZoomLevel();
    int currentViewMaxZoomLevel = currentPage()->getView()->maximumZoomLevel();
    if (zoomLevel == currentViewZoomLevel) {
        return;
    }

    // save default zoom level
    Peony::GlobalSettings::getInstance()->setValue(DEFAULT_VIEW_ZOOM_LEVEL, zoomLevel);

    if (zoomLevel <= currentViewMaxZoomLevel && zoomLevel >= currentViewMimZoomLevel) {
        currentPage()->getView()->setCurrentZoomLevel(zoomLevel);
    } else {
        //check which view to switch.
        auto directoryViewManager = Peony::DirectoryViewFactoryManager2::getInstance();
        auto viewId = directoryViewManager->getDefaultViewId(zoomLevel, getCurrentUri());
        switchViewType(viewId);
        currentPage()->getView()->setCurrentZoomLevel(zoomLevel);
    }
}

void TabWidget::updateSearchBar(bool showSearch)
{
    qDebug() << "updateSearchBar:" <<showSearch;
    m_show_search_bar = showSearch;
    if (showSearch)
    {
        //default add one bar
        updateSearchList();
        m_search_path->show();
        m_search_close->show();
        m_search_title->show();
        m_search_bar->show();
        m_search_child->show();
        //m_search_more->show();
        m_search_bar_layout->setContentsMargins(10, 5, 10, 5);
        //m_search_more->setIcon(QIcon::fromTheme("go-down"));
        updateSearchPathButton();
    }
    else
    {
        m_search_path->hide();
        m_search_close->hide();
        m_search_title->hide();
        m_search_bar->hide();
        m_search_child->hide();
        //m_search_more->hide();
        m_search_bar_layout->setContentsMargins(10, 0, 10, 0);
    }

    if (m_search_bar_count >0)
        updateSearchList();

    if (! showSearch)
    {
        //exit advance search, clear search conditions
        clearConditions();
        updateFilter();
    }
}

void TabWidget::updateButtons()
{
    //only one condition, set disabled
    if (m_search_bar_count ==1)
        m_remove_button_list[0]->setDisabled(true);
    else
        m_remove_button_list[0]->setDisabled(false);

    //limit total number to 10
    if (m_search_bar_count >= 10)
    {
        for(int i=0;i<m_search_bar_count;i++)
        {
            m_add_button_list[i]->setDisabled(true);
        }
    }
    else
    {
        for(int i=0;i<m_search_bar_count;i++)
        {
            m_add_button_list[i]->setDisabled(false);
        }
    }
}

void TabWidget::updateSearchPathButton(const QString &uri)
{
    //search path not update
    //qDebug() << "updateSearchPathButton:" <<uri;
    if (uri.startsWith("search://"))
        return;
    QString curUri = uri;
    if (uri == "")
    {
        curUri = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
        if (! getCurrentUri().isNull())
            curUri = getCurrentUri();
    }
    auto iconName = Peony::FileUtils::getFileIconName(curUri);
    auto displayName = Peony::FileUtils::getFileDisplayName(curUri);
    qDebug() << "iconName:" <<iconName <<displayName<<curUri;
    m_search_path->setIcon(QIcon::fromTheme(iconName));

    //elide text if it is too long
    if (displayName.length() > ELIDE_TEXT_LENGTH)
    {
        int  charWidth = fontMetrics().averageCharWidth();
        displayName = fontMetrics().elidedText(displayName, Qt::ElideRight, ELIDE_TEXT_LENGTH * charWidth);
    }
    m_search_path->setText(displayName);
}

void TabWidget::updateSearchList()
{
    m_show_search_list = !m_show_search_list;
    //if not show search bar, then don't show search list
    qDebug() << "updateSearchList:" <<m_show_search_list <<m_show_search_bar;
    if (m_show_search_bar)
    {
        //m_search_more->setIcon(QIcon::fromTheme("go-up"));
        //first click to show advance serach
        if(m_search_bar_list.count() ==0)
        {
            addNewConditionBar();
            return;
        }

        //already had a list,just set to show
        for(int i=0; i<m_search_bar_list.count(); i++)
        {
            m_conditions_list[i]->show();
            m_link_label_list[i]->show();
            if (m_conditions_list[i]->currentIndex()%4 < 3)
                m_classify_list[i]->show();
            else
                m_input_list[i]->show();
            m_search_bar_list[i]->show();
            m_add_button_list[i]->show();
            m_remove_button_list[i]->show();
            m_layout_list[i]->setContentsMargins(10, 0, 10, 5);
        }
    }
    else
    {
        //hide search list
        //m_search_more->setIcon(QIcon::fromTheme("go-down"));
        for(int i=0; i<m_search_bar_list.count(); i++)
        {
            m_conditions_list[i]->hide();
            m_conditions_list[i]->setCurrentIndex(0);
            m_link_label_list[i]->hide();
            m_classify_list[i]->hide();
            m_classify_list[i]->setCurrentIndex(0);
            m_input_list[i]->hide();
            m_input_list[i]->setText("");
            m_search_bar_list[i]->hide();
            m_add_button_list[i]->hide();
            m_remove_button_list[i]->hide();
            m_layout_list[i]->setContentsMargins(10, 0, 10, 0);
        }
    }
}

Peony::DirectoryViewContainer *TabWidget::currentPage()
{
    return qobject_cast<Peony::DirectoryViewContainer *>(m_stack->currentWidget());
}

const QString TabWidget::getCurrentUri()
{
    if (!currentPage())
        return nullptr;
    return currentPage()->getCurrentUri();
}

const QStringList TabWidget::getCurrentSelections()
{
    if (!currentPage())
        return QStringList();
    return currentPage()->getCurrentSelections();
}

const QStringList TabWidget::getAllFileUris()
{
    if (!currentPage())
        return QStringList();
    return currentPage()->getAllFileUris();
}

const QStringList TabWidget::getBackList()
{
    if (!currentPage())
        return QStringList();
    return currentPage()->getBackList();
}

const QStringList TabWidget::getForwardList()
{
    if (!currentPage())
        return QStringList();
    return currentPage()->getForwardList();
}

bool TabWidget::canGoBack()
{
    if (!currentPage())
        return false;
    return currentPage()->canGoBack();
}

bool TabWidget::canGoForward()
{
    if (!currentPage())
        return false;
    return currentPage()->canGoForward();
}

bool TabWidget::canCdUp()
{
    if (!currentPage())
        return false;
    return currentPage()->canCdUp();
}

int TabWidget::getSortType()
{
    if (!currentPage())
        return 0;
    return currentPage()->getSortType();
}

Qt::SortOrder TabWidget::getSortOrder()
{
    if (!currentPage())
        return Qt::AscendingOrder;
    return currentPage()->getSortOrder();
}

bool TabWidget::eventFilter(QObject *obj, QEvent *e)
{
    if (e->type() == QEvent::Resize) {
        updateStatusBarGeometry();
    }
    return false;
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
    setCursor(QCursor(Qt::WaitCursor));

    auto info = Peony::FileInfo::fromUri(uri);
    auto infoJob = new Peony::FileInfoJob(info);
    infoJob->setAutoDelete();

    connect(infoJob, &Peony::FileInfoJob::queryAsyncFinished, this, [=](){
        auto enumerator = new Peony::FileEnumerator;
        enumerator->setEnumerateDirectory(info.get()->uri());
        connect(enumerator, &Peony::FileEnumerator::prepared, this, [=](const std::shared_ptr<Peony::GErrorWrapper> &err = nullptr, const QString &t = nullptr, bool critical = false){
            if (critical) {
                QMessageBox::critical(0, 0, err.get()->message());
                setCursor(QCursor(Qt::ArrowCursor));
                return;
            }
            auto viewContainer = new Peony::DirectoryViewContainer(m_stack);
            bool hasCurrentPage = currentPage();
            bool hasView = false;
            if (hasCurrentPage)
                hasView = currentPage()->getView();
            int zoomLevel = -1;

            if (hasCurrentPage) {
                // perfer to use current page view type
                auto internalViews = Peony::DirectoryViewFactoryManager2::getInstance()->internalViews();
                //fix continuously click add button quickly crash issue, bug #41425
                if (hasView && internalViews.contains(currentPage()->getView()->viewId()))
                    viewContainer->switchViewType(currentPage()->getView()->viewId());

                if (hasView && hasCurrentPage) {
                    hasCurrentPage = true;
                    zoomLevel = currentPage()->getView()->currentZoomLevel();
                }
            } else {
                viewContainer->switchViewType(Peony::GlobalSettings::getInstance()->getValue(DEFAULT_VIEW_ID).toString());
            }
            m_stack->addWidget(viewContainer);
            if (jumpTo) {
                m_stack->setCurrentWidget(viewContainer);
            }

            //auto viewContainer = new Peony::DirectoryViewContainer(m_stack);
            auto settings = Peony::GlobalSettings::getInstance();
            auto sortType = settings->isExist(SORT_COLUMN)? settings->getValue(SORT_COLUMN).toInt(): 0;
            auto sortOrder = settings->isExist(SORT_ORDER)? settings->getValue(SORT_ORDER).toInt(): 0;
            viewContainer->setSortType(Peony::FileItemModel::ColumnType(sortType));
            viewContainer->setSortOrder(Qt::SortOrder(sortOrder));

            //process open symbolic link
            auto realUri = uri;
            if (info->isSymbolLink() && info->symlinkTarget().length() >0 && uri.startsWith("file://"))
                realUri = "file://" + info->symlinkTarget();

            //m_stack->addWidget(viewContainer);
            viewContainer->goToUri(realUri, false, true);

            bindContainerSignal(viewContainer);
            updateTrashBarVisible(uri);

            if (zoomLevel > 0)
                viewContainer->getView()->setCurrentZoomLevel(zoomLevel);
            else
                viewContainer->getView()->setCurrentZoomLevel(Peony::GlobalSettings::getInstance()->getValue(DEFAULT_VIEW_ZOOM_LEVEL).toInt());

            m_tab_bar->addPage(realUri, jumpTo);
            updateTabBarGeometry();
        });
        enumerator->prepare();
    });

    infoJob->queryAsync();
}

void TabWidget::goToUri(const QString &uri, bool addHistory, bool forceUpdate)
{
    qDebug() << "goToUri:" << uri;
    currentPage()->goToUri(uri, addHistory, forceUpdate);
    m_tab_bar->updateLocation(m_tab_bar->currentIndex(), uri);
    updateTrashBarVisible(uri);
    updatePreviewPage();
}

void TabWidget::updateTabPageTitle()
{
    qDebug() << "updateTabPageTitle:" <<getCurrentUri();
    //fix error for glib2 signal: G_FILE_MONITOR_EVENT_DELETED
    if("trash:///" == getCurrentUri()){
        Peony::VolumeManager* vm = Peony::VolumeManager::getInstance();
        connect(vm,&Peony::VolumeManager::volumeRemoved,this,[=](const std::shared_ptr<Peony::Volume> &volume){
            refresh();
        });
        connect(vm,&Peony::VolumeManager::volumeAdded,this,[=](const std::shared_ptr<Peony::Volume> &volume){
            refresh();
        });
    }

    m_tab_bar->updateLocation(m_tab_bar->currentIndex(), getCurrentUri());
    updateTrashBarVisible(getCurrentUri());
}

void TabWidget::switchViewType(const QString &viewId)
{
    if (currentPage()->getView()->viewId() == viewId)
        return;

    currentPage()->switchViewType(viewId);

    // change default view id
    auto factoryManager = Peony::DirectoryViewFactoryManager2::getInstance();
    auto internalViews = factoryManager->internalViews();
    if (internalViews.contains(viewId))
        Peony::GlobalSettings::getInstance()->setValue(DEFAULT_VIEW_ID, viewId);

    bool supportZoom = this->currentPage()->getView()->supportZoom();
    this->m_status_bar->m_slider->setEnabled(this->currentPage()->getView()->supportZoom());
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

void TabWidget::addFilterCondition(int option, int classify, bool updateNow)
{
    currentPage()->addFilterCondition(option, classify, updateNow);
}

void TabWidget::removeFilterCondition(int option, int classify, bool updateNow)
{
    currentPage()->removeFilterCondition(option, classify, updateNow);
}

void TabWidget::clearConditions()
{
    currentPage()->clearConditions();
}

void TabWidget::updateFilter()
{
    currentPage()->updateFilter();
}

void TabWidget::updateAdvanceConditions()
{
    clearConditions();

    //get key list for proxy-filter
    //input name not show, must be empty
    QStringList keyList;
    for(int i=0; i<m_layout_list.count(); i++)
    {
        QString input = m_input_list[i]->text();
        if(input != "" && ! keyList.contains(input))
        {
            keyList.append(input);
        }
        else
        {
            addFilterCondition(m_conditions_list[i]->currentIndex(), m_classify_list[i]->currentIndex());
        }
    }

    //update file name filter
    for(auto key : keyList)
    {
       currentPage()->addFileNameFilter(key);
    }

    updateFilter();
}

void TabWidget::setCurrentSelections(const QStringList &uris)
{
    currentPage()->getView()->setSelections(uris);
    if (uris.count() >0)
        currentPage()->getView()->scrollToSelection(uris.first());
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
    qDebug()<<"tab widget double clicked"<<uri;
    auto info = Peony::FileInfo::fromUri(uri);
    if (info->uri().startsWith("trash://")) {
        auto w = new Peony::PropertiesWindow(QStringList()<<uri);
        w->show();
        return;
    }
    if (info->isDir() || info->isVolume() || info->isVirtual()) {
        Q_EMIT this->updateWindowLocationRequest(uri, true);
    } else {
        Peony::FileLaunchManager::openAsync(uri, false, false);
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
    connect(container, &Peony::DirectoryViewContainer::zoomRequest, this, &TabWidget::zoomRequest);
    connect(container, &Peony::DirectoryViewContainer::setZoomLevelRequest, m_status_bar, &TabStatusBar::updateZoomLevelState);
    connect(container, &Peony::DirectoryViewContainer::updateStatusBarSliderStateRequest, this, [=]() {
        bool enable = currentPage()->getView()->supportZoom();
        m_status_bar->m_slider->setEnabled(enable);
        m_status_bar->m_slider->setVisible(enable);
    });

    connect(container, &Peony::DirectoryViewContainer::updateWindowSelectionRequest, this, [=](const QStringList &uris){
        if (container == currentPage()) {
            Q_EMIT this->updateWindowSelectionRequest(uris);
        }
    });
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
    //updateStatusBarGeometry();
}

void TabWidget::updateTabBarGeometry()
{
    m_tab_bar->setGeometry(0, 4, m_tab_bar_bg->width(), m_tab_bar->height());
    m_tab_bar_bg->setFixedHeight(m_tab_bar->height());
    m_tab_bar->raise();
}

void TabWidget::updateStatusBarGeometry()
{
    auto font = qApp->font();
    QFontMetrics fm(font);
    m_status_bar->setGeometry(0, this->height() - fm.height() - 10, m_stack->width(), fm.height() + 10);
    m_status_bar->raise();
}

const QList<std::shared_ptr<Peony::FileInfo>> TabWidget::getCurrentSelectionFileInfos()
{
    const QStringList uris = getCurrentSelections();
    QList<std::shared_ptr<Peony::FileInfo>> infos;
    for(auto uri : uris) {
        auto info = Peony::FileInfo::fromUri(uri);
        infos<<info;
    }
    return infos;
}

PreviewPageContainer::PreviewPageContainer(QWidget *parent) : QStackedWidget(parent)
{

}

PreviewPageButtonGroups::PreviewPageButtonGroups(QWidget *parent) : QButtonGroup(parent)
{
    setExclusive(true);
}
