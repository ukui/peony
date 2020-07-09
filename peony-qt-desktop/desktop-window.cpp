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
 * Authors: Meihong He <hemeihong@kylinos.cn>
 *
 */

#include "desktop-window.h"
#include "file-info-job.h"
#include "file-info.h"
#include "file-item-model.h"
#include "file-item-proxy-filter-sort-model.h"
#include "file-launch-manager.h"
#include "file-operation-utils.h"

#include "desktop-menu.h"

#include "desktop-icon-view.h"
#include "desktop-item-model.h"

#include "clipboard-utils.h"
#include "file-copy-operation.h"
#include "file-move-operation.h"
#include "file-operation-manager.h"
#include "file-trash-operation.h"
#include "properties-window.h"

#include "peony-desktop-application.h"
#include "singleapplication.h"

#include <QDesktopServices>

#include <QLabel>
#include <QStandardPaths>

#include <QApplication>
#include <QDesktopWidget>
#include <QScreen>

#include <QItemSelectionModel>

#include <QVariantAnimation>
#include <QPainter>

#include <QFileDialog>

#include <QProcess>

// NOTE build failed on Archlinux. Can't detect `QGSettings/QGSettings' header
// fixed by replaced `QGSettings/QGSettings' with `QGSettings'
#include <QGSettings>

// backup settings
#include <QSettings>

#include <QX11Info>

#include <X11/Xlib.h>
#include <QX11Info>
#include <X11/Xatom.h>
#include <X11/Xproto.h>

#include <QDebug>

#define BACKGROUND_SETTINGS "org.mate.background"
#define PICTRUE "picture-filename"
#define FALLBACK_COLOR "primary-color"

using namespace Peony;

