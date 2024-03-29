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

#include "main-window.h"
#include "header-bar.h"
#include "global-settings.h"

#include "border-shadow-effect.h"

#include <QVariant>
#include <QMouseEvent>
#include <QX11Info>

#include <QDockWidget>
#include <QTreeView>

#include <QScreen>

#include "side-bar-proxy-filter-sort-model.h"
#include "side-bar-model.h"

#include "directory-view-container.h"
#include "tab-widget.h"
#include "x11-window-manager.h"
#include "properties-window.h"
#include "preview-page-factory-manager.h"
#include "preview-page-plugin-iface.h"

#include "navigation-side-bar.h"
#include "advance-search-bar.h"
#include "status-bar.h"

#include "intel/intel-navigation-side-bar.h"

#include "peony-main-window-style.h"

#include "file-label-box.h"
#include "file-operation-manager.h"
#include "file-operation-utils.h"
#include "file-utils.h"
#include "create-template-operation.h"
#include "file-operation-error-dialog.h"
#include "clipboard-utils.h"
#include "search-vfs-uri-parser.h"

#include "directory-view-menu.h"
#include "directory-view-widget.h"
#include "main-window-factory.h"
#include "thumbnail-manager.h"

#include "peony-application.h"

#include "global-settings.h"
#include "audio-play-manager.h"

#include "side-bar-factory-manager.h"

#include <QSplitter>

#include <QPainter>

#include <QDir>
#include <QStandardPaths>
#include <QMessageBox>
#include <QTimer>
#include <QDesktopServices>

#include <QProcess>
#include <QDateTime>

#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
#include <QPainterPath>
#endif

#include <QDebug>

#include <X11/Xlib.h>
#include <KWindowEffects>

// NOTE build failed on Archlinux. Can't detect `QGSettings/QGSettings' header
// fixed by replaced `QGSettings/QGSettings' with `QGSettings'
#include <QGSettings>
#include "xatom-helper.h"
#include "trash-warn-dialog.h"

#define FONT_SETTINGS "org.ukui.style"

static MainWindow *last_resize_window = nullptr;

MainWindow::MainWindow(const QString &uri, QWidget *parent) : QMainWindow(parent)
{
    setContextMenuPolicy(Qt::CustomContextMenu);
    installEventFilter(this);

    setWindowIcon(QIcon::fromTheme("system-file-manager"));
    //setWindowTitle(tr("File Manager"));

    //check all settings and init
    checkSettings();

    setStyle(PeonyMainWindowStyle::getStyle());

    setAnimated(false);
    setAttribute(Qt::WA_DeleteOnClose);
    setAttribute(Qt::WA_TranslucentBackground);
    //setAttribute(Qt::WA_OpaquePaintEvent);
    //fix double window base buttons issue, not effect MinMax button hints
    auto flags = windowFlags() &~Qt::WindowMinMaxButtonsHint;
    setWindowFlags(flags);

    //use ukui-kwin to draw round corner and shadow.
    MotifWmHints hints;
    hints.flags = MWM_HINTS_FUNCTIONS|MWM_HINTS_DECORATIONS;
    hints.functions = MWM_FUNC_ALL;
    hints.decorations = MWM_DECOR_BORDER;
    XAtomHelper::getInstance()->setWindowMotifHint(window()->winId(), hints);
    setContentsMargins(0, 0, 0, 0);

    //disable style window manager
    setProperty("useStyleWindowManager", false);

    //short cut settings
    setShortCuts();

    //init UI
    initUI(uri);

    auto start_cost_time = QDateTime::currentMSecsSinceEpoch()- PeonyApplication::peony_start_time;
    qDebug() << "peony start end in main-window time:" <<start_cost_time
             <<"ms"<<QDateTime::currentMSecsSinceEpoch();
}

MainWindow::~MainWindow()
{
    if (last_resize_window == this) {
        auto settings = Peony::GlobalSettings::getInstance();
        settings->setValue(DEFAULT_WINDOW_WIDTH, this->size().width());
        settings->setValue(DEFAULT_WINDOW_HEIGHT, this->size().height());
        last_resize_window = nullptr;
    }
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::MouseMove) {
        auto me = static_cast<QMouseEvent *>(event);
        auto widget = this->childAt(me->pos());
        if (!widget) {
            // set default sidebar width flag
            m_should_save_side_bar_width = true;
        }
    }

    if (event->type() == QEvent::MouseButtonRelease) {
        if (m_should_save_side_bar_width) {
            // real set default side bar width
            auto settings = Peony::GlobalSettings::getInstance();
            settings->setValue(DEFAULT_SIDEBAR_WIDTH, m_side_bar->width());
        }
        m_should_save_side_bar_width = false;
    }

    return false;
}

QSize MainWindow::sizeHint() const
{
    auto screenSize = QApplication::primaryScreen()->size();
    QSize defaultSize(Peony::GlobalSettings::getInstance()->getValue(DEFAULT_WINDOW_WIDTH).toInt(),
                      Peony::GlobalSettings::getInstance()->getValue(DEFAULT_WINDOW_HEIGHT).toInt());
    int width = qMin(defaultSize.width(), screenSize.width());
    int height = qMin(defaultSize.height(), screenSize.height());
    //return screenSize*2/3;
    //qreal dpr = qApp->devicePixelRatio();
    return QSize(width, height);
}

Peony::FMWindowIface *MainWindow::create(const QString &uri)
{
    auto window = new MainWindow(uri);
    if (currentViewSupportZoom())
        window->setCurrentViewZoomLevel(this->currentViewZoomLevel());
    return window;
}

Peony::FMWindowIface *MainWindow::create(const QStringList &uris)
{
    if (uris.isEmpty()) {
        auto window = new MainWindow;
        if (currentViewSupportZoom())
            window->setCurrentViewZoomLevel(this->currentViewZoomLevel());
        return window;
    }
    auto uri = uris.first();
    auto l = uris;
    l.removeAt(0);
    auto window = new MainWindow(uri);
    if (currentViewSupportZoom())
        window->setCurrentViewZoomLevel(this->currentViewZoomLevel());
    window->addNewTabs(l);
    return window;
}

Peony::FMWindowIface *MainWindow::createWithZoomLevel(const QString &uri, int zoomLevel)
{
    auto window = new MainWindow(uri);
    if (currentViewSupportZoom())
        window->setCurrentViewZoomLevel(zoomLevel);
    return window;
}

