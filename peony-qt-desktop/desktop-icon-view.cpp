/*
 * Peony-Qt
 *
 * Copyright (C) 2019, Tianjin KYLIN Information Technology Co., Ltd.
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

#include "peony-desktop-application.h"
#include "desktop-icon-view.h"

#include "icon-view-style.h"
#include "desktop-icon-view-delegate.h"

#include "desktop-item-model.h"
#include "desktop-item-proxy-model.h"

#include "file-operation-manager.h"
#include "file-move-operation.h"
#include "file-copy-operation.h"
#include "file-trash-operation.h"
#include "clipboard-utils.h"

#include "properties-window.h"
#include "file-utils.h"
#include "file-operation-utils.h"

#include "desktop-menu.h"
#include "desktop-window.h"

#include "file-item-model.h"
#include "file-info-job.h"
#include "file-launch-manager.h"
#include <QProcess>

#include <QDesktopServices>

#include "desktop-index-widget.h"

#include "file-meta-info.h"

#include "global-settings.h"

//play audio lib head file
#include <canberra.h>

#include <QAction>
#include <QMouseEvent>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QMimeData>

#include <QHoverEvent>

#include <QWheelEvent>
#include <QApplication>

#include <QStringList>
#include <QMessageBox>
#include <QDir>

#include <QDebug>

using namespace Peony;

#define ITEM_POS_ATTRIBUTE "metadata::peony-qt-desktop-item-position"

DesktopIconView::DesktopIconView(QWidget *parent) : QListView(parent)
{
    //m_refresh_timer.setInterval(500);
    //m_refresh_timer.setSingleShot(true);

    installEventFilter(this);

    initShoutCut();
    //initMenu();
    initDoubleClick();

    connect(qApp, &QApplication::paletteChanged, this, [=]() {
        viewport()->update();
    });

    m_edit_trigger_timer.setSingleShot(true);
    m_edit_trigger_timer.setInterval(3000);
    m_last_index = QModelIndex();

    setContentsMargins(0, 0, 0, 0);
    setAttribute(Qt::WA_TranslucentBackground);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    //fix rubberband style.
    setStyle(DirectoryView::IconViewStyle::getStyle());

    setItemDelegate(new DesktopIconViewDelegate(this));

    setDefaultDropAction(Qt::MoveAction);

    setSelectionMode(QListView::ExtendedSelection);
    setEditTriggers(QListView::NoEditTriggers);
    setViewMode(QListView::IconMode);
    setMovement(QListView::Snap);
    setFlow(QListView::TopToBottom);
    setResizeMode(QListView::Fixed);
    setWordWrap(true);

    setDragDropMode(QListView::DragDrop);

    //setContextMenuPolicy(Qt::CustomContextMenu);
    setSelectionMode(QListView::ExtendedSelection);

    auto zoomLevel = this->zoomLevel();
    setDefaultZoomLevel(zoomLevel);

//#if QT_VERSION > QT_VERSION_CHECK(5, 12, 0)
//    QTimer::singleShot(500, this, [=](){
//#else
//    QTimer::singleShot(500, [=](){
//#endif
//        connect(this->selectionModel(), &QItemSelectionModel::selectionChanged, [=](const QItemSelection &selection, const QItemSelection &deselection){
//            //qDebug()<<"selection changed";
//            m_real_do_edit = false;
//            this->setIndexWidget(m_last_index, nullptr);
//            auto currentSelections = this->selectionModel()->selection().indexes();

//            if (currentSelections.count() == 1) {
//                //qDebug()<<"set index widget";
//                m_last_index = currentSelections.first();
//                auto delegate = qobject_cast<DesktopIconViewDelegate *>(itemDelegate());
//                this->setIndexWidget(m_last_index, new DesktopIndexWidget(delegate, viewOptions(), m_last_index, this));
//            } else {
//                m_last_index = QModelIndex();
//                for (auto index : deselection.indexes()) {
//                    this->setIndexWidget(index, nullptr);
//                }
//            }
//        });
//    });

    m_model = new DesktopItemModel(this);
    m_proxy_model = new DesktopItemProxyModel(m_model);

    m_proxy_model->setSourceModel(m_model);

    connect(m_model, &QAbstractItemModel::rowsRemoved, this, [=](){
        for (auto uri : getAllFileUris()) {
            auto pos = getFileMetaInfoPos(uri);
            if (pos.x() >= 0)
                updateItemPosByUri(uri, pos);
        }
    });

    //connect(m_model, &DesktopItemModel::dataChanged, this, &DesktopIconView::clearAllIndexWidgets);

    connect(m_model, &DesktopItemModel::refreshed, this, [=]() {
        this->setCursor(QCursor(Qt::ArrowCursor));
        m_is_refreshing = false;

        // check if there are items overlapped.
        QTimer::singleShot(150, this, [=](){
            if (isItemsOverlapped()) {
                // refresh again?
                //this->refresh();
            }
        });
        return;
    });

    connect(m_model, &DesktopItemModel::requestClearIndexWidget, this, &DesktopIconView::clearAllIndexWidgets);

    connect(m_model, &DesktopItemModel::fileCreated, this, [=](const QString &uri) {
        if (m_new_files_to_be_selected.isEmpty()) {
            m_new_files_to_be_selected<<uri;

            QTimer::singleShot(500, this, [=]() {
                if (this->state() & QAbstractItemView::EditingState)
                    return;
                this->setSelections(m_new_files_to_be_selected);
                m_new_files_to_be_selected.clear();
            });
        } else {
            if (!m_new_files_to_be_selected.contains(uri)) {
                m_new_files_to_be_selected<<uri;
            }
        }
        //refresh();
    });

    connect(m_proxy_model, &QSortFilterProxyModel::layoutChanged, this, [=]() {
        //qDebug()<<"layout changed=========================\n\n\n\n\n";
        if (m_proxy_model->getSortType() == DesktopItemProxyModel::Other) {
            return;
        }
        if (m_proxy_model->sortColumn() != 0) {
            return;
        }
        //qDebug()<<"save====================================";

        QTimer::singleShot(100, this, [=]() {
            //this->saveAllItemPosistionInfos();
        });
    });

    connect(this, &QListView::iconSizeChanged, this, [=]() {
        //qDebug()<<"save=============";
        this->setSortType(GlobalSettings::getInstance()->getValue(LAST_DESKTOP_SORT_ORDER).toInt());

        QTimer::singleShot(100, this, [=]() {
            this->saveAllItemPosistionInfos();
            for (int i = 0; i < m_proxy_model->rowCount(); i++) {
                auto index = m_proxy_model->index(i, 0);
                m_item_rect_hash.insert(index.data(Qt::UserRole).toString(), QListView::visualRect(index));
                updateItemPosByUri(index.data(Qt::UserRole).toString(), QListView::visualRect(index).topLeft());
            }
        });
    });

    setModel(m_proxy_model);
    //m_proxy_model->sort(0);

    this->refresh();
}

DesktopIconView::~DesktopIconView()
{
    //saveAllItemPosistionInfos();
}

bool DesktopIconView::eventFilter(QObject *obj, QEvent *e)
{
    //fixme:
    if (e->type() == QEvent::StyleChange) {
        if (m_model)
            refresh();
    }
    return false;
}

void DesktopIconView::initShoutCut()
{
    QAction *copyAction = new QAction(this);
    copyAction->setShortcut(QKeySequence::Copy);
    connect(copyAction, &QAction::triggered, [=]() {
        auto selectedUris = this->getSelections();
        if (!selectedUris.isEmpty())
            ClipboardUtils::setClipboardFiles(selectedUris, false);
    });
    addAction(copyAction);

    QAction *cutAction = new QAction(this);
    cutAction->setShortcut(QKeySequence::Cut);
    connect(cutAction, &QAction::triggered, [=]() {
        auto selectedUris = this->getSelections();
        if (!selectedUris.isEmpty())
            ClipboardUtils::setClipboardFiles(selectedUris, true);
    });
    addAction(cutAction);

    QAction *pasteAction = new QAction(this);
    pasteAction->setShortcut(QKeySequence::Paste);
    connect(pasteAction, &QAction::triggered, [=]() {
        auto clipUris = ClipboardUtils::getClipboardFilesUris();
        if (ClipboardUtils::isClipboardHasFiles()) {
            ClipboardUtils::pasteClipboardFiles(this->getDirectoryUri());
        }
    });
    addAction(pasteAction);

    //add CTRL+D for delete operation
    auto trashAction = new QAction(this);
    trashAction->setShortcuts(QList<QKeySequence>()<<Qt::Key_Delete<<QKeySequence(Qt::CTRL + Qt::Key_D));
    connect(trashAction, &QAction::triggered, [=]() {
        auto selectedUris = getSelections();
        if (! selectedUris.isEmpty())
           FileOperationUtils::trash(selectedUris, true);
    });
    addAction(trashAction);

    QAction *undoAction = new QAction(this);
    undoAction->setShortcut(QKeySequence::Undo);
    connect(undoAction, &QAction::triggered,
    [=]() {
        // do not relayout item with undo.
        setRenaming(true);
        FileOperationManager::getInstance()->undo();
    });
    addAction(undoAction);

    QAction *redoAction = new QAction(this);
    redoAction->setShortcut(QKeySequence::Redo);
    connect(redoAction, &QAction::triggered,
    [=]() {
        // do not relayout item with redo.
        setRenaming(true);
        FileOperationManager::getInstance()->redo();
    });
    addAction(redoAction);

    QAction *zoomInAction = new QAction(this);
    zoomInAction->setShortcut(QKeySequence::ZoomIn);
    connect(zoomInAction, &QAction::triggered, [=]() {
        this->zoomIn();
    });
    addAction(zoomInAction);

    QAction *zoomOutAction = new QAction(this);
    zoomOutAction->setShortcut(QKeySequence::ZoomOut);
    connect(zoomOutAction, &QAction::triggered, [=]() {
        this->zoomOut();
    });
    addAction(zoomOutAction);

    QAction *renameAction = new QAction(this);
    renameAction->setShortcut(QKeySequence(Qt::ALT + Qt::Key_E));
    connect(renameAction, &QAction::triggered, [=]() {
        auto selections = this->getSelections();
        if (selections.count() == 1) {
            this->editUri(selections.first());
        }
    });
    addAction(renameAction);

    QAction *removeAction = new QAction(this);
    removeAction->setShortcut(QKeySequence(Qt::SHIFT + Qt::Key_Delete));
    connect(removeAction, &QAction::triggered, [=]() {
        qDebug() << "delete" << this->getSelections();
        clearAllIndexWidgets();
        FileOperationUtils::executeRemoveActionWithDialog(this->getSelections());
    });
    addAction(removeAction);

    QAction *helpAction = new QAction(this);
    helpAction->setShortcut(Qt::Key_F1);
    connect(helpAction, &QAction::triggered, this, [=]() {
        PeonyDesktopApplication::showGuide();
    });
    addAction(helpAction);

    auto propertiesWindowAction = new QAction(this);
    propertiesWindowAction->setShortcuts(QList<QKeySequence>()<<QKeySequence(Qt::ALT + Qt::Key_Return)
                                         <<QKeySequence(Qt::ALT + Qt::Key_Enter));
    connect(propertiesWindowAction, &QAction::triggered, this, [=]() {
        if (this->getSelections().count() > 0)
        {
            PropertiesWindow *w = new PropertiesWindow(this->getSelections());
            w->show();
        }
    });
    addAction(propertiesWindowAction);

    auto newFolderAction = new QAction(this);
    newFolderAction->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_N));
    connect(newFolderAction, &QAction::triggered, this, [=]() {
        CreateTemplateOperation op(this->getDirectoryUri(), CreateTemplateOperation::EmptyFolder, tr("New Folder"));
        op.run();
        auto targetUri = op.target();

        QTimer::singleShot(500, this, [=]() {
            this->scrollToSelection(targetUri);
        });
    });
    addAction(newFolderAction);

    QAction *refreshWinAction = new QAction(this);
    refreshWinAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_R));
    connect(refreshWinAction, &QAction::triggered, [=]() {
        this->refresh();
    });
    addAction(refreshWinAction);

    QAction *reverseSelectAction = new QAction(this);
    reverseSelectAction->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_L));
    connect(reverseSelectAction, &QAction::triggered, [=]() {
        this->invertSelections();
    });
    addAction(reverseSelectAction);

    QAction *normalIconAction = new QAction(this);
    normalIconAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_0));
    connect(normalIconAction, &QAction::triggered, [=]() {
        this->setDefaultZoomLevel(DesktopIconView::Normal);
    });
    addAction(normalIconAction);

    auto refreshAction = new QAction(this);
    refreshAction->setShortcut(Qt::Key_F5);
    connect(refreshAction, &QAction::triggered, this, [=]() {
        this->refresh();
    });
    addAction(refreshAction);

    QAction *editAction = new QAction(this);
    editAction->setShortcuts(QList<QKeySequence>()<<QKeySequence(Qt::ALT + Qt::Key_E)<<Qt::Key_F2);
    connect(editAction, &QAction::triggered, this, [=]() {
        auto selections = this->getSelections();
        if (selections.count() == 1) {
            this->editUri(selections.first());
        }
    });
    addAction(editAction);

    auto settings = GlobalSettings::getInstance();
    m_show_hidden = settings->isExist("show-hidden")? settings->getValue("show-hidden").toBool(): false;
    //show hidden action
    QAction *showHiddenAction = new QAction(this);
    showHiddenAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_H));
    addAction(showHiddenAction);
    connect(showHiddenAction, &QAction::triggered, this, [=]() {
        //qDebug() << "show hidden";
        this->setShowHidden();
    });
}

void DesktopIconView::initMenu()
{
    /*!
     * \bug
     *
     * when view switch to another desktop window,
     * menu might no be callable.
     */
    return;
    setContextMenuPolicy(Qt::CustomContextMenu);

    // menu
    connect(this, &QListView::customContextMenuRequested, this,
    [=](const QPoint &pos) {
        // FIXME: use other menu
        qDebug() << "menu request";
        if (!this->indexAt(pos).isValid()) {
            this->clearSelection();
        } else {
            this->clearSelection();
            this->selectionModel()->select(this->indexAt(pos), QItemSelectionModel::Select);
        }

        QTimer::singleShot(1, [=]() {
            DesktopMenu menu(this);
            if (this->getSelections().isEmpty()) {
                auto action = menu.addAction(tr("set background"));
                connect(action, &QAction::triggered, [=]() {
                    //go to control center set background
                    DesktopWindow::gotoSetBackground();
//                    QFileDialog dlg;
//                    dlg.setNameFilters(QStringList() << "*.jpg"
//                                       << "*.png");
//                    if (dlg.exec()) {
//                        auto url = dlg.selectedUrls().first();
//                        this->setBg(url.path());
//                        // qDebug()<<url;
//                        Q_EMIT this->changeBg(url.path());
//                    }
                });
            }
            menu.exec(QCursor::pos());
            auto urisToEdit = menu.urisToEdit();
            if (urisToEdit.count() == 1) {
                QTimer::singleShot(
                100, this, [=]() {
                    this->editUri(urisToEdit.first());
                });
            }
        });
    }, Qt::UniqueConnection);
}