DesktopWindow::DesktopWindow(QScreen *screen, bool is_primary, QWidget *parent)
    : QMainWindow(parent) {
    initGSettings();

    setWindowTitle(tr("Desktop"));
    m_opacity = new QVariantAnimation(this);
    m_opacity->setDuration(1000);
    m_opacity->setStartValue(double(0));
    m_opacity->setEndValue(double(1));
    connect(m_opacity, &QVariantAnimation::valueChanged, this, [=]() {
        this->update();
    });

    connect(m_opacity, &QVariantAnimation::finished, this, [=]() {
        m_bg_back_pixmap = m_bg_font_pixmap;
        m_bg_back_cache_pixmap = m_bg_font_cache_pixmap;
        m_last_pure_color = m_color_to_be_set;
    });

    m_screen = screen;

    //connect(m_screen, &QScreen::availableGeometryChanged, this, &DesktopWindow::updateView);
    connectSignal();

    m_is_primary = is_primary;
    setContentsMargins(0, 0, 0, 0);

    qDebug() << "DesktopWindow is_primary:" << is_primary << screen->objectName()
             << screen->name();
    auto flags = windowFlags() &~Qt::WindowMinMaxButtonsHint;
    setWindowFlags(flags |Qt::FramelessWindowHint);
    setAttribute(Qt::WA_X11NetWmWindowTypeDesktop);
    setAttribute(Qt::WA_TranslucentBackground);

    //fix qt5.6 setAttribute as desktop has no effect issue
#if (QT_VERSION < QT_VERSION_CHECK(5, 7, 0))
    if (QX11Info::isPlatformX11()) {
        Atom m_WindowType = XInternAtom(QX11Info::display(), "_NET_WM_WINDOW_TYPE", true);
        Atom m_DesktopType = XInternAtom(QX11Info::display(), "_NET_WM_WINDOW_TYPE_DESKTOP", true);
        XDeleteProperty(QX11Info::display(), winId(), m_WindowType);
        XChangeProperty(QX11Info::display(), winId(), m_WindowType,
                        XA_ATOM, 32, 1, (unsigned char *)&m_DesktopType, 1);
    }
#endif

    setGeometry(screen->geometry());

    setContextMenuPolicy(Qt::CustomContextMenu);

    // menu
    connect(this, &QMainWindow::customContextMenuRequested,
    [=](const QPoint &pos) {
        // FIXME: use other menu
        qDebug() << "menu request";
        auto contentMargins = contentsMargins();
        auto fixedPos = pos - QPoint(contentMargins.left(), contentMargins.top());
        auto index = PeonyDesktopApplication::getIconView()->indexAt(fixedPos);
        if (!index.isValid() || !centralWidget()) {
            PeonyDesktopApplication::getIconView()->clearSelection();
        } else {
            if (!PeonyDesktopApplication::getIconView()->selectionModel()->selection().indexes().contains(index)) {
                PeonyDesktopApplication::getIconView()->clearSelection();
                PeonyDesktopApplication::getIconView()->selectionModel()->select(index, QItemSelectionModel::Select);
            }
        }

        QTimer::singleShot(1, [=]() {
            DesktopMenu menu(PeonyDesktopApplication::getIconView());
            if (PeonyDesktopApplication::getIconView()->getSelections().isEmpty()) {
                auto action = menu.addAction(tr("set background"));
                connect(action, &QAction::triggered, [=]() {
                    //go to control center set background
                    gotoSetBackground();
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
            menu.exec(mapToGlobal(pos));
            auto urisToEdit = menu.urisToEdit();
            if (urisToEdit.count() == 1) {
                QTimer::singleShot(
                100, this, [=]() {
                    PeonyDesktopApplication::getIconView()->editUri(urisToEdit.first());
                });
            }
        });
    });

    connect(m_screen, &QScreen::geometryChanged, this,
            &DesktopWindow::geometryChangedProcess);
    connect(m_screen, &QScreen::virtualGeometryChanged, this,
            &DesktopWindow::virtualGeometryChangedProcess);

    if (!m_is_primary || true) {
        setBg(getCurrentBgPath());
        return;
    }
}

DesktopWindow::~DesktopWindow() {}

void DesktopWindow::initGSettings() {
    if (!QGSettings::isSchemaInstalled(BACKGROUND_SETTINGS)) {
        m_backup_setttings = new QSettings("org.ukui", "peony-qt-desktop", this);
        if (m_backup_setttings->value("color").isNull()) {
            auto defaultColor = QColor(Qt::cyan).darker();
            m_backup_setttings->setValue("color", defaultColor);
        }
        return;
    }

    m_bg_settings = new QGSettings(BACKGROUND_SETTINGS, QByteArray(), this);

    connect(m_bg_settings, &QGSettings::changed, this, [=](const QString &key) {
        qDebug() << "bg settings changed:" << key;
        if (key == "pictureFilename") {
            auto bg_path = m_bg_settings->get("pictureFilename").toString();
            if (!QFile::exists(bg_path)) {
                // use pure color;
                auto colorString = m_bg_settings->get("primary-color").toString();
                auto color = QColor(colorString);
                qDebug() << colorString;
                this->setBg(color);
            } else {
                if (m_current_bg_path == bg_path)
                    return;
                this->setBg(bg_path);
            }
        }
        if (key == "primaryColor") {
            auto bg_path = m_bg_settings->get("pictureFilename").toString();
            if (!bg_path.startsWith("/")) {
                auto colorString = m_bg_settings->get("primary-color").toString();
                auto color = QColor(colorString);
                qDebug() << colorString;
                this->setBg(color);
            } else {
                // do nothing
            }
        }
    });
}

void DesktopWindow::gotoSetBackground()
{
    QProcess p;
    p.setProgram("ukui-control-center");
    //old version use -a, new version use -b as para
    p.setArguments(QStringList()<<"-b");
#if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
    p.startDetached();
#else
    p.startDetached("ukui-control-center", QStringList()<<"-b");
#endif
    p.waitForFinished(-1);
}

void DesktopWindow::paintEvent(QPaintEvent *e)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    if (m_opacity->state() == QVariantAnimation::Running) {
        //draw bg?
        if (m_use_pure_color) {
            auto opacity = m_opacity->currentValue().toDouble();
            p.fillRect(this->rect(), m_last_pure_color);
            p.save();
            p.setOpacity(opacity);
            p.fillRect(this->rect(), m_color_to_be_set);
            p.restore();
        } else {
            auto opacity = m_opacity->currentValue().toDouble();
            p.drawPixmap(this->rect(), m_bg_back_cache_pixmap, m_bg_back_cache_pixmap.rect());
            p.save();
            p.setOpacity(opacity);
            p.drawPixmap(this->rect(), m_bg_font_cache_pixmap, m_bg_font_cache_pixmap.rect());
            p.restore();
        }
    } else {
        //draw bg?
        if (m_use_pure_color) {
            p.fillRect(this->rect(), m_last_pure_color);
        } else {
            p.drawPixmap(this->rect(), m_bg_back_cache_pixmap, m_bg_back_cache_pixmap.rect());
        }
    }
    QMainWindow::paintEvent(e);
}

const QString DesktopWindow::getCurrentBgPath() {
    // FIXME: implement custom bg settings storage
    if (m_current_bg_path.isEmpty()) {
        if (m_bg_settings)
            m_current_bg_path = m_bg_settings->get("pictureFilename").toString();
        else
            m_current_bg_path = m_backup_setttings->value("pictrue").toString();
    }
    return m_current_bg_path;
}

const QColor DesktopWindow::getCurrentColor()
{
    QColor color;
    if (m_bg_settings) {
        color = qvariant_cast<QColor>(m_bg_settings->get("primary-color"));
    } else {
        color = qvariant_cast<QColor>(m_backup_setttings->value("color"));
    }
    return color;
}

void DesktopWindow::setBg(const QString &path) {
    qDebug() << path;
    if (path.isNull()) {
        setBg(getCurrentColor());
        return;
    }

    m_use_pure_color = false;

    m_bg_back_pixmap = m_bg_font_pixmap;

    m_bg_font_pixmap = QPixmap(path);
    // FIXME: implement different pixmap clip algorithm.

    m_bg_back_cache_pixmap = m_bg_back_pixmap.scaled(m_screen->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    m_bg_font_cache_pixmap = m_bg_font_pixmap.scaled(m_screen->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    m_current_bg_path = path;
    setBgPath(path);

    if (m_opacity->state() == QVariantAnimation::Running) {
        m_opacity->setCurrentTime(500);
    } else {
        m_opacity->stop();
        m_opacity->start();
    }
}

void DesktopWindow::setBg(const QColor &color) {
    m_color_to_be_set = color;

    m_use_pure_color = true;

    if (m_opacity->state() == QVariantAnimation::Running) {
        m_opacity->setCurrentTime(500);
    } else {
        m_opacity->stop();
        m_opacity->start();
    }
}

void DesktopWindow::setBgPath(const QString &bgPath) {
    if (m_bg_settings) {
        m_bg_settings->set(PICTRUE, bgPath);
    } else {
        m_backup_setttings->setValue("pictrue", bgPath);
        m_backup_setttings->sync();
        Q_EMIT this->changeBg(bgPath);
    }
}

void DesktopWindow::setScreen(QScreen *screen) {
    m_screen = screen;
}

void DesktopWindow::setIsPrimary(bool is_primary) {
    m_is_primary = is_primary;
}

void DesktopWindow::connectSignal() {
    connect(m_screen, &QScreen::geometryChanged, this,
            &DesktopWindow::geometryChangedProcess);
    connect(m_screen, &QScreen::virtualGeometryChanged, this,
            &DesktopWindow::virtualGeometryChangedProcess);
    connect(m_screen, &QScreen::availableGeometryChanged, this,
            &DesktopWindow::availableGeometryChangedProcess);
}

void DesktopWindow::disconnectSignal() {
    disconnect(m_screen, &QScreen::geometryChanged, this,
               &DesktopWindow::geometryChangedProcess);
    disconnect(m_screen, &QScreen::virtualGeometryChanged, this,
               &DesktopWindow::virtualGeometryChangedProcess);

    if (m_is_primary) {
        disconnect(m_screen, &QScreen::availableGeometryChanged, this,
                   &DesktopWindow::availableGeometryChangedProcess);
    }
}

void DesktopWindow::scaleBg(const QRect &geometry) {
    if (this->geometry() == geometry)
        return;

    setGeometry(geometry);
    /*!
     * \note
     * There is a bug in kwin, if we directly set window
     * geometry or showFullScreen, window will not be resized
     * correctly.
     *
     * reset the window flags will resovle the problem,
     * but screen will be black a while.
     * this is not user's expected.
     */
    //setWindowFlag(Qt::FramelessWindowHint, false);
    auto flags = windowFlags() &~Qt::WindowMinMaxButtonsHint;
    setWindowFlags(flags |Qt::FramelessWindowHint);

    //fix qt5.6 setAttribute as desktop has no effect issue
#if QT_VERSION_CHECK(5, 6, 0)
    Atom m_WindowType = XInternAtom(QX11Info::display(), "_NET_WM_WINDOW_TYPE", true);
    Atom m_DesktopType = XInternAtom(QX11Info::display(), "_NET_WM_WINDOW_TYPE_DESKTOP", true);
    XDeleteProperty(QX11Info::display(), winId(), m_WindowType);
    XChangeProperty(QX11Info::display(), winId(), m_WindowType,
                    XA_ATOM, 32, 1, (unsigned char *)&m_DesktopType, 1);
#endif

    show();

    m_bg_back_cache_pixmap = m_bg_back_pixmap.scaled(geometry.size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    m_bg_font_cache_pixmap = m_bg_font_pixmap.scaled(geometry.size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    this->update();
}

void DesktopWindow::initShortcut() {
    // shotcut
    return;
    QAction *copyAction = new QAction(this);
    copyAction->setShortcut(QKeySequence::Copy);
    connect(copyAction, &QAction::triggered, [=]() {
        auto selectedUris = PeonyDesktopApplication::getIconView()->getSelections();
        if (!selectedUris.isEmpty())
            ClipboardUtils::setClipboardFiles(selectedUris, false);
    });
    addAction(copyAction);

    QAction *cutAction = new QAction(this);
    cutAction->setShortcut(QKeySequence::Cut);
    connect(cutAction, &QAction::triggered, [=]() {
        auto selectedUris = PeonyDesktopApplication::getIconView()->getSelections();
        if (!selectedUris.isEmpty())
            ClipboardUtils::setClipboardFiles(selectedUris, true);
    });
    addAction(cutAction);

    QAction *pasteAction = new QAction(this);
    pasteAction->setShortcut(QKeySequence::Paste);
    connect(pasteAction, &QAction::triggered, [=]() {
        auto clipUris = ClipboardUtils::getClipboardFilesUris();
        if (ClipboardUtils::isClipboardHasFiles()) {
            ClipboardUtils::pasteClipboardFiles(PeonyDesktopApplication::getIconView()->getDirectoryUri());
        }
    });
    addAction(pasteAction);

    QAction *trashAction = new QAction(this);
    trashAction->setShortcut(QKeySequence::Delete);
    connect(trashAction, &QAction::triggered, [=]() {
        auto selectedUris = PeonyDesktopApplication::getIconView()->getSelections();
        if (!selectedUris.isEmpty()) {
            auto op = new FileTrashOperation(selectedUris);
            FileOperationManager::getInstance()->startOperation(op, true);
        }
    });
    addAction(trashAction);

    QAction *undoAction = new QAction(this);
    undoAction->setShortcut(QKeySequence::Undo);
    connect(undoAction, &QAction::triggered,
    [=]() {
        FileOperationManager::getInstance()->undo();
    });
    addAction(undoAction);

    QAction *redoAction = new QAction(this);
    redoAction->setShortcut(QKeySequence::Redo);
    connect(redoAction, &QAction::triggered,
    [=]() {
        FileOperationManager::getInstance()->redo();
    });
    addAction(redoAction);

    QAction *zoomInAction = new QAction(this);
    zoomInAction->setShortcut(QKeySequence::ZoomIn);
    connect(zoomInAction, &QAction::triggered, [=]() {
        PeonyDesktopApplication::getIconView()->zoomIn();
    });
    addAction(zoomInAction);

    QAction *zoomOutAction = new QAction(this);
    zoomOutAction->setShortcut(QKeySequence::ZoomOut);
    connect(zoomOutAction, &QAction::triggered, [=]() {
        PeonyDesktopApplication::getIconView()->zoomOut();
    });
    addAction(zoomOutAction);

    QAction *renameAction = new QAction(this);
    renameAction->setShortcuts(QList<QKeySequence>()<<QKeySequence(Qt::ALT + Qt::Key_E)<<Qt::Key_F2);
    connect(renameAction, &QAction::triggered, [=]() {
        auto selections = PeonyDesktopApplication::getIconView()->getSelections();
        if (selections.count() == 1) {
            PeonyDesktopApplication::getIconView()->editUri(selections.first());
        }
    });
    addAction(renameAction);

    QAction *removeAction = new QAction(this);
    removeAction->setShortcuts(QList<QKeySequence>()<<QKeySequence(Qt::SHIFT + Qt::Key_Delete));
    connect(removeAction, &QAction::triggered, [=]() {
        qDebug() << "delete" << PeonyDesktopApplication::getIconView()->getSelections();
        FileOperationUtils::executeRemoveActionWithDialog(PeonyDesktopApplication::getIconView()->getSelections());
    });
    addAction(removeAction);

    auto propertiesWindowAction = new QAction(this);
    propertiesWindowAction->setShortcuts(QList<QKeySequence>()<<QKeySequence(Qt::ALT + Qt::Key_Return)
                                         <<QKeySequence(Qt::ALT + Qt::Key_Enter));
    connect(propertiesWindowAction, &QAction::triggered, this, [=]() {
        if (PeonyDesktopApplication::getIconView()->getSelections().count() > 0)
        {
            PropertiesWindow *w = new PropertiesWindow(PeonyDesktopApplication::getIconView()->getSelections());
            w->show();
        }
    });
    addAction(propertiesWindowAction);

    auto newFolderAction = new QAction(this);
    newFolderAction->setShortcuts(QList<QKeySequence>()<<QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_N));
    connect(newFolderAction, &QAction::triggered, this, [=]() {
        CreateTemplateOperation op(PeonyDesktopApplication::getIconView()->getDirectoryUri(), CreateTemplateOperation::EmptyFolder, tr("New Folder"));
        op.run();
        auto targetUri = op.target();

        QTimer::singleShot(500, this, [=]() {
            PeonyDesktopApplication::getIconView()->scrollToSelection(targetUri);
        });
    });
    addAction(newFolderAction);

    auto refreshAction = new QAction(this);
    refreshAction->setShortcut(Qt::Key_F5);
    connect(refreshAction, &QAction::triggered, this, [=]() {
        PeonyDesktopApplication::getIconView()->refresh();
    });
    addAction(refreshAction);

    QAction *editAction = new QAction(PeonyDesktopApplication::getIconView());
    editAction->setShortcuts(QList<QKeySequence>()<<QKeySequence(Qt::ALT + Qt::Key_E)<<Qt::Key_F2);
    connect(editAction, &QAction::triggered, this, [=]() {
        auto selections = PeonyDesktopApplication::getIconView()->getSelections();
        if (selections.count() == 1) {
            PeonyDesktopApplication::getIconView()->editUri(selections.first());
        }
    });
    addAction(editAction);
}

void DesktopWindow::availableGeometryChangedProcess(const QRect &geometry) {
    updateView();
}

void DesktopWindow::virtualGeometryChangedProcess(const QRect &geometry) {
    updateWinGeometry();
}

void DesktopWindow::geometryChangedProcess(const QRect &geometry) {
    // screen resolution ratio change
    updateWinGeometry();
}

void DesktopWindow::updateView() {
    auto avaliableGeometry = m_screen->availableGeometry();
    auto geomerty = m_screen->geometry();
    int top = qAbs(avaliableGeometry.top() - geomerty.top());
    int left = qAbs(avaliableGeometry.left() - geomerty.left());
    int bottom = qAbs(avaliableGeometry.bottom() - geomerty.bottom());
    int right = qAbs(avaliableGeometry.right() - geomerty.right());
    //skip unexpected avaliable geometry, it might lead by ukui-panel.
    if (top > 200 | left > 200 | bottom > 200 | right > 200) {
        setContentsMargins(0, 0, 0, 0);
        return;
    }
    setContentsMargins(left, top, right, bottom);
}

void DesktopWindow::updateWinGeometry() {
    auto screenName = m_screen->name();
    auto screenSize = m_screen->size();
    auto g = getScreen()->geometry();
    auto vg = getScreen()->virtualGeometry();
    auto ag = getScreen()->availableGeometry();

//    this->move(m_screen->geometry().topLeft());
//    this->setFixedSize(m_screen->geometry().size());
//    /*!
//      \bug
//      can not set window geometry correctly in kwin.
//      strangely it works in ukwm.
//      */
//    this->setGeometry(m_screen->geometry());
//    Q_EMIT this->checkWindow();

    scaleBg(g);

    auto name = m_screen->name();
    if (m_screen == qApp->primaryScreen()) {
        if (auto view = qobject_cast<DesktopIconView *>(centralWidget())) {
            this->show();
        }
    } else {
        if (m_screen->geometry() == qApp->primaryScreen()->geometry())
            this->hide();
        else
            this->show();
    }

    //updateView();
}