Peony::FMWindowIface *MainWindow::createWithZoomLevel(const QStringList &uris, int zoomLevel)
{
    if (uris.isEmpty()) {
        auto window = new MainWindow;
        if (currentViewSupportZoom())
            window->setCurrentViewZoomLevel(zoomLevel);
        return window;
    }
    auto uri = uris.first();
    auto l = uris;
    l.removeAt(0);
    auto window = new MainWindow(uri);
    if (currentViewSupportZoom())
        window->setCurrentViewZoomLevel(zoomLevel);
    window->addNewTabs(l);
    return window;
}

Peony::FMWindowFactory *MainWindow::getFactory()
{
    return MainWindowFactory::getInstance();
}

Peony::DirectoryViewContainer *MainWindow::getCurrentPage()
{
    return m_tab->currentPage();
}

void MainWindow::checkSettings()
{
    auto settings = Peony::GlobalSettings::getInstance();
    m_show_hidden_file = settings->isExist(SHOW_HIDDEN_PREFERENCE)? settings->getValue(SHOW_HIDDEN_PREFERENCE).toBool(): false;
    m_use_default_name_sort_order = settings->isExist(SORT_CHINESE_FIRST)? settings->getValue(SORT_CHINESE_FIRST).toBool(): false;
    m_folder_first = settings->isExist(SORT_FOLDER_FIRST)? settings->getValue(SORT_FOLDER_FIRST).toBool(): true;

    if (QGSettings::isSchemaInstalled("org.ukui.style"))
    {
        //font monitor
        QGSettings *fontSetting = new QGSettings(FONT_SETTINGS, QByteArray(), this);
        connect(fontSetting, &QGSettings::changed, this, [=](const QString &key){
            qDebug() << "fontSetting changed:" << key;
            if (key == "systemFont" || key == "systemFontSize")
            {
                QFont font = this->font();
                for(auto widget : qApp->allWidgets())
                    widget->setFont(font);
            }
        });
    }
}

void MainWindow::setShortCuts()
{
    //stop loading action
    QAction *stopLoadingAction = new QAction(this);
    stopLoadingAction->setShortcut(QKeySequence(Qt::Key_Escape));
    addAction(stopLoadingAction);
    connect(stopLoadingAction, &QAction::triggered, this, &MainWindow::forceStopLoading);

    //show hidden action
    QAction *showHiddenAction = new QAction(this);
    showHiddenAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_H));
    addAction(showHiddenAction);
    connect(showHiddenAction, &QAction::triggered, this, [=]() {
        //qDebug() << "show hidden";
        this->setShowHidden();
    });

    auto undoAction = new QAction(QIcon::fromTheme("edit-undo-symbolic"), tr("Undo"), this);
    undoAction->setShortcut(QKeySequence::Undo);
    addAction(undoAction);
    connect(undoAction, &QAction::triggered, [=]() {
        Peony::FileOperationManager::getInstance()->undo();
    });

    auto redoAction = new QAction(QIcon::fromTheme("edit-redo-symbolic"), tr("Redo"), this);
    redoAction->setShortcut(QKeySequence::Redo);
    addAction(redoAction);
    connect(redoAction, &QAction::triggered, [=]() {
        Peony::FileOperationManager::getInstance()->redo();
    });

        //add CTRL+D for delete operation
        auto trashAction = new QAction(this);
        trashAction->setShortcuts(QList<QKeySequence>()<<Qt::Key_Delete<<QKeySequence(Qt::CTRL + Qt::Key_D));
        connect(trashAction, &QAction::triggered, [=]() {
            auto currentUri = getCurrentUri();
            if (currentUri.startsWith("search://")
                    || currentUri.startsWith("favorite://") || currentUri == "filesafe:///"
                    || currentUri.startsWith("kmre://") || currentUri.startsWith("kydroid://"))
                return;

        auto uris = this->getCurrentSelections();
        QString desktopPath = "file://" +  QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
        QString desktopUri = Peony::FileUtils::getEncodedUri(desktopPath);
        QString homeUri = "file://" +  QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
        if (!uris.isEmpty() && !uris.contains(desktopUri) && !uris.contains(homeUri)) {
            bool isTrash = this->getCurrentUri() == "trash:///";
            if (!isTrash) {
                Peony::FileOperationUtils::trash(uris, true);
            } else {
                Peony::FileOperationUtils::executeRemoveActionWithDialog(uris);
            }
        }
    });
    addAction(trashAction);

        auto deleteAction = new QAction(this);
        deleteAction->setShortcuts(QList<QKeySequence>()<<QKeySequence(Qt::SHIFT + Qt::Key_Delete));
        addAction(deleteAction);
        connect(deleteAction, &QAction::triggered, [=]() {
            auto currentUri = getCurrentUri();
            if (currentUri.startsWith("search://") || currentUri == "filesafe:///"
                    || currentUri.startsWith("kmre://") || currentUri.startsWith("kydroid://"))
                return;

        auto uris = this->getCurrentSelections();
        QString desktopPath = "file://" +  QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
        QString desktopUri = Peony::FileUtils::getEncodedUri(desktopPath);
        QString homeUri = "file://" +  QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
        if (! uris.contains(desktopUri) && !uris.contains(homeUri))
           Peony::FileOperationUtils::executeRemoveActionWithDialog(uris);
    });

    auto searchAction = new QAction(this);
    searchAction->setShortcuts(QList<QKeySequence>()<<QKeySequence(Qt::CTRL + Qt::Key_F));
    connect(searchAction, &QAction::triggered, this, [=]() {
        m_is_search = ! m_is_search;
        m_header_bar->startEdit(m_is_search);
    });
    addAction(searchAction);

    //F4 or Alt+D, change to address
    auto locationAction = new QAction(this);
    locationAction->setShortcuts(QList<QKeySequence>()<<Qt::Key_F4<<QKeySequence(Qt::ALT + Qt::Key_D));
    connect(locationAction, &QAction::triggered, this, [=]() {
        m_header_bar->startEdit();
    });
    addAction(locationAction);

    auto newWindowAction = new QAction(this);
    newWindowAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_N));
    connect(newWindowAction, &QAction::triggered, this, [=]() {
        MainWindow *newWindow = new MainWindow(getCurrentUri());
        newWindow->show();
    });
    addAction(newWindowAction);

    auto closeWindowAction = new QAction(this);
    closeWindowAction->setShortcuts(QList<QKeySequence>()<<QKeySequence(Qt::ALT + Qt::Key_F4));
    connect(closeWindowAction, &QAction::triggered, this, [=]() {
        this->close();
    });
    addAction(closeWindowAction);

    auto aboutAction = new QAction(this);
    aboutAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_F2));
    connect(aboutAction, &QAction::triggered, this, [=]() {
        PeonyApplication::about();
    });
    addAction(aboutAction);

    auto newTabAction = new QAction(this);
    newTabAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_T));
    connect(newTabAction, &QAction::triggered, this, [=]() {
        this->addNewTabs(QStringList()<<this->getCurrentUri());
    });
    addAction(newTabAction);

    auto closeTabAction = new QAction(this);
    closeTabAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_W));
    connect(closeTabAction, &QAction::triggered, this, [=]() {
        if (m_tab->count() <= 1) {
            this->close();
        } else {
            m_tab->removeTab(m_tab->currentIndex());
        }
    });
    addAction(closeTabAction);

    auto nextTabAction = new QAction(this);
    nextTabAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Tab));
    connect(nextTabAction, &QAction::triggered, this, [=]() {
        int currentIndex = m_tab->currentIndex();
        if (currentIndex + 1 < m_tab->count()) {
            m_tab->setCurrentIndex(currentIndex + 1);
        } else {
            m_tab->setCurrentIndex(0);
        }
    });
    addAction(nextTabAction);

    auto previousTabAction = new QAction(this);
    previousTabAction->setShortcuts(QList<QKeySequence>()<<QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_Tab));
    connect(previousTabAction, &QAction::triggered, this, [=]() {
        int currentIndex = m_tab->currentIndex();
        if (currentIndex > 0) {
            m_tab->setCurrentIndex(currentIndex - 1);
        } else {
            m_tab->setCurrentIndex(m_tab->count() - 1);
        }
    });
    addAction(previousTabAction);

    auto newFolderAction = new QAction(this);
    newFolderAction->setShortcuts(QList<QKeySequence>()<<QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_N));
    connect(newFolderAction, &QAction::triggered, this, &MainWindow::createFolderOperation);
    addAction(newFolderAction);

    //show selected item's properties
    auto propertiesWindowAction = new QAction(this);
    propertiesWindowAction->setShortcuts(QList<QKeySequence>()<<QKeySequence(Qt::ALT + Qt::Key_Return)
                                         <<QKeySequence(Qt::ALT + Qt::Key_Enter));
    connect(propertiesWindowAction, &QAction::triggered, this, [=]() {
        //Fixed issue:when use this shortcut without any selections, this will crash
        QStringList uris;
        if (getCurrentSelections().count() > 0)
        {
            uris<<getCurrentSelections();
        }
        else
        {
            uris<<getCurrentUri();
        }

        Peony::PropertiesWindow *w = new Peony::PropertiesWindow(uris);
        w->setAttribute(Qt::WA_DeleteOnClose);
        w->show();
    });
    addAction(propertiesWindowAction);

    auto helpAction = new QAction(this);
    helpAction->setShortcut(QKeySequence(Qt::Key_F1));
    connect(helpAction, &QAction::triggered, this, [=]() {
        PeonyApplication::help();
    });
    addAction(helpAction);

    auto maxAction = new QAction(this);
        maxAction->setShortcut(QKeySequence(Qt::Key_F11));
        connect(maxAction, &QAction::triggered, this, [=]() {
            //showFullScreen has some issue, change to showMaximized, fix #20043
            m_header_bar->cancelEdit();
            maximizeOrRestore();
        });
        addAction(maxAction);