void DesktopIconView::setShowHidden()
{
    m_show_hidden = ! m_show_hidden;
    qDebug() << "DesktopIconView::setShowHidden:" <<m_show_hidden;
    m_proxy_model->setShowHidden(m_show_hidden);
}

void DesktopIconView::openFileByUri(QString uri)
{
    auto info = FileInfo::fromUri(uri, false);
    auto job = new FileInfoJob(info);
    job->setAutoDelete();
    job->connect(job, &FileInfoJob::queryAsyncFinished, [=]() {
        if ((info->isDir() || info->isVolume() || info->isVirtual())) {
            QDir dir(info->filePath());
            if (! dir.exists())
            {
                ca_context *caContext;
                ca_context_create(&caContext);
                const gchar* eventId = "dialog-warning";
                //eventid 是/usr/share/sounds音频文件名,不带后缀
                ca_context_play (caContext, 0,
                                 CA_PROP_EVENT_ID, eventId,
                                 CA_PROP_EVENT_DESCRIPTION, tr("Delete file Warning"), NULL);

                auto result = QMessageBox::question(nullptr, tr("Open Link failed"),
                                      tr("File not exist, do you want to delete the link file?"));
                if (result == QMessageBox::Yes) {
                    qDebug() << "Delete unused symbollink in desktop.";
                    QStringList selections;
                    selections.push_back(uri);
                    FileOperationUtils::trash(selections, true);
                }
                return;
            }

            if (! info->uri().startsWith("trash://")
                    && ! info->uri().startsWith("computer://")
                    &&  ! info->canExecute())
            {
                ca_context *caContext;
                ca_context_create(&caContext);
                const gchar* eventId = "dialog-warning";
                //eventid 是/usr/share/sounds音频文件名,不带后缀
                ca_context_play (caContext, 0,
                                 CA_PROP_EVENT_ID, eventId,
                                 CA_PROP_EVENT_DESCRIPTION, tr("Delete file Warning"), NULL);

                QMessageBox::critical(nullptr, tr("Open failed"),
                                      tr("Open directory failed, you have no permission!"));
                return;
            }
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
            QProcess p;
            QUrl url = uri;
            p.setProgram("peony");
            p.setArguments(QStringList() << url.toEncoded() <<"%U&");
            p.startDetached();
#else
            QProcess p;
            QString strq;
            for (int i = 0;i < uri.length();++i) {
                if(uri[i] == ' '){
                    strq += "%20";
                }else{
                    strq += uri[i];
                }
            }

            p.startDetached("peony", QStringList()<<strq<<"%U&");
#endif
        } else {

            FileLaunchManager::openAsync(uri, false, false);


        }
        this->clearSelection();
    });
    job->queryAsync();
}

