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

#include "navigation-side-bar.h"
#include "side-bar-model.h"
#include "side-bar-proxy-filter-sort-model.h"
#include "side-bar-abstract-item.h"
#include "volume-manager.h"
#include "volumeManager.h"
#include "side-bar-menu.h"
#include "side-bar-abstract-item.h"
#include "bookmark-manager.h"
#include "file-info.h"
#include "file-info-job.h"
#include "main-window.h"

#include "global-settings.h"

#include "file-enumerator.h"
#include "gerror-wrapper.h"

#include "file-utils.h"

#include <QHeaderView>
#include <QPushButton>

#include <QVBoxLayout>

#include <QEvent>

#include <QPainter>

#include <QScrollBar>

#include <QKeyEvent>

#include <QUrl>
#include <QDropEvent>
#include <QMimeData>

#include <QTimer>
#include <QMessageBox>

#include <QPainterPath>

#include <QDebug>
#include <QToolTip>

#define NAVIGATION_SIDEBAR_ITEM_BORDER_RADIUS 4

using namespace Peony;

NavigationSideBar::NavigationSideBar(QWidget *parent) : QTreeView(parent)
{
    static NavigationSideBarStyle *global_style = new NavigationSideBarStyle;

    setSortingEnabled(true);

    setIconSize(QSize(16, 16));

    setProperty("useIconHighlightEffect", true);
    //both default and highlight.
    setProperty("iconHighlightEffectMode", 1);

    this->verticalScrollBar()->setProperty("drawScrollBarGroove", false);

    setDragDropMode(QTreeView::DropOnly);

    setProperty("doNotBlur", true);
    viewport()->setProperty("doNotBlur", true);

    auto delegate = new NavigationSideBarItemDelegate(this);
    setItemDelegate(delegate);

    installEventFilter(this);

    setStyleSheet(".NavigationSideBar"
                  "{"
                  "border: 0px solid transparent"
                  "}");

    setStyle(global_style);

    setAttribute(Qt::WA_TranslucentBackground);
    viewport()->setAttribute(Qt::WA_TranslucentBackground);
    header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    header()->setStretchLastSection(false);
    header()->hide();

    setContextMenuPolicy(Qt::CustomContextMenu);

    setExpandsOnDoubleClick(false);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    m_model = new Peony::SideBarModel(this);
    m_proxy_model = new Peony::SideBarProxyFilterSortModel(this);
    m_proxy_model->setSourceModel(m_model);

    this->setModel(m_proxy_model);

    setMouseTracking(true);//追踪鼠标

    VolumeManager *volumeManager = VolumeManager::getInstance();
    connect(volumeManager,&Peony::VolumeManager::volumeAdded,this,[=](const std::shared_ptr<Peony::Volume> &volume){
        m_proxy_model->invalidate();//display DVD device in real time.
    });
    connect(volumeManager,&Peony::VolumeManager::volumeRemoved,this,[=](const std::shared_ptr<Peony::Volume> &volume){
        m_proxy_model->invalidate();//The drive does not display when the DVD device is removed.
        //qDebug() << "volumeRemoved:" <<QToolTip::text();
        //fix abnormal pull out usb device tips not hide issue, link to bug#81190
        if (QToolTip::isVisible()) {
            QToolTip::hideText();
        }
    });
    connect(volumeManager,&Peony::VolumeManager::driveDisconnected,this,[=](const std::shared_ptr<Peony::Drive> &drive){
        m_proxy_model->invalidate();//Multiple udisk eject display problem
    });
        connect(volumeManager,&Peony::VolumeManager::mountAdded,this,[=](const std::shared_ptr<Peony::Mount> &mount){
        m_proxy_model->invalidate();//display udisk in real time after format it.
    });

    connect(this, &QTreeView::expanded, [=](const QModelIndex &index) {
        auto item = m_proxy_model->itemFromIndex(index);
        qDebug()<<item->uri();
        /*!
          \bug can not expanded? enumerator can not get prepared signal, why?
          */
        item->findChildrenAsync();
    });

    connect(this, &QTreeView::collapsed, [=](const QModelIndex &index) {
        auto item = m_proxy_model->itemFromIndex(index);
        item->clearChildren();
    });

    connect(Experimental_Peony::VolumeManager::getInstance(), &Experimental_Peony::VolumeManager::signal_mountFinished,this,[=](){
        if(!m_currSelectedItem)
            return;
        JumpDirectory(m_currSelectedItem->uri());
        qDebug()<<"挂载后跳转路径："<<m_currSelectedItem->uri();
    });

    connect(this, &QTreeView::clicked, [=](const QModelIndex &index) {
        switch (index.column()) {
        case 0: {
            m_currSelectedItem = m_proxy_model->itemFromIndex(index);
            if(m_currSelectedItem && m_currSelectedItem->isMountable()&&!m_currSelectedItem->isMounted())
                m_currSelectedItem->mount();
            else{
                JumpDirectory(m_currSelectedItem->uri());
            }
            break;

        }
        case 1: {
            auto item = m_proxy_model->itemFromIndex(index);
            if (item->isMounted() || item->isEjectable()||item->isStopable()) {
                auto leftIndex = m_proxy_model->index(index.row(), 0, index.parent());
                this->collapse(leftIndex);
                item->ejectOrUnmount();
            } else {
                // if item is not unmountable, just be same with first column.
                // fix #39716
                if (!item->uri().isNull())
                    Q_EMIT this->updateWindowLocationRequest(item->uri());
            }
            break;
        }
        default:
            break;
        }
    });

    connect(this, &QTreeView::customContextMenuRequested, this, [=](const QPoint &pos) {
        auto index = indexAt(pos);
        auto item = m_proxy_model->itemFromIndex(index);
        if (item) {
            if (item->type() != Peony::SideBarAbstractItem::SeparatorItem) {
                Peony::SideBarMenu menu(item, nullptr);
                QList<QAction *> actionList;
                MainWindow *window = dynamic_cast<MainWindow *>(this->topLevelWidget());

                actionList << menu.addAction(QIcon::fromTheme("window-new-symbolic"), tr("Open In New Window"), [=](){
                    auto enumerator = new Peony::FileEnumerator;
                    enumerator->setEnumerateDirectory(item->uri());
                    enumerator->setAutoDelete();

                    enumerator->connect(enumerator, &Peony::FileEnumerator::prepared, this, [=](const std::shared_ptr<Peony::GErrorWrapper> &err = nullptr, const QString &t = nullptr, bool critical = false){
                        auto targetUri = Peony::FileUtils::getTargetUri(item->uri());
                        if (!targetUri.isEmpty()) {
                            auto enumerator2 = new Peony::FileEnumerator;
                            enumerator2->setEnumerateDirectory(targetUri);
                            enumerator2->connect(enumerator2, &Peony::FileEnumerator::prepared, this, [=](const std::shared_ptr<Peony::GErrorWrapper> &err = nullptr, const QString &t = nullptr, bool critical = false){
                                if (!critical) {
                                    auto newWindow = window->create(targetUri);
                                    dynamic_cast<QWidget *>(newWindow)->show();
                                } else {
                                    auto info = FileInfo::fromUri(targetUri);
                                    QMessageBox::critical(0, 0, tr("Can not open %1, %2").arg(info.get()->displayName()).arg(err.get()->message()));
                                }
                                enumerator2->deleteLater();
                            });
                            enumerator2->prepare();
                        } else if (!err.get() && !critical) {
                            auto newWindow = window->create(item->uri());
                            dynamic_cast<QWidget *>(newWindow)->show();
                        }
                    });

                    enumerator->connect(enumerator, &Peony::FileEnumerator::prepared, [=](){
                        enumerator->deleteLater();
                    });

                    enumerator->prepare();
                });

                actionList << menu.addAction(QIcon::fromTheme("tab-new-symbolic"), tr("Open In New Tab"), [=](){
                    auto enumerator = new Peony::FileEnumerator;
                    enumerator->setEnumerateDirectory(item->uri());
                    enumerator->setAutoDelete();

                    enumerator->connect(enumerator, &Peony::FileEnumerator::prepared, this, [=](const std::shared_ptr<Peony::GErrorWrapper> &err = nullptr, const QString &t = nullptr, bool critical = false){
                        auto targetUri = Peony::FileUtils::getTargetUri(item->uri());
                        if (!targetUri.isEmpty()) {
                            auto enumerator2 = new Peony::FileEnumerator;
                            enumerator2->setEnumerateDirectory(targetUri);
                            enumerator2->connect(enumerator2, &Peony::FileEnumerator::prepared, this, [=](const std::shared_ptr<Peony::GErrorWrapper> &err = nullptr, const QString &t = nullptr, bool critical = false){
                                if (!critical) {
                                    window->addNewTabs(QStringList()<<targetUri);
                                    dynamic_cast<QWidget *>(window)->show();
                                } else {
                                    auto info = FileInfo::fromUri(targetUri);
                                    QMessageBox::critical(0, 0, tr("Can not open %1, %2").arg(info.get()->displayName()).arg(err.get()->message()));
                                }
                                enumerator2->deleteLater();
                            });
                            enumerator2->prepare();
                        } else if (!err.get() && !critical) {
                            window->addNewTabs(QStringList()<<item->uri());
                            dynamic_cast<QWidget *>(window)->show();
                        }
                    });

                    enumerator->connect(enumerator, &Peony::FileEnumerator::prepared, [=](){
                        enumerator->deleteLater();
                    });

                    enumerator->prepare();
                });

                if (item->type() == SideBarAbstractItem::FileSystemItem) {
                    if ((0 != QString::compare(item->uri(), "computer:///")) &&
                        (0 != QString::compare(item->uri(), "filesafe:///"))) {
                        for (const auto &actionItem : actionList) {
                            if(item->isVolume())/* 分区才去需要判断是否已挂载 */
                                actionItem->setEnabled(item->isMounted());
                        }
                    }
                }

                menu.exec(QCursor::pos());
            }
        }
    });

    connect(m_model, &QAbstractItemModel::dataChanged, this, [=](){
        this->viewport()->update();
        m_proxy_model->invalidate();
    });

    expandToDepth(1);/* 快速访问、计算机、网络 各模块往下展开一层 */
}