//    auto previewPageAction = new QAction(this);
//    previewPageAction->setShortcuts(QList<QKeySequence>()<<Qt::Key_F3<<QKeySequence(Qt::ALT + Qt::Key_P));
//    connect(previewPageAction, &QAction::triggered, this, [=]() {
//        auto triggered = m_tab->getTriggeredPreviewPage();
//        if (triggered)
//        {
//            m_tab->setPreviewPage(nullptr);
//        }
//        else
//        {
//            auto instance = Peony::PreviewPageFactoryManager::getInstance();
//            auto lastPreviewPageId  = instance->getLastPreviewPageId();
//            auto *page = instance->getPlugin(lastPreviewPageId)->createPreviewPage();
//            m_tab->setPreviewPage(page);
//        }
//        m_tab->setTriggeredPreviewPage(! triggered);
//    });
//    addAction(previewPageAction);

    auto refreshWindowAction = new QAction(this);
    refreshWindowAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_R));
    connect(refreshWindowAction, &QAction::triggered, this, [=]() {
        this->refresh();
    });
    addAction(refreshWindowAction);

    auto listToIconViewAction = new QAction(this);
    listToIconViewAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_1));
    connect(listToIconViewAction, &QAction::triggered, this, [=]() {
        this->beginSwitchView(QString("Icon View"));
    });
    addAction(listToIconViewAction);

    auto iconToListViewAction = new QAction(this);
    iconToListViewAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_2));
    connect(iconToListViewAction, &QAction::triggered, this, [=]() {
        this->beginSwitchView(QString("List View"));
    });
    addAction(iconToListViewAction);

    auto reverseSelectAction = new QAction(this);
    reverseSelectAction->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_L));
    connect(reverseSelectAction, &QAction::triggered, this, [=]() {
        this->getCurrentPage()->getView()->invertSelections();
    });
    addAction(reverseSelectAction);

    auto remodelViewAction = new QAction(this);
    remodelViewAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_0));
    connect(remodelViewAction, &QAction::triggered, this, [=]() {
        this->getCurrentPage()->setZoomLevelRequest(25);
    });
    addAction(remodelViewAction);

    auto enlargViewAction = new QAction(this);
    enlargViewAction->setShortcut(QKeySequence::ZoomIn);
    connect(enlargViewAction, &QAction::triggered, this, [=]() {
        int defaultZoomLevel = this->currentViewZoomLevel();
        if(defaultZoomLevel <= 95){ defaultZoomLevel+=5; }
        for (int i = 0; i < 5; i++) {
            this->getCurrentPage()->setZoomLevelRequest(defaultZoomLevel);
        }

    });
    addAction(enlargViewAction);

    auto shrinkViewAction = new QAction(this);
    shrinkViewAction->setShortcut(QKeySequence::ZoomOut);
    connect(shrinkViewAction, &QAction::triggered, this, [=]() {
        int defaultZoomLevel = this->currentViewZoomLevel();
        if(defaultZoomLevel > 6){ defaultZoomLevel-=5; }
        this->getCurrentPage()->setZoomLevelRequest(defaultZoomLevel);
    });
    addAction(shrinkViewAction);

    auto quitAllAction = new QAction(this);
    quitAllAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q));
    connect(quitAllAction, &QAction::triggered, this, [=]() {
        QProcess p(0);
        p.start("peony", QStringList()<<"-q");
        p.waitForStarted();
        p.waitForFinished();
    });
    addAction(quitAllAction);

    auto refreshAction = new QAction(this);
    refreshAction->setShortcut(Qt::Key_F5);
    connect(refreshAction, &QAction::triggered, this, [=]() {
        this->refresh();
    });
    addAction(refreshAction);

    //file operations
    auto *copyAction = new QAction(this);
    copyAction->setShortcut(QKeySequence::Copy);
    connect(copyAction, &QAction::triggered, [=]() {
        if (!this->getCurrentSelections().isEmpty())
        {
            if (this->getCurrentSelections().first().startsWith("trash://", Qt::CaseInsensitive)) {
                return ;
            }
            if (this->getCurrentSelections().first().startsWith("recent://", Qt::CaseInsensitive)) {
                return ;
            }
            if (this->getCurrentSelections().first().startsWith("favorite://", Qt::CaseInsensitive)) {
                return ;
            }
        }
        Peony::ClipboardUtils::setClipboardFiles(this->getCurrentSelections(), false);
    });
    addAction(copyAction);

    auto *pasteAction = new QAction(this);
    pasteAction->setShortcut(QKeySequence::Paste);
    connect(pasteAction, &QAction::triggered, [=]() {
        auto currentUri = getCurrentUri();
        if (currentUri.startsWith("trash://") || currentUri.startsWith("recent://")
            || currentUri.startsWith("computer://") || currentUri.startsWith("favorite://")
            || currentUri.startsWith("search://"))
        {
            return;
        }
        if (Peony::ClipboardUtils::isClipboardHasFiles()) {
            //FIXME: how about duplicated copy?
            //FIXME: how to deal with a failed move?
            auto op = Peony::ClipboardUtils::pasteClipboardFiles(this->getCurrentUri());
            if (op) {
                connect(op, &Peony::FileOperation::operationFinished, this, [=](){
                    auto opInfo = op->getOperationInfo();
                    auto targetUirs = opInfo->dests();
                    QTimer::singleShot(500, this, [=](){
                        setCurrentSelectionUris(targetUirs);
                    });
//                    setCurrentSelectionUris(targetUirs);
                }, Qt::BlockingQueuedConnection);
            }
        }
    });
    addAction(pasteAction);

    auto *cutAction = new QAction(this);
    cutAction->setShortcut(QKeySequence::Cut);
    connect(cutAction, &QAction::triggered, [=]() {
        if (!this->getCurrentSelections().isEmpty()) {
            if (this->getCurrentSelections().first().startsWith("trash://", Qt::CaseInsensitive)) {
                return ;
            }
            if (this->getCurrentSelections().first().startsWith("recent://", Qt::CaseInsensitive)) {
                return ;
            }
            if (this->getCurrentSelections().first().startsWith("favorite://", Qt::CaseInsensitive)) {
                return ;
            }

            auto currentUri = getCurrentUri();
            if (currentUri.startsWith("trash://") || currentUri.startsWith("recent://")
                || currentUri.startsWith("computer://") || currentUri.startsWith("favorite://")
                || currentUri.startsWith("search://") || currentUri == "filesafe:///")
            {
                return;
            }

            QString desktopPath = "file://" +  QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
            QString desktopUri = Peony::FileUtils::getEncodedUri(desktopPath);
            QString homeUri = "file://" +  QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
            if (! this->getCurrentSelections().contains(desktopUri) && ! this->getCurrentSelections().contains(homeUri))
                Peony::ClipboardUtils::setClipboardFiles(this->getCurrentSelections(), true);
        }
    });
    addAction(cutAction);
}