void DesktopIconView::initDoubleClick()
{
    connect(this, &QListView::doubleClicked, this, [=](const QModelIndex &index) {
        qDebug() << "double click" << index.data(FileItemModel::UriRole);
        auto uri = index.data(FileItemModel::UriRole).toString();
        openFileByUri(uri);
    }, Qt::UniqueConnection);
}

void DesktopIconView::saveAllItemPosistionInfos()
{
    //qDebug()<<"======================save";
    for (int i = 0; i < m_proxy_model->rowCount(); i++) {
        auto index = m_proxy_model->index(i, 0);
        auto indexRect = QListView::visualRect(index);
        QStringList topLeft;
        topLeft<<QString::number(indexRect.top());
        topLeft<<QString::number(indexRect.left());

        auto metaInfo = FileMetaInfo::fromUri(index.data(Qt::UserRole).toString());
        if (metaInfo) {
            //qDebug()<<"save real"<<index.data()<<topLeft;
            metaInfo->setMetaInfoStringList(ITEM_POS_ATTRIBUTE, topLeft);
        }
    }
    //qDebug()<<"======================save finished";
}

void DesktopIconView::saveItemPositionInfo(const QString &uri)
{
    return;
}

void DesktopIconView::resetAllItemPositionInfos()
{
    if (!m_proxy_model)
        return;

    m_item_rect_hash.clear();
    for (int i = 0; i < m_proxy_model->rowCount(); i++) {
        auto index = m_proxy_model->index(i, 0);
        auto indexRect = QListView::visualRect(index);
        QStringList topLeft;
        topLeft<<QString::number(indexRect.top());
        topLeft<<QString::number(indexRect.left());
        auto metaInfo = FileMetaInfo::fromUri(index.data(Qt::UserRole).toString());
        if (metaInfo) {
            QStringList tmp;
            tmp<<"-1"<<"-1";
            metaInfo->setMetaInfoStringList(ITEM_POS_ATTRIBUTE, tmp);
        }
    }
}