bool NavigationSideBar::eventFilter(QObject *obj, QEvent *e)
{
    return false;
}

void NavigationSideBar::updateGeometries()
{
    setViewportMargins(4, 0, 4, 0);
    QTreeView::updateGeometries();
    if(m_notAllowHorizontalMove){
        horizontalScrollBar()->setValue(0);/* hotfix bug#93557 */
    }
    m_notAllowHorizontalMove = false;
}

void NavigationSideBar::scrollTo(const QModelIndex &index, QAbstractItemView::ScrollHint hint)
{
    //skip unmount indicator index
    if (index.isValid()) {
        if (index.column() == 0) {
            QTreeView::scrollTo(index, hint);
        }
    }
}

void NavigationSideBar::paintEvent(QPaintEvent *event)
{
    QTreeView::paintEvent(event);
}

void NavigationSideBar::resizeEvent(QResizeEvent *e)
{
    QTreeView::resizeEvent(e);
    if (header()->count() > 0) {
        this->setColumnWidth(1, 20);
        header()->resizeSection(0, this->viewport()->width() - this->columnWidth(1));
    }
}

void NavigationSideBar::dropEvent(QDropEvent *e)
{
    //fix invalid index cause crash issue
    if (! indexAt(e->pos()).isValid())
        return;

    QString destUri = m_proxy_model->itemFromIndex(indexAt(e->pos()))->uri();

    if (dropIndicatorPosition() == QAbstractItemView::AboveItem || dropIndicatorPosition() == QAbstractItemView::BelowItem || "favorite:///" == destUri) {
        // add to bookmark
        e->setAccepted(true);

        auto data = e->mimeData();
        auto bookmark = Peony::BookMarkManager::getInstance();
        if (bookmark->isLoaded()) {
            for (auto url : data->urls()) {
                if(url.toString().startsWith("filesafe:///")){
                    continue;
                }
                //FIXME: replace BLOCKING api in ui thread.
                auto info = Peony::FileInfo::fromUri(url.toDisplayString());
                if (info->displayName().isNull()) {
                    Peony::FileInfoJob j(info);
                    j.querySync();
                }
                if (info->isDir()) {
                    bookmark->addBookMark(url.url());
                }
            }
        }
    }

    if (e->keyboardModifiers() == Qt::ControlModifier) {
        m_model->dropMimeData(e->mimeData(), Qt::CopyAction, 0, 0, QModelIndex());
        e->accept();
        return;
    }

    QTreeView::dropEvent(e);
}