void MainWindow::updateTabPageTitle()
{
    m_tab->updateTabPageTitle();
    //FIXME: replace BLOCKING api in ui thread.
    auto curUri = getCurrentUri();
    auto show = Peony::FileUtils::getFileDisplayName(curUri);
    QString title = show;// + "-" + tr("File Manager");
    if (curUri.startsWith("search:///"))
        title = tr("Search");
    if (title.length() <= 0)
        title = tr("File Manager");
    //qDebug() << "updateTabPageTitle:" <<title;
    setWindowTitle(title);
}

void MainWindow::createFolderOperation()
{
//    Peony::CreateTemplateOperation op(getCurrentUri(), Peony::CreateTemplateOperation::EmptyFolder, tr("New Folder"));
//    Peony::FileOperationErrorDialogConflict dlg;
//    connect(&op, &Peony::FileOperation::errored, &dlg, &Peony::FileOperationErrorDialogConflict::handle);
//    op.run();
//    auto targetUri = op.target();

    auto op = Peony::FileOperationUtils::create(getCurrentUri(), tr("New Folder"), Peony::CreateTemplateOperation::EmptyFolder);
    connect(op, &Peony::FileOperation::operationFinished, this, [=](){
        if (op->hasError())
            return;
        auto opInfo = op->getOperationInfo();
        //auto targetUri = opInfo->target();
        this->getCurrentPage()->getView()->clearIndexWidget();
        //set a short time delay, fix bug#86070, select two folders
        QTimer::singleShot(10, this, [=](){
            this->editUri(opInfo->target());
        });
    }, Qt::BlockingQueuedConnection);

//    QTimer::singleShot(500, this, [=]() {
//        this->getCurrentPage()->getView()->scrollToSelection(targetUri);
//        this->editUri(targetUri);
//    });
}