void DesktopIconView::resetItemPosistionInfo(const QString &uri)
{
    return;
}

void DesktopIconView::updateItemPosistions(const QString &uri)
{
    return;
}

QPoint DesktopIconView::getFileMetaInfoPos(const QString &uri)
{
    auto value = m_item_rect_hash.value(uri);
    if (!value.isEmpty())
        return value.topLeft();

    auto metaInfo = FileMetaInfo::fromUri(uri);
    if (metaInfo) {
        auto list = metaInfo->getMetaInfoStringList(ITEM_POS_ATTRIBUTE);
        if (!list.isEmpty()) {
            if (list.count() == 2) {
                int top = list.first().toInt();
                int left = list.at(1).toInt();
                if (top > 0 && left >= 0) {
                    QPoint p(left, top);
                    return p;
                }
            }
        }
    }
    return QPoint(-1, -1);
}

void DesktopIconView::setFileMetaInfoPos(const QString &uri, const QPoint &pos)
{
    auto srcIndex = m_model->indexFromUri("computer:///");
    auto index = m_proxy_model->mapFromSource(srcIndex);
    m_item_rect_hash.remove(uri);
    m_item_rect_hash.insert(uri, QRect(pos, QListView::visualRect(index).size()));

    auto metaInfo = FileMetaInfo::fromUri(uri);
    if (metaInfo) {
        QStringList topLeft;
        topLeft<<QString::number(pos.y());
        topLeft<<QString::number(pos.x());
        metaInfo->setMetaInfoStringList(ITEM_POS_ATTRIBUTE, topLeft);
    }
}

QHash<QString, QRect> DesktopIconView::getCurrentItemRects()
{
    return m_item_rect_hash;
}

void DesktopIconView::removeItemRect(const QString &uri)
{
    m_item_rect_hash.remove(uri);
}

void DesktopIconView::updateItemPosByUri(const QString &uri, const QPoint &pos)
{
    auto srcIndex = m_model->indexFromUri(uri);
    auto index = m_proxy_model->mapFromSource(srcIndex);
    if (index.isValid()) {
        setPositionForIndex(pos, index);
        m_item_rect_hash.remove(uri);
        m_item_rect_hash.insert(uri, QRect(pos, QListView::visualRect(index).size()));
    }
}

void DesktopIconView::ensureItemPosByUri(const QString &uri)
{
    auto srcIndex = m_model->indexFromUri(uri);
    auto index = m_proxy_model->mapFromSource(srcIndex);
    auto rect = QListView::visualRect(index);
    if (index.isValid()) {
        m_item_rect_hash.remove(uri);
        m_item_rect_hash.insert(uri, rect);
        setFileMetaInfoPos(uri, rect.topLeft());
    }
}

const QStringList DesktopIconView::getSelections()
{
    QStringList uris;
    auto indexes = selectionModel()->selection().indexes();
    for (auto index : indexes) {
        uris<<index.data(Qt::UserRole).toString();
    }
    uris.removeDuplicates();
    return uris;
}

const QStringList DesktopIconView::getAllFileUris()
{
    QStringList uris;
    for (int i = 0; i < m_proxy_model->rowCount(); i++) {
        auto index = m_proxy_model->index(i, 0);
        uris<<index.data(Qt::UserRole).toString();
    }
    return uris;
}

void DesktopIconView::setSelections(const QStringList &uris)
{
    clearSelection();
    for (int i = 0; i < m_proxy_model->rowCount(); i++) {
        auto index = m_proxy_model->index(i, 0);
        if (uris.contains(index.data(Qt::UserRole).toString())) {
            selectionModel()->select(index, QItemSelectionModel::Select);
        }
    }
}