QSize NavigationSideBar::sizeHint() const
{
    return QTreeView::sizeHint();
}

void NavigationSideBar::JumpDirectory(const QString &uri)
{
    if(uri=="" && m_currSelectedItem && m_currSelectedItem->getDevice().startsWith("/dev/sd"))
    {/* 异常U盘 */
        QMessageBox::information(nullptr, tr("Tips"), tr("This is an abnormal Udisk, please fix it or format it"));
        return;
    }

    auto info = FileInfo::fromUri(uri);
    if (info.get()->isEmptyInfo()) {
        FileInfoJob j(info);
        j.querySync();
    }
    auto targetUri = FileUtils::getTargetUri(uri);
    if (targetUri == "" && uri== "burn://")
    {
        qDebug() << "empty drive"<<uri;
        QMessageBox::information(nullptr, tr("Tips"), tr("This is an empty drive, please insert a Disc."));
        return;
    }

    if (!targetUri.isEmpty()) {
        Q_EMIT this->updateWindowLocationRequest(targetUri);
        return;
    }

    //some side bar item doesn't have a uri.
    //do not emit signal with a null uri to window.
    if (!uri.isNull())
        Q_EMIT this->updateWindowLocationRequest(uri);
}

void NavigationSideBar::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Left||event->key()==Qt::Key_Right)
    {
        m_notAllowHorizontalMove = true;
    }

    QTreeView::keyPressEvent(event);

    if (event->key() == Qt::Key_Return) {
        if (!selectedIndexes().isEmpty()) {
            auto index = selectedIndexes().first();
            auto uri = index.data(Qt::UserRole).toString();
            Q_EMIT this->updateWindowLocationRequest(uri, true);
        }
    }
}