void MainWindow::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Backspace)
    {
        auto uri = Peony::FileUtils::getParentUri(getCurrentUri());
        //qDebug() << "goUp Action" << getCurrentUri() << uri;
        if (uri.isNull())
            return;
        m_tab->goToUri(uri, true, true);
    }

    if (e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return)
    {
        auto selections = this->getCurrentSelections();
        //if select only one item, let view to process
        if (selections.count() > 1)
        {
            QStringList files;
            QStringList dirs;
            for (auto uri : selections) {
                auto info = Peony::FileInfo::fromUri(uri);
                if (info->isDir() || info->isVolume()) {
                    dirs<<uri;
                } else {
                    files<<uri;
                }
            }
            for (auto uri : dirs) {
                m_tab->addPage(uri);
            }
            for (auto uri : files) {
                Q_EMIT m_tab->currentPage()->viewDoubleClicked(uri);
            }
        }
    }

    return QMainWindow::keyPressEvent(e);
}

const QString MainWindow::getCurrentUri()
{
    return m_tab->getCurrentUri();
}

const QStringList MainWindow::getCurrentSelections()
{
    return m_tab->getCurrentSelections();
}

const QStringList MainWindow::getCurrentAllFileUris()
{
    return m_tab->getAllFileUris();
}

Qt::SortOrder MainWindow::getCurrentSortOrder()
{
    return m_tab->getSortOrder();
}

int MainWindow::getCurrentSortColumn()
{
    return m_tab->getSortType();
}

int MainWindow::currentViewZoomLevel()
{
    if (getCurrentPage()) {
        if (auto view = getCurrentPage()->getView()) {
            return view->currentZoomLevel();
        }
    }

    int defaultZoomLevel = Peony::GlobalSettings::getInstance()->getValue(DEFAULT_VIEW_ZOOM_LEVEL).toInt();

    if (defaultZoomLevel >= 0 && defaultZoomLevel <= 100) {
        return defaultZoomLevel;
    }

    return m_tab->m_status_bar->m_slider->value();
}

bool MainWindow::currentViewSupportZoom()
{
    if (getCurrentPage()) {
        if (auto view = getCurrentPage()->getView()) {
            return view->supportZoom();
        }
    }
    return m_tab->m_status_bar->m_slider->isEnabled();
}

void MainWindow::maximizeOrRestore()
{
    if (!this->isMaximized()) {
        this->showMaximized();
    } else {
        this->showNormal();
    }
    //maybe not need update icons? comment to try fix bug#77966
    //m_header_bar->updateIcons();
    m_header_bar->updateMaximizeState();
}

void MainWindow::syncControlsLocation(const QString &uri)
{
    m_tab->goToUri(uri, false, false);
}

void MainWindow::updateHeaderBar()
{
    m_header_bar->setLocation(getCurrentUri());
    m_header_bar->updateIcons();
    //fix bug#82685
    m_header_bar->updateSortTypeEnable();
    //fix bug#66336, 83711
    m_header_bar->updateViewTypeEnable();
    //m_status_bar->update();
    m_header_bar->updatePreviewPageVisible();
}

void MainWindow::updateWindowIcon()
{
   auto currentUri = getCurrentUri();
   if (currentUri.startsWith("trash://"))
   {
       QIcon icon = QIcon::fromTheme("user-trash");
       setWindowIcon(icon);
   }
   else if (currentUri.startsWith("computer://"))
   {
       QIcon icon = QIcon::fromTheme("computer");
       setWindowIcon(icon);
   }
   else
   {
       QIcon icon = QIcon::fromTheme("system-file-manager");
       setWindowIcon(icon);
   }
}

void MainWindow::goToUri(const QString &uri, bool addHistory, bool force)
{
    auto viewId = this->getCurrentPage()->getView()->viewId();

    if (QString::compare(viewId, "Icon View", Qt::CaseSensitive) == 0) {
        this->getCurrentPage()->getView()->disableMultiSelect();
    }

    QUrl url(uri);
    auto realUri = uri;
    if (uri == "computer:///ukui-data-volume") {
        realUri = "file:///data";
    }
    if (url.scheme().isEmpty()) {
        if (uri.startsWith("/")) {
            realUri = "file://" + uri;
        } else {
            QDir currentDir = QDir(QStandardPaths::writableLocation(QStandardPaths::HomeLocation));
            currentDir.cd(uri);
            auto absPath = currentDir.absoluteFilePath(uri);
            url = QUrl::fromLocalFile(absPath);

            realUri = url.toDisplayString();
        }
    }

    if (getCurrentUri() == realUri) {
        if (!force) {
            refresh();
            return;
        }
    }

    locationChangeStart();
    m_tab->goToUri(realUri, addHistory, force);
    m_header_bar->setLocation(uri);
}

void MainWindow::addNewTabs(const QStringList &uris)
{
    //fix search path add new tab,page title show abnormal issue
    if (uris.count() == 1)
    {
        m_tab->addPage(uris.first(), true);
        return;
    }

    for (auto uri : uris) {
        m_tab->addPage(uri, false);
    }
}

void MainWindow::beginSwitchView(const QString &viewId)
{
    //not allow change to other view when in computer, link to bug#83711
    if (getCurrentUri() == "computer:///")
        return;

    auto selection = getCurrentSelections();
//    int sortType = getCurrentSortColumn();
//    Qt::SortOrder sortOrder = getCurrentSortOrder();
    m_tab->switchViewType(viewId);
    // save zoom level
    Peony::GlobalSettings::getInstance()->setValue(DEFAULT_VIEW_ZOOM_LEVEL, currentViewZoomLevel());
    m_tab->setCurrentSelections(selection);
    bool supportZoom = m_tab->currentPage()->getView()->supportZoom();
    m_tab->m_status_bar->m_slider->setEnabled(supportZoom);
//    m_tab->m_status_bar->m_slider->setVisible(supportZoom);
    //fix slider value not update issue
    m_tab->m_status_bar->m_slider->setValue(currentViewZoomLevel());
}

void MainWindow::refresh()
{
    locationChangeStart();
    m_tab->refresh();
    //goToUri(getCurrentUri(), false, true);
}

void MainWindow::advanceSearch()
{
    qDebug()<<"advanceSearch clicked";
    initAdvancePage();
}

void MainWindow::clearRecord()
{
    //qDebug()<<"clearRecord clicked";
//    m_search_bar->clearSearchRecord();
//    m_clear_record->setDisabled(true);
}