void DesktopIconView::invertSelections(bool isInvert)
{
    QItemSelectionModel *selectionModel = this->selectionModel();
    const QItemSelection currentSelection = selectionModel->selection();
    this->selectAll();
    selectionModel->select(currentSelection, QItemSelectionModel::Deselect);
    clearAllIndexWidgets();
}

void DesktopIconView::scrollToSelection(const QString &uri)
{

}

int DesktopIconView::getSortType()
{
    return m_proxy_model->getSortType();
}

void DesktopIconView::setSortType(int sortType)
{
    m_item_rect_hash.clear();
    //resetAllItemPositionInfos();
    m_proxy_model->setSortType(sortType);
    m_proxy_model->sort(1);
    m_proxy_model->sort(0, m_proxy_model->sortOrder());
    saveAllItemPosistionInfos();
    for (int i = 0; i < m_proxy_model->rowCount(); i++) {
        auto index = m_proxy_model->index(i, 0);
        m_item_rect_hash.insert(index.data(Qt::UserRole).toString(), QListView::visualRect(index));
        updateItemPosByUri(index.data(Qt::UserRole).toString(), QListView::visualRect(index).topLeft());
    }
}

int DesktopIconView::getSortOrder()
{
    return m_proxy_model->sortOrder();
}

void DesktopIconView::setSortOrder(int sortOrder)
{
    m_proxy_model->sort(0, Qt::SortOrder(sortOrder));
}

void DesktopIconView::editUri(const QString &uri)
{
    clearAllIndexWidgets();
    auto origin = FileUtils::getOriginalUri(uri);
    QTimer::singleShot(100, this, [=]() {
        edit(m_proxy_model->mapFromSource(m_model->indexFromUri(origin)));
    });
}

void DesktopIconView::editUris(const QStringList uris)
{

}

void DesktopIconView::setCutFiles(const QStringList &uris)
{
    ClipboardUtils::setClipboardFiles(uris, true);
}

void DesktopIconView::closeView()
{
    deleteLater();
}

void DesktopIconView::wheelEvent(QWheelEvent *e)
{
    if (QApplication::keyboardModifiers() == Qt::ControlModifier)
    {
        if (e->delta() > 0) {
            zoomIn();
        } else {
            zoomOut();
        }
    }
}

void DesktopIconView::keyPressEvent(QKeyEvent *e)
{
    switch (e->key()) {
    case Qt::Key_Home: {
        auto boundingRect = getBoundingRect();
        QRect homeRect = QRect(boundingRect.topLeft(), this->gridSize());
        while (!indexAt(homeRect.center()).isValid()) {
            homeRect.translate(0, gridSize().height());
        }
        auto homeIndex = indexAt(homeRect.center());
        selectionModel()->select(homeIndex, QItemSelectionModel::SelectCurrent);
        break;
    }
    case Qt::Key_End: {
        auto boundingRect = getBoundingRect();
        QRect endRect = QRect(boundingRect.bottomRight(), this->gridSize());
        endRect.translate(-gridSize().width(), -gridSize().height());
        while (!indexAt(endRect.center()).isValid()) {
            endRect.translate(0, -gridSize().height());
        }
        auto endIndex = indexAt(endRect.center());
        selectionModel()->select(endIndex, QItemSelectionModel::SelectCurrent);
        break;
    }
    case Qt::Key_Up: {
        if (getSelections().isEmpty()) {
            selectionModel()->select(model()->index(0, 0), QItemSelectionModel::SelectCurrent);
        } else {
            auto index = selectionModel()->selectedIndexes().first();
            auto center = visualRect(index).center();
            auto up = center - QPoint(0, gridSize().height());
            auto upIndex = indexAt(up);
            if (upIndex.isValid()) {
                clearAllIndexWidgets();
                selectionModel()->select(upIndex, QItemSelectionModel::SelectCurrent);
                auto delegate = qobject_cast<DesktopIconViewDelegate *>(itemDelegate());
                setIndexWidget(upIndex, new DesktopIndexWidget(delegate, viewOptions(), upIndex, this));
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
                for (auto uri : getAllFileUris()) {
                    auto pos = getFileMetaInfoPos(uri);
                    if (pos.x() >= 0)
                        updateItemPosByUri(uri, pos);
                }
#endif
            }
        }
        return;
    }
    case Qt::Key_Down: {
        if (getSelections().isEmpty()) {
            selectionModel()->select(model()->index(0, 0), QItemSelectionModel::SelectCurrent);
        } else {
            auto index = selectionModel()->selectedIndexes().first();
            auto center = visualRect(index).center();
            auto down = center + QPoint(0, gridSize().height());
            auto downIndex = indexAt(down);
            if (downIndex.isValid()) {
                clearAllIndexWidgets();
                selectionModel()->select(downIndex, QItemSelectionModel::SelectCurrent);
                auto delegate = qobject_cast<DesktopIconViewDelegate *>(itemDelegate());
                setIndexWidget(downIndex, new DesktopIndexWidget(delegate, viewOptions(), downIndex, this));
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
                for (auto uri : getAllFileUris()) {
                    auto pos = getFileMetaInfoPos(uri);
                    if (pos.x() >= 0)
                        updateItemPosByUri(uri, pos);
                }
#endif
            }
        }
        return;
    }
    case Qt::Key_Left: {
        if (getSelections().isEmpty()) {
            selectionModel()->select(model()->index(0, 0), QItemSelectionModel::SelectCurrent);
        } else {
            auto index = selectionModel()->selectedIndexes().first();
            auto center = visualRect(index).center();
            auto left = center - QPoint(gridSize().width(), 0);
            auto leftIndex = indexAt(left);
            if (leftIndex.isValid()) {
                clearAllIndexWidgets();
                selectionModel()->select(leftIndex, QItemSelectionModel::SelectCurrent);
                auto delegate = qobject_cast<DesktopIconViewDelegate *>(itemDelegate());
                setIndexWidget(leftIndex, new DesktopIndexWidget(delegate, viewOptions(), leftIndex, this));
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
                for (auto uri : getAllFileUris()) {
                    auto pos = getFileMetaInfoPos(uri);
                    if (pos.x() >= 0)
                        updateItemPosByUri(uri, pos);
                }
#endif
            }
        }
        return;
    }
    case Qt::Key_Right: {
        if (getSelections().isEmpty()) {
            selectionModel()->select(model()->index(0, 0), QItemSelectionModel::SelectCurrent);
        } else {
            auto index = selectionModel()->selectedIndexes().first();
            auto center = visualRect(index).center();
            auto right = center + QPoint(gridSize().width(), 0);
            auto rightIndex = indexAt(right);
            if (rightIndex.isValid()) {
                clearAllIndexWidgets();
                selectionModel()->select(rightIndex, QItemSelectionModel::SelectCurrent);
                auto delegate = qobject_cast<DesktopIconViewDelegate *>(itemDelegate());
                setIndexWidget(rightIndex, new DesktopIndexWidget(delegate, viewOptions(), rightIndex, this));
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
                for (auto uri : getAllFileUris()) {
                    auto pos = getFileMetaInfoPos(uri);
                    if (pos.x() >= 0)
                        updateItemPosByUri(uri, pos);
                }
#endif
            }
        }
        return;
    }
    case Qt::Key_Shift:
    case Qt::Key_Control:
        m_ctrl_or_shift_pressed = true;
        break;
    case Qt::Key_Enter:
    case Qt::Key_Return:
    {
        auto selections = this->getSelections();
        for (auto uri : selections)
        {
           openFileByUri(uri);
        }
    }
        break;
    default:
        return QListView::keyPressEvent(e);
    }
}

