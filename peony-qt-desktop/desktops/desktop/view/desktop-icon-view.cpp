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

#include "file-item-model.h"
#include "file-info-job.h"
#include "file-launch-manager.h"
#include <QProcess>

#include <QDesktopServices>

#include "desktop-index-widget.h"

#include "file-meta-info.h"

#include "global-settings.h"
#include "audio-play-manager.h"

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
#include <QToolTip>
#include <QGSettings>
#include <QDrag>
#include <QMimeData>
#include <QPixmap>
#include <QPainter>
#include <QGraphicsDropShadowEffect>

#include <QtX11Extras/QX11Info>
#include <kstartupinfo.h>

using namespace Peony;

#define ITEM_POS_ATTRIBUTE "metadata::peony-qt-desktop-item-position"
#define PANEL_SETTINGS "org.ukui.panel.settings"
#define UKUI_STYLE_SETTINGS "org.ukui.style"

static bool iconSizeLessThan (const QPair<QRect, QString> &p1, const QPair<QRect, QString> &p2);

static bool initialized = false;

DesktopIconView::DesktopIconView(QWidget *parent) : QListView(parent)
{
    //m_refresh_timer.setInterval(500);
    //m_refresh_timer.setSingleShot(true);
    setAttribute(Qt::WA_AlwaysStackOnTop);
    installEventFilter(this);

    //解决桌面可以拖动的问题
    setProperty("notUseSlideGesture", true);

    initShoutCut();
    //initMenu();
    initDoubleClick();

    connect(qApp, &QApplication::paletteChanged, this, [=]() {
        viewport()->update();
    });

    m_edit_trigger_timer.setSingleShot(true);
    m_edit_trigger_timer.setInterval(3000);
    m_last_index = QModelIndex();

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

    setSelectionMode(QListView::ExtendedSelection);

    auto zoomLevel = this->zoomLevel();
    setDefaultZoomLevel(zoomLevel);

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
            if (!initialized) {
                initialized = true;

                if (!QGSettings::isSchemaInstalled(PANEL_SETTINGS))
                    return;
                //panel
                QGSettings *panelSetting = new QGSettings(PANEL_SETTINGS, QByteArray(), this);
                int position = panelSetting->get("panelposition").toInt();
                int margins = panelSetting->get("panelsize").toInt();
                switch (position) {
                case 1: {
                    setViewportMargins(0, margins, 0, 0);
                    break;
                }
                case 2: {
                    setViewportMargins(margins, 0, 0, 0);
                    break;
                }
                case 3: {
                    setViewportMargins(0, 0, margins, 0);
                    break;
                }
                default: {
                    setViewportMargins(0, 0, 0, margins);
                    break;
                }
                }
                resolutionChange();
            }

            if (isItemsOverlapped()) {
                // refresh again?
                //this->refresh();
                QStringList needRelayoutItems;
                QRegion notEmptyRegion;
                for (auto value : m_item_rect_hash.values()) {
                    auto keys = m_item_rect_hash.keys(value);
                    if (keys.count() > 1) {
                        keys.pop_front();
                        for (auto key : keys) {
                            needRelayoutItems.append(key);
                            m_item_rect_hash.remove(key);
                        }
                    }
                    notEmptyRegion += value;
                }

                int gridWidth = gridSize().width();
                int gridHeight = gridSize().height();
                // aligin exsited rect
                int marginTop = notEmptyRegion.boundingRect().top();
                while (marginTop - gridHeight > 0) {
                    marginTop -= gridHeight;
                }
                int marginLeft = notEmptyRegion.boundingRect().left();
                while (marginLeft - gridWidth > 0) {
                    marginLeft -= gridWidth;
                }
                marginLeft = marginLeft < 0? 0: marginLeft;
                marginTop = marginTop < 0? 0: marginTop;
                int posX = marginLeft;
                int posY = marginTop;
                for (auto item : needRelayoutItems) {
                    QRect itemRect = QRect(posX, posY, gridWidth, gridHeight);
                    while (notEmptyRegion.contains(itemRect.center())) {
                        if (posY + 2*gridHeight > this->viewport()->height()) {
                            posY = marginTop;
                            posX += gridWidth;
                        } else {
                            posY += gridHeight;
                        }
                        if (this->viewport()->geometry().contains(itemRect.topLeft())) {
                            itemRect.moveTo(posX, posY);
                        } else {
                            itemRect.moveTo(0, 0);
                        }
                    }
                    notEmptyRegion += itemRect;
                    m_item_rect_hash.insert(item, itemRect);
                }
                for (auto uri : m_item_rect_hash.keys()) {
                    auto rect = m_item_rect_hash.value(uri);
                    updateItemPosByUri(uri, rect.topLeft());
                    setFileMetaInfoPos(uri, rect.topLeft());
                }
                this->saveAllItemPosistionInfos();
            }

            // check icon is out of screen
            auto geo = viewport()->rect();
            if (geo.width() != 0 && geo.height() != 0) {
                for (auto rec : m_item_rect_hash.values()) {
                    if (!geo.contains(rec)) {
                        resolutionChange();
                        break;
                    }
                }
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

        auto geo = viewport()->rect();
        if (geo.width() != 0 && geo.height() != 0) {
            QModelIndex index = m_proxy_model->mapToSource(m_model->indexFromUri(uri));
            QRect indexRect = QListView::visualRect(index);
            if (!geo.contains(indexRect)) {
                resolutionChange();
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

    connect(m_proxy_model, &DesktopItemProxyModel::showHiddenFile, this, [=]() {
        QTimer::singleShot(100, this, [=]() {
            resetAllItemPositionInfos();
            refresh();
        });
    });

    connect(this, &QListView::iconSizeChanged, this, [=]() {
        //qDebug()<<"save=============";
        this->setSortType(GlobalSettings::getInstance()->getValue(LAST_DESKTOP_SORT_ORDER).toInt());

        QTimer::singleShot(100, this, [=]() {
            bool isFull = false;
            auto geo = viewport()->rect();
            this->saveAllItemPosistionInfos();
            for (int i = 0; i < m_proxy_model->rowCount(); i++) {
                auto index = m_proxy_model->index(i, 0);
                m_item_rect_hash.insert(index.data(Qt::UserRole).toString(), QListView::visualRect(index));
                updateItemPosByUri(index.data(Qt::UserRole).toString(), QListView::visualRect(index).topLeft());

                if (geo.width() != 0 && geo.height() != 0) {
                    if (!geo.contains(QListView::visualRect(index))) {
                        isFull = true;
                    }
                }
            }
            if (isFull) resolutionChange();
        });
    });

    setModel(m_proxy_model);
    //m_proxy_model->sort(0);

    m_peonyDbusSer = new PeonyDbusService(this);
    m_peonyDbusSer->DbusServerRegister();

    setMouseTracking(true);//追踪鼠标

    this->refresh();

    //fix task bar overlap with desktop icon and can drag move issue
    //bug #27811,33188
    if (QGSettings::isSchemaInstalled(PANEL_SETTINGS))
    {
        //panel monitor
        QGSettings *panelSetting = new QGSettings(PANEL_SETTINGS, QByteArray(), this);
        connect(panelSetting, &QGSettings::changed, this, [=](const QString &key){
            if (key == "panelposition" || key == "panelsize")
            {
                int position = panelSetting->get("panelposition").toInt();
                int margins = panelSetting->get("panelsize").toInt();
                switch (position) {
                case 1: {
                    setViewportMargins(0, margins, 0, 0);
                    break;
                }
                case 2: {
                    setViewportMargins(margins, 0, 0, 0);
                    break;
                }
                case 3: {
                    setViewportMargins(0, 0, margins, 0);
                    break;
                }
                default: {
                    setViewportMargins(0, 0, 0, margins);
                    break;
                }
                }
            }
            if (initialized)
                resolutionChange();
        });
    }

    // try fixing #63358
    if (QGSettings::isSchemaInstalled(UKUI_STYLE_SETTINGS)) {
        auto styleSettings = new QGSettings(UKUI_STYLE_SETTINGS, QByteArray(), this);
        connect(styleSettings, &QGSettings::changed, this, [=](const QString &key){
            if (key == "iconThemeName") {
                QTimer::singleShot(1000, viewport(), [=]{
                    viewport()->update();
                });
            }
        });
    }
//该阴影效果应用于整个view，会导致桌面全部操作都存在阴影效果，引出很多bug
//    QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect;
//    shadowEffect->setBlurRadius(20);
//    shadowEffect->setColor(QColor(63, 63, 63, 180));
//    shadowEffect->setOffset(0,0);
//    setGraphicsEffect(shadowEffect);

//    m_animation = new QVariantAnimation(this);
//    m_animation->setDuration(250);
//    m_animation->setKeyValueAt(0,1.0);
//    m_animation->setKeyValueAt(0.5,1.1);
//    m_animation->setKeyValueAt(1,1.0);
//    m_animation->setEndValue(1.0);

//    connect(m_animation,&QVariantAnimation::valueChanged,this,[=](){
//        viewport()->update();
//    });
}

DesktopIconView::~DesktopIconView()
{
    delete m_peonyDbusSer;
    //saveAllItemPosistionInfos();
}

bool DesktopIconView::eventFilter(QObject *obj, QEvent *e)
{
    //fixme:
    //comment to fix change night style refresh desktop issue
    if (e->type() == QEvent::StyleChange || e->type() == QEvent::ApplicationFontChange || e->type() == QEvent::FontChange) {
        auto type = e->type();
        if (m_model) {
            for (auto uri : getAllFileUris()) {
                auto pos = getFileMetaInfoPos(uri);
                if (pos.x() >= 0)
                    updateItemPosByUri(uri, pos);
            }
        }
    }
    return false;
}

static bool meetSpecialConditions(const QStringList& selectedUris)
{
    /* The desktop home directory, computer, and trash do not allow operations such as copying, cutting,
     * deleting, renaming, moving, or using shortcut keys for corresponding operations.add by 2021/06/17 */
    static QString homeUri = "file://" +  QStandardPaths::writableLocation(QStandardPaths::HomeLocation);

    if (selectedUris.contains("computer:///")
       ||selectedUris.contains("trash:///")
       ||selectedUris.contains(homeUri)){
        return true;
    }

    return false;
}

void DesktopIconView::initShoutCut()
{
    QAction *copyAction = new QAction(this);
    copyAction->setShortcut(QKeySequence::Copy);
    connect(copyAction, &QAction::triggered, [=]() {
        auto selectedUris = this->getSelections();
        if (!selectedUris.isEmpty() && !meetSpecialConditions(selectedUris)){
            ClipboardUtils::setClipboardFiles(selectedUris, false);}
    });
    addAction(copyAction);

    QAction *cutAction = new QAction(this);
    cutAction->setShortcut(QKeySequence::Cut);
    connect(cutAction, &QAction::triggered, [=]() {
        auto selectedUris = this->getSelections();
        if (!selectedUris.isEmpty() && !meetSpecialConditions(selectedUris))
        {
            ClipboardUtils::setClipboardFiles(selectedUris, true);
            this->update();
        }
    });
    addAction(cutAction);

    QAction *pasteAction = new QAction(this);
    pasteAction->setShortcut(QKeySequence::Paste);
    connect(pasteAction, &QAction::triggered, [=]() {
        auto clipUris = ClipboardUtils::getClipboardFilesUris();
        if (ClipboardUtils::isClipboardHasFiles() && !meetSpecialConditions(this->getSelections())) {
            ClipboardUtils::pasteClipboardFiles(this->getDirectoryUri());
        }
    });
    addAction(pasteAction);

    //add CTRL+D for delete operation
    auto trashAction = new QAction(this);
    trashAction->setShortcuts(QList<QKeySequence>()<<Qt::Key_Delete<<QKeySequence(Qt::CTRL + Qt::Key_D));
    connect(trashAction, &QAction::triggered, [=]() {
        auto selectedUris = getSelections();
        if (!selectedUris.isEmpty() && !meetSpecialConditions(selectedUris)){
           FileOperationUtils::trash(selectedUris, true);
        }
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
        if (selections.count() == 1 && !meetSpecialConditions(selections)) {
            this->editUri(selections.first());
        }
    });
    addAction(renameAction);

    QAction *removeAction = new QAction(this);
    removeAction->setShortcut(QKeySequence(Qt::SHIFT + Qt::Key_Delete));
    connect(removeAction, &QAction::triggered, [=]() {
        auto selectedUris = this->getSelections();
        if (!meetSpecialConditions(selectedUris)){
            qDebug() << "delete" << selectedUris;
            clearAllIndexWidgets();
            FileOperationUtils::executeRemoveActionWithDialog(selectedUris);
        }
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
        DesktopMenu menu(this);
        if (this->getSelections().count() > 0)
        {
            menu.showProperties(this->getSelections());
        }
        else
        {
            QString desktopPath = "file://" +  QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
            menu.showProperties(desktopPath);
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
        if (this->zoomLevel() == DesktopIconView::Normal)
            return;
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
    m_show_hidden = settings->isExist(SHOW_HIDDEN_PREFERENCE)? settings->getValue(SHOW_HIDDEN_PREFERENCE).toBool(): false;
    //show hidden action
    QAction *showHiddenAction = new QAction(this);
    showHiddenAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_H));
    addAction(showHiddenAction);
    connect(showHiddenAction, &QAction::triggered, this, [=]() {
        this->setShowHidden();
    });

    auto cancelAction = new QAction(this);
    cancelAction->setShortcut(Qt::Key_Escape);
    connect(cancelAction, &QAction::triggered, [=]() {
        if (Peony::ClipboardUtils::isClipboardHasFiles())
        {
            Peony::ClipboardUtils::clearClipboard();
            this->update();
        }
    });
    addAction(cancelAction);
}

void DesktopIconView::initMenu()
{
    return;
    setContextMenuPolicy(Qt::CustomContextMenu);

    // menu
    connect(this, &QListView::customContextMenuRequested, this,
    [=](const QPoint &pos) {
        // FIXME: use other menu
        qDebug() << "menu request  in desktop icon view";
        if (!this->indexAt(pos).isValid()) {
            this->clearSelection();
        } else {
            this->clearSelection();
            this->selectionModel()->select(this->indexAt(pos), QItemSelectionModel::Select);
        }

        QTimer::singleShot(1, this, [=]() {
            DesktopMenu menu(this);
            if (this->getSelections().isEmpty()) {
                auto action = menu.addAction(tr("set background"));
                connect(action, &QAction::triggered, [=]() {
                    //go to control center set background
                    PeonyDesktopApplication::gotoSetBackground();
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
    m_show_hidden = !GlobalSettings::getInstance()->getValue(SHOW_HIDDEN_PREFERENCE).toBool();
    m_proxy_model->setShowHidden(m_show_hidden);
    //fix show hidden file desktop icons overlapped issue
    QTimer::singleShot(100, this, [=]() {
        resetAllItemPositionInfos();
        refresh();
    });
}

void DesktopIconView::resolutionChange()
{
    QSize screenSize = this->viewport()->size();

    // do not relayout items while screen size is empty.
    if (screenSize.isEmpty())
        return;

    float iconWidth = 0;
    float iconHeigth = 0;

    // icon size
    QSize icon = gridSize();
    iconWidth = icon.width();
    iconHeigth = icon.height();

    QRect screenRect = QRect(0, 0, screenSize.width(), screenSize.height());

    if (!m_item_rect_hash.isEmpty()) {
        QList<QPair<QRect, QString>> newPosition;

        for (auto i = m_item_rect_hash.constBegin(); i != m_item_rect_hash.constEnd(); ++i) {
            newPosition << QPair<QRect, QString>(i.value(), i.key());
        }

        // not get current size
        if (iconWidth == 0 || iconHeigth == 0) {
            qDebug() << "Unable to get icon size, need to get it another way!";
            return;
        }

    //    qDebug() << "icon width: " << iconWidth << " icon heigth: " << iconHeigth;
    //    qDebug() << "width:" << screenSize.width() << " height:" << screenSize.height();

        std::stable_sort(newPosition.begin(), newPosition.end(), iconSizeLessThan);

        //m_item_rect_hash.clear();

        // only reset items over viewport.
        QRegion notEmptyRegion;
        QList<QPair<QRect, QString>> needChanged;
        for (auto pair : newPosition) {
            if (!screenRect.contains(pair.first)) {
                needChanged.append(pair);
                // remember item position before resolution changed.
                m_resolution_item_rect.insert(pair.second, m_item_rect_hash.value(pair.second));
                m_item_rect_hash.remove(pair.second);
            } else {
                notEmptyRegion += pair.first;
            }
        }

        // aligin exsited rect
        int marginTop = notEmptyRegion.boundingRect().top();
        while (marginTop - iconHeigth > 0) {
            marginTop -= iconHeigth;
        }
        int marginLeft = notEmptyRegion.boundingRect().left();
        while (marginLeft - iconWidth > 0) {
            marginLeft -= iconWidth;
        }
        marginTop = marginTop < 0? 0: marginTop;
        marginLeft = marginLeft < 0? 0: marginLeft;

        if (!needChanged.isEmpty()) {
            int posX = marginLeft;
            int posY = marginTop;

            for (int i = 0; i < needChanged.count(); i++) {
                bool placeOK = false;
                while (notEmptyRegion.contains(QPoint(posX + iconWidth/2, posY + iconHeigth/2))) {
                    if (posY + 2 * iconHeigth > screenSize.height()) {
                        posY = marginTop;
                        posX += iconWidth;
                    } else {
                        posY += iconHeigth;
                    }
                }
                QRect newRect = QRect(QPoint(posX, posY), gridSize());
                if (posX + iconWidth > screenSize.width()) {
                    newRect.moveTo(0, 0);
                }
                m_item_rect_hash.insert(needChanged.at(i).second, newRect);
                notEmptyRegion += newRect;
            }
        } else {
            // re-layout overlayed items
            for (auto pair : newPosition) {
                if (QRect(0, 0, 10, 10).contains(pair.first.topLeft())) {
                    needChanged.append(pair);
                }
            }
//            // first item doesn't need re-layout
            if (!needChanged.isEmpty())
                needChanged.removeFirst();

            int posX = marginLeft;
            int posY = marginTop;
            for (int i = 0; i < needChanged.count(); i++) {
                while (notEmptyRegion.contains(QPoint(posX + iconWidth/2, posY + iconHeigth/2))) {
                    if (posY + iconHeigth * 2 > screenSize.height()) {
                        posY = marginTop;
                        posX += iconWidth;
                    } else {
                        posY += iconHeigth;
                    }
                }
                QRect newRect = QRect(QPoint(posX, posY), gridSize());
                if (posX + iconWidth > screenSize.width()) {
                    newRect.moveTo(0, 0);
                }
                notEmptyRegion += newRect;
                m_item_rect_hash.insert(needChanged.at(i).second, newRect);
            }

            // try restore items which's positions changed by resolution changed.
            QStringList needRelayoutItems2;
            for (auto uri : m_resolution_item_rect.keys()) {
                auto rect = m_resolution_item_rect.value(uri);
                if (screenRect.contains(rect)) {
                    m_item_rect_hash.insert(uri, rect);
                } else {
                    // need be relayout.
                    needRelayoutItems2<<uri;
                }
            }
            relayoutExsitingItems(needRelayoutItems2);
        }
    }

    for (auto uri : m_item_rect_hash.keys()) {
        auto rect = m_item_rect_hash.value(uri);
        updateItemPosByUri(uri, rect.topLeft());
        setFileMetaInfoPos(uri, rect.topLeft());
    }

    this->saveAllItemPosistionInfos();
}

void DesktopIconView::openFileByUri(QString uri)
{
    auto info = FileInfo::fromUri(uri);
    auto job = new FileInfoJob(info);
    job->setAutoDelete();
    job->connect(job, &FileInfoJob::queryAsyncFinished, [=]() {
        if ((info->isDir() || info->isVolume() || info->isVirtual())) {
            QDir dir(info->filePath());
            if (! dir.exists())
            {
                Peony::AudioPlayManager::getInstance()->playWarningAudio();
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
                Peony::AudioPlayManager::getInstance()->playWarningAudio();
                QMessageBox::critical(nullptr, tr("Open failed"),
                                      tr("Open directory failed, you have no permission!"));
                return;
            }

#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
            QProcess p;
            QUrl url = uri;
            p.setProgram("peony");
            p.setArguments(QStringList() << url.toEncoded() <<"%U&");
            qint64 pid;
            p.startDetached(&pid);

            // send startinfo to kwindowsystem
            quint32 timeStamp = QX11Info::isPlatformX11() ? QX11Info::appUserTime() : 0;
            KStartupInfoId startInfoId;
            startInfoId.initId(KStartupInfo::createNewStartupIdForTimestamp(timeStamp));
            startInfoId.setupStartupEnv();
            KStartupInfoData data;
            data.setHostname();
            data.addPid(pid);
            QRect rect = info.get()->property("iconGeometry").toRect();
            if (rect.isValid())
                data.setIconGeometry(rect);
            data.setLaunchedBy(getpid());
            KStartupInfo::sendStartup(startInfoId, data);
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
    connect(this, &QListView::activated, this, [=](const QModelIndex &index) {
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

            QRect rect(mapToGlobal(indexRect.topLeft()), indexRect.size());
            FileInfo::fromUri(index.data(Qt::UserRole).toString()).get()->setProperty("iconGeometry", rect);
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

    m_resolution_item_rect.clear();
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

            FileInfo::fromUri(index.data(Qt::UserRole).toString()).get()->setProperty("iconGeometry", QVariant());
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
                if (top >= 0 && left >= 0) {
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

    QRect rect(mapToGlobal(pos), QListView::visualRect(index).size());
    FileInfo::fromUri(uri).get()->setProperty("iconGeometry", rect);

    auto metaInfo = FileMetaInfo::fromUri(uri);
    if (metaInfo) {
        QStringList topLeft;
        topLeft<<QString::number(pos.y());
        topLeft<<QString::number(pos.x());
        metaInfo->setMetaInfoStringList(ITEM_POS_ATTRIBUTE, topLeft);
    }
}

QMap<QString, QRect> DesktopIconView::getCurrentItemRects()
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
    m_resolution_item_rect.clear();
    m_item_rect_hash.clear();
    //resetAllItemPositionInfos();
    m_proxy_model->setSortType(sortType);
    m_proxy_model->sort(1);
    m_proxy_model->sort(0, m_proxy_model->sortOrder());
    saveAllItemPosistionInfos();
    bool isFull = false;
    auto geo = viewport()->rect();
    if (geo.width() != 0 && geo.height() != 0) {
        for (int i = 0; i < m_proxy_model->rowCount(); i++) {
            auto index = m_proxy_model->index(i, 0);
            m_item_rect_hash.insert(index.data(Qt::UserRole).toString(), QListView::visualRect(index));
            updateItemPosByUri(index.data(Qt::UserRole).toString(), QListView::visualRect(index).topLeft());
            if (!geo.contains(QListView::visualRect(index))) {
                isFull = true;
            }
        }

        if (isFull) {
            resolutionChange();
        }
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

void DesktopIconView::scrollTo(const QModelIndex &index, QAbstractItemView::ScrollHint hint)
{
    return;
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
                auto indexWidget = new DesktopIndexWidget(delegate, viewOptions(), upIndex, this);
                setIndexWidget(upIndex, indexWidget);
                indexWidget->move(visualRect(upIndex).topLeft());

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
                auto indexWidget = new DesktopIndexWidget(delegate, viewOptions(), downIndex, this);
                setIndexWidget(downIndex, indexWidget);
                indexWidget->move(visualRect(downIndex).topLeft());

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
                auto indexWidget = new DesktopIndexWidget(delegate, viewOptions(), leftIndex, this);
                setIndexWidget(leftIndex, indexWidget);
                indexWidget->move(visualRect(leftIndex).topLeft());
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
                auto indexWidget = new DesktopIndexWidget(delegate, viewOptions(), rightIndex, this);
                setIndexWidget(rightIndex, indexWidget);
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

    if (initialized)
        resolutionChange();
}

void DesktopIconView::rowsInserted(const QModelIndex &parent, int start, int end)
{
    m_model->relayoutAddedItems();
    QListView::rowsInserted(parent, start, end);
    for (auto uri : getAllFileUris()) {
        auto pos = getFileMetaInfoPos(uri);
        if (pos.x() >= 0) {
            updateItemPosByUri(uri, pos);
        }
    }
    // try fix item overrlapped sometimes, link to #58739
    if (start == end) {
        auto index = model()->index(start, 0);
        auto uri = index.data(Qt::UserRole).toString();
        auto itemRectHash = m_item_rect_hash;
        itemRectHash.remove(uri);
        QRegion notEmptyRegion;
        QSize itemRectSize = itemRectHash.first().size();
        for (auto rect : itemRectHash.values()) {
            notEmptyRegion += rect;
        }

        auto itemRect = QRect(m_item_rect_hash.value(uri).topLeft(), itemRectSize);
        auto itemCenter = itemRect.center();
        if (notEmptyRegion.contains(itemCenter)) {
            // handle overlapped
            qWarning()<<"unexpected overrlapped happend";
            qDebug()<<"check item rect hash"<<m_item_rect_hash;
            QStringList fakeList;
            fakeList<<uri;
            relayoutExsitingItems(fakeList);
        }

        if (uri == m_model->m_renaming_file_pos.first || uri == m_model->m_renaming_file_pos.first + ".desktop") {
            updateItemPosByUri(uri, m_model->m_renaming_file_pos.second);
        } else if (m_model->m_renaming_operation_info.get()) {
            if (m_model->m_renaming_operation_info.get()->target() == uri) {
                updateItemPosByUri(uri, m_model->m_renaming_file_pos.second);
            }
        }
    }
    clearAllIndexWidgets();
}

void DesktopIconView::rowsAboutToBeRemoved(const QModelIndex &parent, int start, int end)
{
    m_model->relayoutAddedItems();
    QListView::rowsAboutToBeRemoved(parent, start, end);
//    QTimer::singleShot(1, this, [=](){
//        for (auto uri : getAllFileUris()) {
//            auto pos = getFileMetaInfoPos(uri);
//            if (pos.x() >= 0)
//                updateItemPosByUri(uri, pos);
//        }
//    });

    for (int row = start; row <= end; row++) {
        auto uri = model()->index(row, 0).data(Qt::UserRole).toString();
        m_resolution_item_rect.remove(uri);
    }

    clearAllIndexWidgets();
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

void DesktopIconView::relayoutExsitingItems(const QStringList &uris)
{
    if (uris.isEmpty()) {
        return;
    }
    auto allFileUris = getAllFileUris();

    auto ensuredItemRectHash = m_item_rect_hash;
    for (auto uri : uris) {
        ensuredItemRectHash.remove(uri);
    }

    QRegion notEmptyRegion;
    for (auto rect : ensuredItemRectHash.values()) {
        notEmptyRegion += rect;
    }

    auto grid = this->gridSize();
    auto viewRect = this->viewport()->rect();

    // aligin exsited rect
    int marginTop = notEmptyRegion.boundingRect().top();
    while (marginTop - grid.height() > 0) {
        marginTop -= grid.height();
    }

    int marginLeft = notEmptyRegion.boundingRect().left();
    while (marginLeft - grid.width() > 0) {
        marginLeft -= grid.width();
    }
    marginLeft = marginLeft < 0? 0: marginLeft;
    marginTop = marginTop < 0? 0: marginTop;

    for (auto uri : uris) {
        if (!allFileUris.contains(uri))
            continue;
        auto indexRect = QRect(QPoint(marginLeft, marginTop), m_item_rect_hash.values().first().size());
        if (notEmptyRegion.contains(indexRect.center())) {

            // move index to closest empty grid.
            auto next = indexRect;
            bool isEmptyPos = false;
            while (!isEmptyPos) {
                next.translate(0, grid.height());
                if (next.bottom() > viewRect.bottom()) {
                    int top = next.y();
                    while (true) {
                        if (top < grid.height()) {
                            break;
                        }
                        top-=grid.height();
                    }
                    //put item to next column first row
                    next.moveTo(next.x() + grid.width(), top);
                }
                if (notEmptyRegion.contains(next.center()))
                    continue;

                isEmptyPos = true;
                m_item_rect_hash.insert(uri, next);
                notEmptyRegion += next;

                setFileMetaInfoPos(uri, next.topLeft());
            }
        } else {
            setFileMetaInfoPos(uri, indexRect.topLeft());
        }
    }
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
        setGridSize(QSize(5, 5) + itemDelegate()->sizeHint(viewOptions(), QModelIndex()));
        break;
    case Large:
        setIconSize(QSize(64, 64));
        setGridSize(QSize(15, 15) + itemDelegate()->sizeHint(viewOptions(), QModelIndex()));
        break;
    case Huge:
        setIconSize(QSize(96, 96));
        setGridSize(QSize(20, 20) + itemDelegate()->sizeHint(viewOptions(), QModelIndex()));
        break;
    default:
        m_zoom_level = Normal;
        setIconSize(QSize(48, 48));
        setGridSize(QSize(10, 10) + itemDelegate()->sizeHint(viewOptions(), QModelIndex()));
        break;
    }
    clearAllIndexWidgets();
    auto metaInfo = FileMetaInfo::fromUri("computer:///");
    if (metaInfo) {
        qDebug()<<"set zoom level"<<m_zoom_level;
        metaInfo->setMetaInfoInt("peony-qt-desktop-zoom-level", int(m_zoom_level));
    } else {

    }

    GlobalSettings::getInstance()->setGSettingValue(DEFAULT_DESKTOP_ZOOM_LEVEL, m_zoom_level);

    resetAllItemPositionInfos();
    if (m_model) {
        m_model->clearFloatItems();
    }
}

DesktopIconView::ZoomLevel DesktopIconView::zoomLevel() const
{
    //FIXME:
    if (m_zoom_level != Invalid)
        return m_zoom_level;

    bool ok = false;
    int zl = GlobalSettings::getInstance()->getValue(DEFAULT_DESKTOP_ZOOM_LEVEL).toInt(&ok);
    if (ok) {
        return ZoomLevel(zl);
    }

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

void DesktopIconView::setEditFlag(bool edit)
{
    m_is_edit = edit;
}

bool DesktopIconView::getEditFlag()
{
    return m_is_edit;
}

int DesktopIconView::verticalOffset() const
{
    return 0;
}

int DesktopIconView::horizontalOffset() const
{
    return 0;
}

void DesktopIconView::restoreItemsPosByMetaInfo()
{
    for (auto uri : getAllFileUris()) {
        auto pos = getFileMetaInfoPos(uri);
        if (pos.x() >= 0) {
            updateItemPosByUri(uri, pos);
        }
    }
}

void DesktopIconView::mousePressEvent(QMouseEvent *e)
{
    m_press_pos = e->pos();
    // bug extend selection bug
    m_real_do_edit = false;

    if (e->modifiers() & Qt::ControlModifier)
        m_ctrl_key_pressed = true;
    else
        m_ctrl_key_pressed = false;

    if (!m_ctrl_or_shift_pressed) {
        if (!indexAt(e->pos()).isValid()) {
            clearAllIndexWidgets();
            clearSelection();
        } else {
            auto index = indexAt(e->pos());
            m_last_index = index;
            //fix rename state has no menuRequest issue, bug#44107
            if (! m_is_edit)
            {
                clearAllIndexWidgets();
                //force to recreate new DesktopIndexWidget, to fix not show name issue
                if (indexWidget(m_last_index))
                    setIndexWidget(m_last_index, nullptr);
                auto indexWidget = new DesktopIndexWidget(qobject_cast<DesktopIconViewDelegate *>(itemDelegate()), viewOptions(), m_last_index);
                setIndexWidget(m_last_index,
                               indexWidget);
                indexWidget->move(visualRect(m_last_index).topLeft());
                //m_animation->start();
            }
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
            for (auto uri : getAllFileUris()) {
                auto pos = getFileMetaInfoPos(uri);
                if (pos.x() >= 0)
                    updateItemPosByUri(uri, pos);
            }
#endif
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

    this->viewport()->update(viewport()->rect());
}

void DesktopIconView::mouseMoveEvent(QMouseEvent *e)
{
    QModelIndex itemIndex = indexAt(e->pos());
    if (!itemIndex.isValid()) {
        if (QToolTip::isVisible()) {
            QToolTip::hideText();
        }
    }

    QListView::mouseMoveEvent(e);
}

void DesktopIconView::mouseDoubleClickEvent(QMouseEvent *event)
{
    QListView::mouseDoubleClickEvent(event);
    m_real_do_edit = false;
}

void DesktopIconView::dragEnterEvent(QDragEnterEvent *e)
{
    m_real_do_edit = false;

    auto action = m_ctrl_key_pressed ? Qt::CopyAction : Qt::MoveAction;
    qDebug()<<"drag enter event" <<action;
    if (e->mimeData()->hasUrls()) {
        if (FileUtils::containsStandardPath(e->mimeData()->urls())) {
            e->ignore();
            return;
        }
        e->setDropAction(action);
        e->accept();
        //e->acceptProposedAction();
    }

    if (e->source() == this) {
        m_drag_indexes = selectedIndexes();
    }
}

void DesktopIconView::dragMoveEvent(QDragMoveEvent *e)
{
    m_real_do_edit = false;
    if (e->keyboardModifiers() & Qt::ControlModifier)
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
    m_model->clearFloatItems();
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
    if (e->keyboardModifiers() & Qt::ControlModifier) {
        m_ctrl_key_pressed = true;
    } else {
        m_ctrl_key_pressed = false;
    }

    auto action = m_ctrl_key_pressed ? Qt::CopyAction : Qt::MoveAction;
    if (e->keyboardModifiers() & Qt::ShiftModifier) {
        action = Qt::TargetMoveAction;
    }
    qDebug() << "DesktopIconView dropEvent" <<action;
    auto index = indexAt(e->pos());
    if (index.isValid() || m_ctrl_key_pressed)
    {
        qDebug() <<"DesktopIconView index copyAction:";
        auto urls = e->mimeData()->urls();
        QString homePath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
        QStringList uris;
        for (auto url : urls)
        {
            if (url.toString() == "computer:///")
                uris << "computer:///";
            else
                uris << url.path();
        }

        //fix can drag copy home folder issue, link to bug#64824
        if (uris.contains(homePath) || uris.contains("computer:///"))
            return;
    }

    if (this == e->source() && !m_ctrl_key_pressed) {
        qDebug() <<"DesktopIconView index:" <<index <<index.isValid();
        bool bmoved = false;
        if (index.isValid()) {
            auto uri = m_model->indexUri(m_proxy_model->mapToSource(index));
            auto info = FileInfo::fromUri(index.data(Qt::UserRole).toString());
            if (!info->isDir()||e->mimeData()->urls().contains(uri)) {
                return;
            }
            bmoved = true;
        }

        if (bmoved) {
            //move file to desktop folder
            qDebug() << "DesktopIconView move file to folder";
            for (auto uuri : e->mimeData()->urls()) {
                if ("trash:///" == uuri.toDisplayString() || "computer:///" == uuri.toDisplayString()) {
                    return;
                }
            }

            m_model->dropMimeData(e->mimeData(), action, -1, -1, this->indexAt(e->pos()));
        } else {
            // do not trigger file operation, link to: #66345
            m_model->setAcceptDropAction(false);
            QListView::dropEvent(e);
            m_model->setAcceptDropAction(true);
        }

        QRegion dirtyRegion;
        QHash<QModelIndex, QRect> currentIndexesRects;
        for (int i = 0; i < m_proxy_model->rowCount(); i++) {
            auto tmp = m_proxy_model->index(i, 0);
            currentIndexesRects.insert(tmp, QListView::visualRect(tmp));
            if (!m_drag_indexes.contains(tmp)) {
                dirtyRegion += QListView::visualRect(tmp);
            }
        }

        //fixme: handle overlapping.
        if (!m_drag_indexes.isEmpty()) {
            // remove info from resolution item rect map.
            for (auto index : m_drag_indexes) {
                m_resolution_item_rect.remove(index.data(Qt::UserRole).toString());
            }

            QModelIndexList overlappedIndexes;
            QModelIndexList unoverlappedIndexes = m_drag_indexes;

            for (auto index : unoverlappedIndexes) {
                QRect visualRect = QListView::visualRect(index);
                if (dirtyRegion.contains(visualRect.center())) {
                    unoverlappedIndexes.removeOne(index);
                    overlappedIndexes.append(index);
                }
            }

            for (auto index : unoverlappedIndexes) {
                // save pos
                QTimer::singleShot(1, this, [=]() {
                    setFileMetaInfoPos(index.data(Qt::UserRole).toString(), QListView::visualRect(index).topLeft());
                });
            }

            auto grid = this->gridSize();
            auto viewRect = this->viewport()->rect();

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
                        if (notEmptyRegion.contains(next.center())) {
                            continue;
                        }

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
                QRect next(0, 0, 0, 0);
                for (auto existedRect = notEmptyRegion.begin(); existedRect != notEmptyRegion.end(); ++existedRect) {
                    if (this->viewport()->rect().contains(*existedRect)) {
                        next = *existedRect;
                        break;
                    } else if (existedRect == notEmptyRegion.end() - 1
                               && next == QRect(0, 0, 0, 0)) {
                        next = *existedRect;
                        next.moveTo(0, 0);
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

void DesktopIconView::startDrag(Qt::DropActions supportedActions)
{
    auto indexes = selectedIndexes();
    if (indexes.count() > 0) {
        auto pos = m_press_pos;
        qreal scale = 1.0;
        QWidget *window = this->window();
        if (window) {
            auto windowHandle = window->windowHandle();
            if (windowHandle) {
                scale = windowHandle->devicePixelRatio();
            }
        }

        auto drag = new QDrag(this);
        drag->setMimeData(model()->mimeData(indexes));

        QRegion rect;
        QHash<QModelIndex, QRect> indexRectHash;
        for (auto index : indexes) {
            rect += (visualRect(index));
            indexRectHash.insert(index, visualRect(index));
        }

        QRect realRect = rect.boundingRect();

        // fix #78263, text displayment is not completed.
        //realRect.adjust(-5, -5, 5, 5);
        realRect.adjust(-15, -15, 15, 15);

        QPixmap pixmap(realRect.size() * scale);
        pixmap.fill(Qt::transparent);
        pixmap.setDevicePixelRatio(scale);
        QPainter painter(&pixmap);
        for (auto index : indexes) {
            painter.save();
            painter.translate(indexRectHash.value(index).topLeft() - rect.boundingRect().topLeft());
            itemDelegate()->paint(&painter, viewOptions(), index);
            painter.restore();
        }

        drag->setPixmap(pixmap);
        drag->setHotSpot(pos - rect.boundingRect().topLeft() - QPoint(viewportMargins().left(), viewportMargins().top()));
        drag->setDragCursor(QPixmap(), m_ctrl_key_pressed? Qt::CopyAction: Qt::MoveAction);
        drag->exec(m_ctrl_key_pressed? Qt::CopyAction: Qt::MoveAction);

    } else {
        return QListView::startDrag(Qt::MoveAction|Qt::CopyAction);
    }
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

void DesktopIconView::clearAllIndexWidgets(const QStringList &uris)
{
    if (!model())
        return;

    int row = 0;
    auto index = model()->index(row, 0);
    while (index.isValid()) {
        if (uris.isEmpty() || uris.contains(index.data(Qt::UserRole).toString())) {
            setIndexWidget(index, nullptr);
        }
        row++;
        index = model()->index(row, 0);
    }

    // avoid dirty region out of index visual rect.
    // link to: #77272.
    viewport()->update();
}

void DesktopIconView::refresh()
{
    this->setCursor(QCursor(Qt::WaitCursor));
//    if (m_refresh_timer.isActive())
//        return;
    Peony::ClipboardUtils::clearClipboard();/* Refresh clear cut status */
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

    auto size = itemDelegate()->sizeHint(QStyleOptionViewItem(), index);
    rect.setSize(size);

    return rect;
}

int DesktopIconView::updateBWList()
{
    m_proxy_model->updateBlackAndWriteLists();
    /*
    * 重新按照既定规则排序，这样可以避免出现空缺和图标重叠的情况
    */
    int sortType = GlobalSettings::getInstance()->getValue(LAST_DESKTOP_SORT_ORDER).toInt();
    setSortType(sortType);
    return 0;
}

static bool iconSizeLessThan (const QPair<QRect, QString>& p1, const QPair<QRect, QString>& p2)
{
    if (p1.first.x() > p2.first.x())
        return false;

    if (p1.first.x() < p2.first.x())
        return true;

    if ((p1.first.x() == p2.first.x()))
        return p1.first.y() < p2.first.y();

    return true;
}