void MainWindow::searchFilter(QString target_path, QString keyWord, bool search_file_name, bool search_content)
{
//    auto targetUri = SearchVFSUriParser::parseSearchKey(target_path, keyWord, search_file_name, search_content);
//    //qDebug()<<"targeturi:"<<targetUri;
//    m_update_condition = true;
//    this->goToUri(targetUri, true);
}

void MainWindow::filterUpdate(int type_index, int time_index, int size_index)
{
    //qDebug()<<"filterUpdate:";
    //m_tab->getActivePage()->setSortFilter(type_index, time_index, size_index);
}

void MainWindow::setLabelNameFilter(QString name)
{
    getCurrentPage()->setFilterLabelConditions(name);
}

void MainWindow::setShowHidden()
{
    m_show_hidden_file = !m_show_hidden_file;
    getCurrentPage()->setShowHidden(m_show_hidden_file);
    //显示隐藏文件，更新项目个数
    Q_EMIT m_tab->updateItemsNum();
}

void MainWindow::setUseDefaultNameSortOrder()
{
    m_use_default_name_sort_order = ! m_use_default_name_sort_order;
    getCurrentPage()->setUseDefaultNameSortOrder(m_use_default_name_sort_order);
}

void MainWindow::setSortFolderFirst()
{
    m_folder_first = ! m_folder_first;
    getCurrentPage()->setSortFolderFirst(m_folder_first);
}

void MainWindow::forceStopLoading()
{
    m_tab->stopLoading();
}

void MainWindow::setCurrentSelectionUris(const QStringList &uris)
{
    m_tab->setCurrentSelections(uris);
    //move scrollToSelection to m_tab to try fix new unzip file show two same icon issue
    //Fix me, unknow caused reason
//    if (uris.isEmpty())
//        return;
//    getCurrentPage()->getView()->scrollToSelection(uris.first());
}

void MainWindow::setCurrentSortOrder(Qt::SortOrder order)
{
    m_tab->setSortOrder(order);
}

void MainWindow::setCurrentSortColumn(int sortColumn)
{
    m_tab->setSortType(sortColumn);
}

void MainWindow::editUri(const QString &uri)
{
    m_tab->editUri(uri);
}

void MainWindow::editUris(const QStringList &uris)
{
    m_tab->editUris(uris);
}

void MainWindow::setCurrentViewZoomLevel(int zoomLevel)
{
    if (currentViewSupportZoom())
        m_tab->m_status_bar->m_slider->setValue(zoomLevel);
}

void MainWindow::resizeEvent(QResizeEvent *e)
{
    QMainWindow::resizeEvent(e);
    //may not need update? comment to try fix bug#77966
    //m_header_bar->updateMaximizeState();
    validBorder();
    update();

    last_resize_window = this;
}

/*!
 * \note
 * The window has a noticeable tearing effect due to the drawing of shadow effects.
 * I should consider do not painting a shadow when resizing.
 */
void MainWindow::paintEvent(QPaintEvent *e)
{
    validBorder();
    QColor color = this->palette().window().color();
    QColor colorBase = this->palette().base().color();

    int R1 = color.red();
    int G1 = color.green();
    int B1 = color.blue();
    qreal a1 = 0.3;

    int R2 = colorBase.red();
    int G2 = colorBase.green();
    int B2 = colorBase.blue();
    qreal a2 = 1;

    qreal a = 1 - (1 - a1)*(1 - a2);

    qreal R = (a1*R1 + (1 - a1)*a2*R2) / a;
    qreal G = (a1*G1 + (1 - a1)*a2*G2) / a;
    qreal B = (a1*B1 + (1 - a1)*a2*B2) / a;

    colorBase.setRed(R);
    colorBase.setGreen(G);
    colorBase.setBlue(B);

    auto sidebarOpacity = Peony::GlobalSettings::getInstance()->getValue(SIDEBAR_BG_OPACITY).toInt();

    colorBase.setAlphaF(sidebarOpacity/100.0);

//    if (qApp->property("blurEnable").isValid()) {
//        bool blurEnable = qApp->property("blurEnable").toBool();
//        if (!blurEnable) {
//            colorBase.setAlphaF(1);
//        }
//    } else {
//        colorBase.setAlphaF(1);
//    }

    QPainterPath sidebarPath;
    sidebarPath.setFillRule(Qt::FillRule::WindingFill);

    auto pos = m_tab->mapTo(this, QPoint());
    auto tmpRect = QRect(pos, m_tab->size());
    QPainterPath deletePath;
    QPainterPath tmpPath;

    tmpPath.addRect(rect());

    deletePath.addRoundedRect(tmpRect.adjusted(0, 48, 0, 0), 16, 16);
    deletePath.addRect(rect().width()-18,rect().height()-18,18,18);
    deletePath.addRect(tmpRect.x(),tmpRect.height()-18,18,18);

    sidebarPath = tmpPath - deletePath;


    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing); // 抗锯齿
    p.fillPath(sidebarPath,colorBase);

    QPainter painter(this);
    if(m_is_first_tab)
        deletePath.addRect(m_tab->x(),48,16,16);
    deletePath.setFillRule(Qt::FillRule::WindingFill);
    painter.fillPath(deletePath,this->palette().base());
    QMainWindow::paintEvent(e);
}