void DesktopIconView::keyReleaseEvent(QKeyEvent *e)
{
    QListView::keyReleaseEvent(e);
    m_ctrl_or_shift_pressed = false;
}

void DesktopIconView::focusOutEvent(QFocusEvent *e)
{
    QListView::focusOutEvent(e);
    m_ctrl_or_shift_pressed = false;
}

void DesktopIconView::resizeEvent(QResizeEvent *e)
{
    QListView::resizeEvent(e);
    //refresh();
}

void DesktopIconView::rowsInserted(const QModelIndex &parent, int start, int end)
{
    QListView::rowsInserted(parent, start, end);
    for (auto uri : getAllFileUris()) {
        auto pos = getFileMetaInfoPos(uri);
        if (pos.x() >= 0)
            updateItemPosByUri(uri, pos);
    }
}

void DesktopIconView::rowsAboutToBeRemoved(const QModelIndex &parent, int start, int end)
{
    QListView::rowsAboutToBeRemoved(parent, start, end);
//    QTimer::singleShot(1, this, [=](){
//        for (auto uri : getAllFileUris()) {
//            auto pos = getFileMetaInfoPos(uri);
//            if (pos.x() >= 0)
//                updateItemPosByUri(uri, pos);
//        }
//    });
}

bool DesktopIconView::isItemsOverlapped()
{
    QList<QRect> itemRects;
    if (model()) {
        for (int i = 0; i < model()->rowCount(); i++) {
            auto index = model()->index(i, 0);
            auto rect = QListView::visualRect(index);
            if (itemRects.contains(rect))
                return true;
            itemRects<<QListView::visualRect(index);
        }
    }

    return false;
}

bool DesktopIconView::isRenaming()
{
    return m_is_renaming;
}

void DesktopIconView::setRenaming(bool renaming)
{
    m_is_renaming = renaming;
}

const QRect DesktopIconView::getBoundingRect()
{
    QRegion itemsRegion;
    for (int i = 0; i < m_proxy_model->rowCount(); i++) {
        auto index = m_proxy_model->index(i, 0);
        QRect indexRect = QListView::visualRect(index);
        itemsRegion += indexRect;
    }
    return itemsRegion.boundingRect();
}

void DesktopIconView::zoomOut()
{
    clearAllIndexWidgets();
    switch (zoomLevel()) {
    case Huge:
        setDefaultZoomLevel(Large);
        break;
    case Large:
        setDefaultZoomLevel(Normal);
        break;
    case Normal:
        setDefaultZoomLevel(Small);
        break;
    default:
        //setDefaultZoomLevel(zoomLevel());
        break;
    }
}

void DesktopIconView::zoomIn()
{
    clearAllIndexWidgets();
    switch (zoomLevel()) {
    case Small:
        setDefaultZoomLevel(Normal);
        break;
    case Normal:
        setDefaultZoomLevel(Large);
        break;
    case Large:
        setDefaultZoomLevel(Huge);
        break;
    default:
        //setDefaultZoomLevel(zoomLevel());
        break;
    }
}

/*
Small, //icon: 32x32; grid: 56x64; hover rect: 40x56; font: system*0.8
Normal, //icon: 48x48; grid: 64x72; hover rect = 56x64; font: system
Large, //icon: 64x64; grid: 115x135; hover rect = 105x118; font: system*1.2
Huge //icon: 96x96; grid: 130x150; hover rect = 120x140; font: system*1.4
*/
void DesktopIconView::setDefaultZoomLevel(ZoomLevel level)
{
    //qDebug()<<"set default zoom level:"<<level;
    m_zoom_level = level;
    switch (level) {
    case Small:
        setIconSize(QSize(24, 24));
        setGridSize(QSize(64, 74));
        break;
    case Large:
        setIconSize(QSize(64, 64));
        setGridSize(QSize(115, 145));
        break;
    case Huge:
        setIconSize(QSize(96, 96));
        setGridSize(QSize(140, 180));
        break;
    default:
        m_zoom_level = Normal;
        setIconSize(QSize(48, 48));
        setGridSize(QSize(96, 106));
        break;
    }
    clearAllIndexWidgets();
    auto metaInfo = FileMetaInfo::fromUri("computer:///");
    if (metaInfo) {
        qDebug()<<"set zoom level"<<m_zoom_level;
        metaInfo->setMetaInfoInt("peony-qt-desktop-zoom-level", int(m_zoom_level));
    } else {

    }

    resetAllItemPositionInfos();
}