void NavigationSideBar::focusInEvent(QFocusEvent *event)
{
    QTreeView::focusInEvent(event);
    if (event->reason() == Qt::TabFocus) {
        if (selectedIndexes().isEmpty()) {
            selectionModel()->select(model()->index(0, 0), QItemSelectionModel::Select);
            selectionModel()->select(model()->index(0, 1), QItemSelectionModel::Select);
        } else {
            scrollTo(selectedIndexes().first(), QTreeView::PositionAtCenter);
            auto selections = selectedIndexes();
            clearSelection();
            QTimer::singleShot(100, this, [=](){
                for (auto index : selections) {
                    selectionModel()->select(index, QItemSelectionModel::Select);
                }
            });
        }
    }
}

int NavigationSideBar::sizeHintForColumn(int column) const
{
    if (column == 1)
        return 22;

    if (column == 0)
        return viewport()->width() - 22;

    return QTreeView::sizeHintForColumn(column);
}

void NavigationSideBar::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls()) {
        if ((event->dropAction() == Qt::MoveAction) && FileUtils::containsStandardPath(event->mimeData()->urls())) {
            event->ignore();
            return;
        }
    }
    QAbstractItemView::dragEnterEvent(event);
}

NavigationSideBarItemDelegate::NavigationSideBarItemDelegate(QObject *parent)
{

}

QSize NavigationSideBarItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    auto size = QStyledItemDelegate::sizeHint(option, index);
    size.setHeight(36);
    return size;
}

void NavigationSideBarItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->save();
    if (index.column() == 1) {
        QPainterPath rightRoundedRegion;
        rightRoundedRegion.setFillRule(Qt::WindingFill);
        auto rect = option.rect;
        auto view = qobject_cast<const QAbstractItemView *>(option.widget);
        if (view) {
            rect.setRight(view->viewport()->rect().right());
        }
        rightRoundedRegion.addRoundedRect(rect, NAVIGATION_SIDEBAR_ITEM_BORDER_RADIUS, NAVIGATION_SIDEBAR_ITEM_BORDER_RADIUS);
        rightRoundedRegion.addRect(rect.adjusted(0, 0, -NAVIGATION_SIDEBAR_ITEM_BORDER_RADIUS, 0));
        painter->setClipPath(rightRoundedRegion);
    }

    painter->setRenderHint(QPainter::Antialiasing);
    QStyledItemDelegate::paint(painter, option, index);
    painter->restore();
}

NavigationSideBarContainer::NavigationSideBarContainer(QWidget *parent)
{
    setAttribute(Qt::WA_TranslucentBackground);

    m_layout = new QVBoxLayout;
    m_layout->setContentsMargins(0, 4, 0, 0);
    m_layout->setSpacing(0);
}

void NavigationSideBarContainer::addSideBar(NavigationSideBar *sidebar)
{
    if (m_sidebar)
        return;

    m_sidebar = sidebar;
    m_layout->addWidget(sidebar);

    QWidget *w = new QWidget(this);
    QVBoxLayout *l = new QVBoxLayout;
    l->setContentsMargins(4, 4, 2, 4);

    m_label_button = new QPushButton(QIcon(":/icons/sign"), tr("All tags..."), this);
    m_label_button->setProperty("useIconHighlightEffect", true);
    m_label_button->setProperty("iconHighlightEffectMode", 1);
    m_label_button->setProperty("fillIconSymbolicColor", true);
    m_label_button->setCheckable(true);

    m_label_button->setFocusPolicy(Qt::FocusPolicy(m_label_button->focusPolicy() & ~Qt::TabFocus));

    l->addWidget(m_label_button);

    connect(m_label_button, &QPushButton::clicked, m_sidebar, &NavigationSideBar::labelButtonClicked);

    w->setLayout(l);

    m_layout->addWidget(w);

    setLayout(m_layout);
}

QSize NavigationSideBarContainer::sizeHint() const
{
    auto size = QWidget::sizeHint();
    auto width = Peony::GlobalSettings::getInstance()->getValue(DEFAULT_SIDEBAR_WIDTH).toInt();
    qDebug() << "sizeHint set DEFAULT_SIDEBAR_WIDTH:"<<width;
    //fix width value abnormal issue
    if (width <= 0)
        width = 210;
    size.setWidth(width);
    return size;
}

NavigationSideBarStyle::NavigationSideBarStyle()
{

}

void NavigationSideBarStyle::drawPrimitive(QStyle::PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    painter->save();
    switch (element) {
    case QStyle::PE_IndicatorBranch: {
        if (option->rect.x() == 0) {
            QPainterPath leftRoundedRegion;
            leftRoundedRegion.setFillRule(Qt::WindingFill);
            leftRoundedRegion.addRoundedRect(option->rect, NAVIGATION_SIDEBAR_ITEM_BORDER_RADIUS, NAVIGATION_SIDEBAR_ITEM_BORDER_RADIUS);
            leftRoundedRegion.addRect(option->rect.adjusted(NAVIGATION_SIDEBAR_ITEM_BORDER_RADIUS, 0, 0, 0));
            painter->setClipPath(leftRoundedRegion);
        }
        break;
    }
    case QStyle::PE_PanelItemViewRow: {
        painter->restore();
        return;
        break;
    }
    case QStyle::PE_PanelItemViewItem: {
        break;
    }
    default:
        break;
    }

    QProxyStyle::drawPrimitive(element, option, painter, widget);
    painter->restore();
}