void MainWindow::mousePressEvent(QMouseEvent *e)
{
    //qDebug()<<"mouse pressed"<<e;
    QMainWindow::mousePressEvent(e);
    if (e->button() == Qt::LeftButton && !e->isAccepted()) {
        m_is_draging = true;
        m_offset = mapFromGlobal(QCursor::pos());
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *e)
{
    //NOTE: when starting a X11 window move, the mouse move event
    //will unreachable when draging, and after draging we could not
    //get the release event correctly.
    //qDebug()<<"mouse move";
    QMainWindow::mouseMoveEvent(e);
    if (!m_is_draging)
        return;

    qreal  dpiRatio = qApp->devicePixelRatio();
    if (QX11Info::isPlatformX11()) {
        Display *display = QX11Info::display();
        Atom netMoveResize = XInternAtom(display, "_NET_WM_MOVERESIZE", False);
        XEvent xEvent;
        const auto pos = QCursor::pos();

        memset(&xEvent, 0, sizeof(XEvent));
        xEvent.xclient.type = ClientMessage;
        xEvent.xclient.message_type = netMoveResize;
        xEvent.xclient.display = display;
        xEvent.xclient.window = this->winId();
        xEvent.xclient.format = 32;
        xEvent.xclient.data.l[0] = pos.x() * dpiRatio;
        xEvent.xclient.data.l[1] = pos.y() * dpiRatio;
        xEvent.xclient.data.l[2] = 8;
        xEvent.xclient.data.l[3] = Button1;
        xEvent.xclient.data.l[4] = 0;

        XUngrabPointer(display, CurrentTime);
        XSendEvent(display, QX11Info::appRootWindow(QX11Info::appScreen()),
                   False, SubstructureNotifyMask | SubstructureRedirectMask,
                   &xEvent);
        //XFlush(display);

        XEvent xevent;
        memset(&xevent, 0, sizeof(XEvent));

        xevent.type = ButtonRelease;
        xevent.xbutton.button = Button1;
        xevent.xbutton.window = this->winId();
        xevent.xbutton.x = e->pos().x() * dpiRatio;
        xevent.xbutton.y = e->pos().y() * dpiRatio;
        xevent.xbutton.x_root = pos.x() * dpiRatio;
        xevent.xbutton.y_root = pos.y() * dpiRatio;
        xevent.xbutton.display = display;

        XSendEvent(display, this->effectiveWinId(), False, ButtonReleaseMask, &xevent);
        XFlush(display);

        if (e->source() == Qt::MouseEventSynthesizedByQt) {
            if (!this->mouseGrabber()) {
                this->grabMouse();
                this->releaseMouse();
            }
        }

        m_is_draging = false;
    } else {
        this->move((QCursor::pos() - m_offset) * dpiRatio);
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *e)
{
    /*!
     * \bug
     * release event sometimes "disappear" when we request
     * X11 window manager for movement.
     */
    QMainWindow::mouseReleaseEvent(e);
    //qDebug()<<"mouse released";
    m_is_draging = false;
}

void MainWindow::validBorder()
{
    QPainterPath path;
    auto rect = this->rect();
    path.addRect(rect);
    setProperty("blurRegion", QRegion(path.toFillPolygon().toPolygon()));
    //use KWindowEffects
    KWindowEffects::enableBlurBehind(this->winId(), true, QRegion(path.toFillPolygon().toPolygon()));
}
#include "file-utils.h"
void MainWindow::initUI(const QString &uri)
{
    connect(this, &MainWindow::locationChangeStart, this, [=]() {
        m_side_bar->blockSignals(true);
        m_header_bar->blockSignals(true);
        QCursor c;
        c.setShape(Qt::WaitCursor);
        this->setCursor(c);
        m_tab->setCursor(c);
        m_side_bar->setCursor(c);
        //m_status_bar->update();
    });

    connect(this, &MainWindow::locationChangeEnd, this, [=]() {
        m_side_bar->blockSignals(false);
        m_header_bar->blockSignals(false);
        QCursor c;
        c.setShape(Qt::ArrowCursor);
        this->setCursor(c);
        m_tab->setCursor(c);
        m_side_bar->setCursor(c);
        updateHeaderBar();
        //function for UKUI3.1, update window icon
        //updateWindowIcon();
        //m_status_bar->update();
    });

    //HeaderBar
    auto views = new TabWidget;

    auto headerBar = new HeaderBar(this);
    m_header_bar = headerBar;
    auto headerBarContainer = new HeaderBarContainer(this);
    headerBarContainer->addHeaderBar(headerBar);

    TopMenuBar *top = new TopMenuBar(headerBar, this);
    views->setMenuBar(top);
//    views->m_header_bar_layout->insertWidget(0,headerBarContainer);
    views->addToolBar(headerBarContainer);
    //m_header_bar->setVisible(false);

    connect(m_header_bar, &HeaderBar::updateLocationRequest, this, &MainWindow::goToUri);
    connect(m_header_bar, &HeaderBar::viewTypeChangeRequest, this, &MainWindow::beginSwitchView);
    connect(m_header_bar, &HeaderBar::updateZoomLevelHintRequest, this, [=](int zoomLevelHint) {
        if (zoomLevelHint >= 0) {
            m_tab->m_status_bar->m_slider->setEnabled(true);
            m_tab->m_status_bar->m_slider->setValue(zoomLevelHint);
        } else {
            m_tab->m_status_bar->m_slider->setEnabled(false);
        }
    });

    //SideBar
    auto sideBarFactory = Peony::SideBarFactoryManager::getInstance()->getFactoryFromPlatformName();
    if (!sideBarFactory) {
        NavigationSideBarContainer *sidebar = new NavigationSideBarContainer(this);
        m_side_bar = sidebar;
    } else {
        auto sidebar = sideBarFactory->create(this);
        m_side_bar = sidebar;
    }
    m_transparent_area_widget = m_side_bar;
    connect(m_side_bar, &Peony::SideBar::updateWindowLocationRequest, this, &MainWindow::goToUri);
    connect(m_side_bar, &Peony::SideBar::updateWindowLocationRequest, m_header_bar, &HeaderBar::cancleSelect);
    addDockWidget(Qt::LeftDockWidgetArea, m_side_bar);

//    auto labelDialog = new FileLabelBox(this);
//    labelDialog->hide();
//    TitleLabel *t = new TitleLabel(this);

//    auto splitter = new QSplitter(this);
//    splitter->setChildrenCollapsible(false);
//    splitter->setHandleWidth(0);
//    splitter->setOrientation(Qt::Vertical);
//    splitter->addWidget(t);
//    splitter->addWidget(navigationSidebarContainer);
    //splitter->addWidget(labelDialog);

//    connect(labelDialog->selectionModel(), &QItemSelectionModel::selectionChanged, [=]()
//    {
//        auto selected = labelDialog->selectionModel()->selectedIndexes();
//        //qDebug() << "FileLabelBox selectionChanged:" <<selected.count();
//        if (selected.count() > 0)
//        {
//            auto name = selected.first().data().toString();
//            setLabelNameFilter(name);
//        }
//    });
    //when clicked in blank, currentChanged may not triggered
//    connect(labelDialog, &FileLabelBox::leftClickOnBlank, [=]()
//    {
//        setLabelNameFilter("");
//    });

//    connect(sidebar, &NavigationSideBar::labelButtonClicked, labelDialog, &QWidget::setVisible);

//    m_status_bar = new Peony::StatusBar(this, this);
//    setStatusBar(m_status_bar);

//    auto views = new TabWidget;
    m_tab = views;
    if (uri.isNull()) {
        auto home = "file://" + QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
        m_tab->addPage(home, true);
    } else {
        m_tab->addPage(uri, true);
        m_header_bar->setLocation(uri);
    }
    QTimer::singleShot(1, this, [=]() {
        // FIXME:
        // it is strange that if we set size hint by qsettings,
        // the tab bar will "shrink" until we update geometry
        // after a while (may resize window or move splitter).
        // we should find out the reason and remove this dirty
        // code.
        m_tab->updateTabBarGeometry();
        m_tab->updateStatusBarGeometry();
    });

    connect(Peony::GlobalSettings::getInstance(), &Peony::GlobalSettings::valueChanged, this, [=](const QString &key) {
        if (key == ZOOM_SLIDER_VISIBLE) {
            m_tab->updateStatusBarGeometry();
        }
    });

    connect(views->tabBar(), &QTabBar::tabBarDoubleClicked, this, [=](int index) {
        bool tablet = Peony::GlobalSettings::getInstance()->getValue(TABLET_MODE).toBool();
        if (index == -1&&!tablet)
            maximizeOrRestore();
    });
    connect(m_tab,&TabWidget::tabBarIndexUpdate,this,[=](int index){
        if(index == 0)
            m_is_first_tab = true;
        else
           m_is_first_tab = false;
        update();

    });
    connect(views, &TabWidget::closeWindowRequest, this, &QWidget::close);
    connect(m_header_bar, &HeaderBar::updateSearchRequest, m_tab, &TabWidget::updateSearchBar);

    X11WindowManager *tabBarHandler = X11WindowManager::getInstance();
    tabBarHandler->registerWidget(views->tabBar());

    setCentralWidget(views);

    // check slider zoom level
    if (currentViewSupportZoom())
        setCurrentViewZoomLevel(currentViewZoomLevel());

    //bind signals
    connect(m_tab, &TabWidget::searchRecursiveChanged, headerBar, &HeaderBar::updateSearchRecursive);
    connect(m_tab, &TabWidget::closeSearch, headerBar, &HeaderBar::closeSearch);
    connect(m_tab, &TabWidget::viewSelectStatus, headerBar, &HeaderBar::switchSelectStatus);
    connect(m_tab, &TabWidget::updateWindowLocationRequest, headerBar, &HeaderBar::cancleSelect);
    connect(m_tab,&TabWidget::globalSearch, headerBar, &HeaderBar::setGlobalFlag);
    connect(m_tab, &TabWidget::clearTrash, this, &MainWindow::cleanTrash);
    connect(m_tab, &TabWidget::recoverFromTrash, this, &MainWindow::recoverFromTrash);
    connect(m_tab, &TabWidget::updateWindowLocationRequest, this, &MainWindow::goToUri);
    connect(m_tab, &TabWidget::activePageLocationChanged, this, &MainWindow::locationChangeEnd);
    connect(m_tab, &TabWidget::activePageViewTypeChanged, this, &MainWindow::updateHeaderBar);
    connect(m_tab, &TabWidget::activePageChanged, this, &MainWindow::updateHeaderBar);
    connect(m_tab, &TabWidget::activePageChanged, this, [=](){
        // check slider zoom level
        setCurrentViewZoomLevel(currentViewZoomLevel());
    });

    connect(m_tab, &TabWidget::signal_itemAdded, this, [=](const QString& uri){
        /* 新建文件/文件夹，可编辑文件名，copy时不能编辑 */
        if(this->m_uris_to_edit.isEmpty())
            return;
        QString editUri = Peony::FileUtils::urlDecode(this->m_uris_to_edit.first());
        QString infoUri = Peony::FileUtils::urlDecode(uri);
        if (editUri == infoUri ) {
            this->getCurrentPage()->getView()->scrollToSelection(uri);
            this->editUri(uri);
        }
        this->m_uris_to_edit.clear();
    });

    connect(m_tab, &TabWidget::menuRequest, this, [=]() {
        Peony::DirectoryViewMenu menu(this);
        menu.exec(QCursor::pos());
        m_uris_to_edit = menu.urisToEdit();
    });

    connect(m_tab, &TabWidget::updateWindowSelectionRequest, this, [=](const QStringList &uris){
        setCurrentSelectionUris(uris);
    });
//    connect(m_tab, &TabWidget::currentSelectionChanged, this, [=](){
//        m_status_bar->update();
//    });
}

void MainWindow::cleanTrash()
{
    auto uris = getCurrentAllFileUris();
    Peony::AudioPlayManager::getInstance()->playWarningAudio();
    if (uris.count() >0) {
        Peony::TrashWarnDialog *dialog = new Peony::TrashWarnDialog(nullptr);

        connect(dialog, &Peony::TrashWarnDialog::accepted, [=]{
            Peony::FileOperationUtils::remove(uris);
        });

        dialog->exec();
    } else {
        QMessageBox::information(nullptr, tr("Tips info"),
                                 tr("Trash has no file need to be cleaned."));
    }
}

void MainWindow::recoverFromTrash()
{
    auto m_selections = getCurrentSelections();
    if (m_selections.isEmpty())
        m_selections = getCurrentAllFileUris();
    if (m_selections.count() == 1) {
        Peony::FileOperationUtils::restore(m_selections.first());
    } else {
        Peony::FileOperationUtils::restore(m_selections);
    }
}

void MainWindow::initAdvancePage()
{
    //Fix me: advance search page, need the new design to develop new UI
    //auto filterBar = new Peony::AdvanceSearchBar(this);
}

QRect MainWindow::sideBarRect()
{
    auto pos = m_transparent_area_widget->mapTo(this, QPoint());
    return QRect(pos, m_transparent_area_widget->size());
}

const QList<std::shared_ptr<Peony::FileInfo>> MainWindow::getCurrentSelectionFileInfos()
{
    const QStringList uris = getCurrentSelections();
    QList<std::shared_ptr<Peony::FileInfo>> infos;
    for(auto uri : uris) {
        auto info = Peony::FileInfo::fromUri(uri);
        infos<<info;
    }
    return infos;
}