DesktopIconView::ZoomLevel DesktopIconView::zoomLevel() const
{
    //FIXME:
    if (m_zoom_level != Invalid)
        return m_zoom_level;

    auto metaInfo = FileMetaInfo::fromUri("computer:///");
    if (metaInfo) {
        auto i = metaInfo->getMetaInfoInt("peony-qt-desktop-zoom-level");
        return ZoomLevel(i);
    }

    GFile *computer = g_file_new_for_uri("computer:///");
    GFileInfo *info = g_file_query_info(computer,
                                        "metadata::peony-qt-desktop-zoom-level",
                                        G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS,
                                        nullptr,
                                        nullptr);
    char* zoom_level = g_file_info_get_attribute_as_string(info, "metadata::peony-qt-desktop-zoom-level");
    if (!zoom_level) {
        //qDebug()<<"======================no zoom level meta info\n\n\n";
        g_object_unref(info);
        g_object_unref(computer);
        return Normal;
    }
    g_object_unref(info);
    g_object_unref(computer);
    QString zoomLevel = zoom_level;
    g_free(zoom_level);
    //qDebug()<<ZoomLevel(QString(zoomLevel).toInt())<<"\n\n\n\n\n\n\n\n";
    return ZoomLevel(zoomLevel.toInt()) == Invalid? Normal: ZoomLevel(QString(zoomLevel).toInt());
}

void DesktopIconView::mousePressEvent(QMouseEvent *e)
{
    // bug extend selection bug
    m_real_do_edit = false;

    if (!m_ctrl_or_shift_pressed) {
        if (!indexAt(e->pos()).isValid()) {
            clearAllIndexWidgets();
            clearSelection();
        } else {
            auto index = indexAt(e->pos());
            clearAllIndexWidgets();
            m_last_index = index;
            if (!indexWidget(m_last_index)) {
                setIndexWidget(m_last_index,
                               new DesktopIndexWidget(qobject_cast<DesktopIconViewDelegate *>(itemDelegate()), viewOptions(), m_last_index));
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
                for (auto uri : getAllFileUris()) {
                    auto pos = getFileMetaInfoPos(uri);
                    if (pos.x() >= 0)
                        updateItemPosByUri(uri, pos);
                }
#endif
            }
        }
    }

    //qDebug()<<m_last_index.data();
    if (e->button() != Qt::LeftButton) {
        return;
    }

    QListView::mousePressEvent(e);
}

void DesktopIconView::mouseReleaseEvent(QMouseEvent *e)
{
    QListView::mouseReleaseEvent(e);
}

void DesktopIconView::mouseDoubleClickEvent(QMouseEvent *event)
{
    QListView::mouseDoubleClickEvent(event);
    m_real_do_edit = false;
}

void DesktopIconView::dragEnterEvent(QDragEnterEvent *e)
{
    m_real_do_edit = false;
    if (e->keyboardModifiers() && Qt::ControlModifier)
        m_ctrl_key_pressed = true;
    else
        m_ctrl_key_pressed = false;

    auto action = m_ctrl_key_pressed ? Qt::CopyAction : Qt::MoveAction;
    qDebug()<<"drag enter event" <<action;
    if (e->mimeData()->hasUrls()) {
        e->setDropAction(action);
        e->acceptProposedAction();
    }

    if (e->source() == this) {
        m_drag_indexes = selectedIndexes();
    }
}

void DesktopIconView::dragMoveEvent(QDragMoveEvent *e)
{
    m_real_do_edit = false;
    if (e->keyboardModifiers() && Qt::ControlModifier)
        m_ctrl_key_pressed = true;
    else
        m_ctrl_key_pressed = false;

    auto action = m_ctrl_key_pressed ? Qt::CopyAction : Qt::MoveAction;
    auto index = indexAt(e->pos());
    if (index.isValid() && index != m_last_index) {
        QHoverEvent he(QHoverEvent::HoverMove, e->posF(), e->posF());
        viewportEvent(&he);
    } else {
        QHoverEvent he(QHoverEvent::HoverLeave, e->posF(), e->posF());
        viewportEvent(&he);
    }
    if (e->isAccepted())
        return;
    qDebug()<<"drag move event" <<action;
    if (this == e->source()) {
        e->accept();
        return QListView::dragMoveEvent(e);
    }
    e->setDropAction(action);
    e->accept();
}

void DesktopIconView::dropEvent(QDropEvent *e)
{
    m_real_do_edit = false;
    //qDebug()<<"drop event";
    /*!
      \todo
      fix the bug that move drop action can not move the desktop
      item to correct position.

      i use copy action to avoid this bug, but the drop indicator
      is incorrect.
      */
    m_edit_trigger_timer.stop();
    if (e->keyboardModifiers() && Qt::ControlModifier)
        m_ctrl_key_pressed = true;
    else
        m_ctrl_key_pressed = false;

    auto action = /*m_ctrl_key_pressed ? Qt::CopyAction : */Qt::MoveAction;
    qDebug() << "DesktopIconView dropEvent" <<action;
    if (this == e->source() && !m_ctrl_key_pressed)
    {
        auto index = indexAt(e->pos());
        qDebug() <<"DesktopIconView index:" <<index <<index.isValid();
        bool bmoved = false;
        if (index.isValid()) {
            auto info = FileInfo::fromUri(index.data(Qt::UserRole).toString());
            if (!info->isDir())
                return;
            bmoved = true;
        }

        if (bmoved)
        {
            //move file to desktop folder
            qDebug() << "DesktopIconView move file to folder";
            m_model->dropMimeData(e->mimeData(), action, -1, -1, this->indexAt(e->pos()));
        }
        else
            QListView::dropEvent(e);

        QHash<QModelIndex, QRect> currentIndexesRects;
        for (int i = 0; i < m_proxy_model->rowCount(); i++) {
            auto tmp = m_proxy_model->index(i, 0);
            currentIndexesRects.insert(tmp, QListView::visualRect(tmp));
        }

        //fixme: handle overlapping.
        if (!m_drag_indexes.isEmpty()) {
            QModelIndexList overlappedIndexes;
            QModelIndexList unoverlappedIndexes = m_drag_indexes;
            for (auto value : currentIndexesRects.values()) {
                auto keys = currentIndexesRects.keys(value);
                if (keys.count() > 1) {
                    for (auto key : keys) {
                        if (m_drag_indexes.contains(key) && !overlappedIndexes.contains(key)) {
                            overlappedIndexes<<key;
                            unoverlappedIndexes.removeOne(key);
                        }
                    }
                }
            }

            for (auto index : unoverlappedIndexes) {
                // save pos
                QTimer::singleShot(1, this, [=](){
                    setFileMetaInfoPos(index.data(Qt::UserRole).toString(), QListView::visualRect(index).topLeft());
                });
            }

            auto grid = this->gridSize();
            auto viewRect = this->rect();

            QRegion notEmptyRegion;
            for (auto rect : currentIndexesRects) {
                notEmptyRegion += rect;
            }

            for (auto dragedIndex : overlappedIndexes) {
                auto indexRect = QListView::visualRect(dragedIndex);
                if (notEmptyRegion.contains(indexRect.center())) {
                    // move index to closest empty grid.
                    auto next = indexRect;
                    bool isEmptyPos = false;
                    while (!isEmptyPos) {
                        next.translate(0, grid.height());
                        if (next.bottom() > viewRect.bottom()) {
                            int top = next.y();
                            while (true) {
                                if (top < gridSize().height()) {
                                    break;
                                }
                                top-=gridSize().height();
                            }
                            //put item to next column first column
                            next.moveTo(next.x() + grid.width(), top);
                        }
                        if (notEmptyRegion.contains(next.center()))
                            continue;

                        isEmptyPos = true;
                        setPositionForIndex(next.topLeft(), dragedIndex);
                        setFileMetaInfoPos(dragedIndex.data(Qt::UserRole).toString(), next.topLeft());
                        notEmptyRegion += next;
                    }
                }
            }

            // check if there is any item out of view
            for (auto index : m_drag_indexes) {
                auto indexRect = QListView::visualRect(index);
                if (this->viewport()->rect().contains(indexRect)) {
                    continue;
                }

                // try relocating invisible item

                QRect next;
                for (auto existedRect : notEmptyRegion.rects()) {
                    if (this->viewport()->rect().contains(existedRect)) {
                        next = existedRect;
                        break;
                    }
                }
                while (next.translated(-grid.width(), 0).x() >= 0) {
                    next.translate(-grid.width(), 0);
                }
                while (next.translated(0, -grid.height()).top() >= 0) {
                    next.translate(0, -grid.height());
                }

                while (notEmptyRegion.contains(next.center())) {
                    next.translate(0, grid.height());
                    if (next.bottom() > viewRect.bottom()) {
                        int top = next.y();
                        while (true) {
                            if (top < gridSize().height()) {
                                break;
                            }
                            top-=gridSize().height();
                        }
                        //put item to next column first column
                        next.moveTo(next.x() + grid.width(), top);
                    }
                }

                setPositionForIndex(next.topLeft(), index);
                setFileMetaInfoPos(index.data(Qt::UserRole).toString(), next.topLeft());
                notEmptyRegion += next;
            }
        }

        m_drag_indexes.clear();

        auto urls = e->mimeData()->urls();
        for (auto url : urls) {
//            if (url.path() == QStandardPaths::writableLocation(QStandardPaths::HomeLocation))
//                continue;
            saveItemPositionInfo(url.toDisplayString());
        }
        return;
    }
    m_model->dropMimeData(e->mimeData(), action, -1, -1, this->indexAt(e->pos()));
    //FIXME: save item position
}

const QFont DesktopIconView::getViewItemFont(QStyleOptionViewItem *item)
{
    return item->font;
//    auto font = item->font;
//    if (font.pixelSize() <= 0) {
//        font = QApplication::font();
//    }
//    switch (zoomLevel()) {
//    case DesktopIconView::Small:
//        font.setPixelSize(int(font.pixelSize() * 0.8));
//        break;
//    case DesktopIconView::Large:
//        font.setPixelSize(int(font.pixelSize() * 1.2));
//        break;
//    case DesktopIconView::Huge:
//        font.setPixelSize(int(font.pixelSize() * 1.4));
//        break;
//    default:
//        break;
//    }
//    return font;
}

void DesktopIconView::clearAllIndexWidgets()
{
    if (!model())
        return;

    int row = 0;
    auto index = model()->index(row, 0);
    while (index.isValid()) {
        setIndexWidget(index, nullptr);
        row++;
        index = model()->index(row, 0);
    }
}

void DesktopIconView::refresh()
{
    this->setCursor(QCursor(Qt::WaitCursor));
//    if (m_refresh_timer.isActive())
//        return;

    if (!m_model)
        return;

    if (m_is_refreshing)
        return;
    m_is_refreshing = true;
    m_model->refresh();

    //m_refresh_timer.start();
}

QRect DesktopIconView::visualRect(const QModelIndex &index) const
{
    auto rect = QListView::visualRect(index);
    QPoint p(10, 5);

    switch (zoomLevel()) {
    case Small:
        p *= 0.8;
        break;
    case Large:
        p *= 1.2;
        break;
    case Huge:
        p *= 1.4;
        break;
    default:
        break;
    }
    rect.moveTo(rect.topLeft() + p);
    return rect;
}
