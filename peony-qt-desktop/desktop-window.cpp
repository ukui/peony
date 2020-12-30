/*
 * Peony-Qt
 *
 * Copyright (C) 2020, KylinSoft Co., Ltd.
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
#include "global-settings.h"

#include <QDesktopServices>
#include <QtDBus>
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
#include <QFileInfo>

#include <QX11Info>

#include <X11/Xlib.h>
#include <QX11Info>
#include <X11/Xatom.h>
#include <X11/Xproto.h>

#include <QDateTime>
#include <QDebug>
#include <QObject>

#define BACKGROUND_SETTINGS "org.mate.background"
#define PICTRUE "picture-filename"
#define FALLBACK_COLOR "primary-color"
#define FONT_SETTINGS "org.ukui.style"

using namespace Peony;

DesktopWindow::DesktopWindow(QScreen *screen, bool is_primary, QWidget *parent)
    : QMainWindow(parent) {
    initGSettings();

    setWindowTitle(tr("Desktop"));
    m_opacity = new QVariantAnimation(this);
    m_opacity->setDuration(1000);
    m_opacity->setStartValue(double(0));
    m_opacity->setEndValue(double(1));

    bool tabletMode = Peony::GlobalSettings::getInstance()->getValue(TABLET_MODE).toBool();
    m_tabletmode = tabletMode;
    if(tabletMode){
        setAttribute(Qt::WA_X11NetWmWindowTypeDesktop,false);
        this->hide();
    } else {
        setAttribute(Qt::WA_X11NetWmWindowTypeDesktop);
    }

    connect(qApp, &QApplication::paletteChanged, this, &DesktopWindow::updateScreenVisible);

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
        qDebug() << "menu request in desktop window";
        auto contentMargins = contentsMargins();
//        auto fixedPos = pos - QPoint(contentMargins.left(), contentMargins.top());
        auto index = PeonyDesktopApplication::getIconView()->indexAt(QCursor::pos());
//        auto selectcount = PeonyDesktopApplication::getIconView()->getSelections().count();
        if (!index.isValid()||!centralWidget()) {
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
                });
            }
            menu.exec(QCursor::pos());
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

    auto start_cost_time = QDateTime::currentMSecsSinceEpoch()- PeonyDesktopApplication::peony_desktop_start_time;
    qDebug() << "desktop start end in desktop-window time:" <<start_cost_time
             <<"ms"<<QDateTime::currentMSecsSinceEpoch();
}

DesktopWindow::~DesktopWindow() {}

void DesktopWindow::initGSettings() {
    qDebug() <<"DesktopWindow initGSettings";
    if (!QGSettings::isSchemaInstalled(BACKGROUND_SETTINGS)) {
        m_backup_setttings = new QSettings("org.ukui", "peony-qt-desktop", this);
        if (m_backup_setttings->value("color").isNull()) {
            auto defaultColor = QColor(Qt::cyan).darker();
            m_backup_setttings->setValue("color", defaultColor);
        }
        return;
    }

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

    m_bg_settings = new QGSettings(BACKGROUND_SETTINGS, QByteArray(), this);
    m_picture_option = m_bg_settings->get("pictureOptions").toString();

    if(m_picture_option == ""||m_picture_option == NULL)
        m_picture_option = "stretched";

    connect(m_bg_settings, &QGSettings::changed, this, [=](const QString &key) {
        qDebug() << "bg settings changed:" << key;
        if (key == "pictureFilename") {
            auto bg_path = m_bg_settings->get("pictureFilename").toString();
            //control center set color bg
            if (bg_path == "")
            {
                qDebug() << "bg_path == """;
                // use pure color;
                auto colorString = m_bg_settings->get("primary-color").toString();
                auto color = QColor(colorString);
                qDebug() <<"pure color bg"<< colorString;
                this->setBg(color);
                m_current_bg_path = "";
                return;
            }

            if (QFile::exists(bg_path))
            {
                qDebug() << "bg_path:" <<bg_path << m_current_bg_path;
                if (m_current_bg_path == bg_path)
                    return;
                auto path = QDir::homePath() + "/.config/";
                if(QFileInfo(bg_path).absolutePath() != path )
                {
                    auto pathName = path+QFileInfo(bg_path).fileName();
                    QFile(bg_path).copy(pathName);
                    bool success = m_bg_settings->trySet("pictureFilename", pathName);
                    if (success)
                    {
                        bg_path = pathName;
                    }
                    else
                    {
                        qDebug() << "use default bg picture fail, reset";
                        m_bg_settings->reset("pictureFilename");
                    }
                }
                qDebug() << "set a new bg picture:" <<bg_path;
                this->setBg(bg_path);
                return;
            }

            //can not find bg file, usually the file is moved, use default bg
            QString path = "/usr/share/backgrounds/default.jpg";
            //commercial version use different default bg
            if (COMMERCIAL_VERSION)
               path = "/usr/share/backgrounds/aurora.jpg";

            //system default bg not exist, use pure color bg
            if (! QFile::exists(path))
            {
                qCritical() << "Did not find system default background, use color bg instead!";
                m_bg_settings->trySet("pictureFilename", "");
                setBg(getCurrentColor());
                return;
            }

            bool success = m_bg_settings->trySet("pictureFilename", path);
            if (success)
            {
                m_current_bg_path = "file://" + path;
            }
            else
            {
                qDebug() << "use default bg picture fail, reset";
                m_bg_settings->reset("pictureFilename");
            }
        }
        if (key == "primaryColor")
        {
            auto colorString = m_bg_settings->get("primary-color").toString();
            auto color = QColor(colorString);
            qDebug() <<"set color bg"<< colorString;
            this->setBg(color);
        }
        if(key == "pictureOptions")
        {
            m_picture_option = m_bg_settings->get("pictureOptions").toString();
            if(m_picture_option == ""||m_picture_option == NULL)
                m_picture_option = "stretched";
            this->setBg(m_current_bg_path);
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
            if(m_picture_option == "centered")//居中
            {
                p.drawPixmap((m_screen->size().width()-m_bg_back_cache_pixmap.rect().width())/2,
                              (m_screen->size().height()-m_bg_back_cache_pixmap.rect().height())/2,
                             m_bg_back_cache_pixmap);
                p.save();
                p.setOpacity(opacity);
                p.drawPixmap((m_screen->size().width()-m_bg_font_cache_pixmap.rect().width())/2,
                              (m_screen->size().height()-m_bg_font_cache_pixmap.rect().height())/2,
                             m_bg_font_cache_pixmap);
//                p.drawPixmap(this->rect(), m_bg_font_cache_pixmap, m_bg_font_cache_pixmap.rect());
                p.restore();
            }
            else if(m_picture_option == "stretched")//拉伸
            {
                p.drawPixmap(this->rect(), m_bg_back_cache_pixmap, m_bg_back_cache_pixmap.rect());
                p.save();
                p.setOpacity(opacity);
                p.drawPixmap(this->rect(), m_bg_font_cache_pixmap, m_bg_font_cache_pixmap.rect());
                p.restore();
            }
            else if(m_picture_option == "scaled")//填充
            {
                p.drawPixmap(this->rect(), m_bg_back_cache_pixmap);
                p.save();
                p.setOpacity(opacity);
                p.drawPixmap(this->rect(), m_bg_font_cache_pixmap);
                p.restore();
            }
            else if(m_picture_option == "wallpaper")//平铺
            {
                int drawedWidth = 0;
                int drawedHeight = 0;
                while (1) {
                    drawedWidth = 0;
                    while (1) {
                        p.drawPixmap(drawedWidth, drawedHeight, m_bg_back_cache_pixmap);
                        drawedWidth += m_bg_back_cache_pixmap.width();
                        if (drawedWidth >= m_screen->size().width()) {
                            break;
                        }
                    }
                    drawedHeight += m_bg_back_cache_pixmap.height();
                    if (drawedHeight >= m_screen->size().height()) {
                        break;
                    }
                }
                p.save();
                p.setOpacity(opacity);
                drawedWidth =0;
                drawedHeight = 0;
                while (1) {
                    drawedWidth = 0;
                    while (1) {
                        p.drawPixmap(drawedWidth, drawedHeight, m_bg_font_cache_pixmap);
                        drawedWidth += m_bg_font_cache_pixmap.width();
                        if (drawedWidth >= m_screen->size().width()) {
                            break;
                        }
                    }
                    drawedHeight += m_bg_font_cache_pixmap.height();
                    if (drawedHeight >= m_screen->size().height()) {
                        break;
                    }
                }
                p.restore();
            }
            else
            {
                p.drawPixmap(this->rect(), m_bg_back_cache_pixmap, m_bg_back_cache_pixmap.rect());
                p.save();
                p.setOpacity(opacity);
                p.drawPixmap(this->rect(), m_bg_font_cache_pixmap, m_bg_font_cache_pixmap.rect());
                p.restore();
            }
        }
    } else {
        //draw bg?
        if (m_use_pure_color) {
            p.fillRect(this->rect(), m_color_to_be_set);
            m_used_pure_color = true;
            m_last_pure_color = m_color_to_be_set;
        } else {
//            p.drawPixmap(this->rect(), m_bg_back_cache_pixmap, m_bg_back_cache_pixmap.rect());
            if(m_picture_option == "centered")
                p.drawPixmap((m_screen->size().width()-m_bg_font_cache_pixmap.rect().width())/2,
                             (m_screen->size().height()-m_bg_font_cache_pixmap.rect().height())/2,
                              m_bg_font_cache_pixmap);
            else if(m_picture_option == "stretched")
                p.drawPixmap(this->rect(), m_bg_font_cache_pixmap);
            else if(m_picture_option == "scaled")
                p.drawPixmap(this->rect(), m_bg_font_cache_pixmap);
            else if(m_picture_option == "wallpaper")
            {
                 int drawedWidth = 0;
                 int drawedHeight = 0;
                while (1) {
                    drawedWidth = 0;
                    while (1) {
                        p.drawPixmap(drawedWidth, drawedHeight, m_bg_font_cache_pixmap);
                        drawedWidth += m_bg_font_cache_pixmap.width();
                        if (drawedWidth >= m_screen->size().width()) {
                            break;
                        }
                    }
                    drawedHeight += m_bg_font_cache_pixmap.height();
                    if (drawedHeight >= m_screen->size().height()) {
                        break;
                    }
                }
            }
            else
                p.drawPixmap(this->rect(), m_bg_font_cache_pixmap);
            m_used_pure_color = false;
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
    qDebug() << "DesktopWindow::setBg:"<<path;
    if (path.isNull()) {
        setBg(getCurrentColor());
        return;
    }

    m_use_pure_color = false;

    m_bg_back_pixmap = m_bg_font_pixmap;
    if(m_bg_back_pixmap.isNull())
        m_bg_back_pixmap = QPixmap(path);

    m_bg_font_pixmap = QPixmap(path);
    // FIXME: implement different pixmap clip algorithm.
    if(m_picture_option == "scaled"){
        m_bg_back_cache_pixmap = m_bg_back_pixmap.scaled(m_screen->size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);

        m_bg_font_cache_pixmap = m_bg_font_pixmap.scaled(m_screen->size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    }
    else
    {
        m_bg_back_cache_pixmap = m_bg_back_pixmap;

        m_bg_font_cache_pixmap = m_bg_font_pixmap;
    }

    m_current_bg_path = path;
    setBgPath(path);

    if(!m_used_pure_color)
        if (m_opacity->state() == QVariantAnimation::Running) {
            m_opacity->setCurrentTime(500);
        } else {
            m_opacity->stop();
            m_opacity->start();
        }
    else
        update();

}

void DesktopWindow::setBg(const QColor &color) {
    m_color_to_be_set = color;

    m_use_pure_color = true;

    if(m_used_pure_color)
        if (m_opacity->state() == QVariantAnimation::Running) {
            m_opacity->setCurrentTime(500);
        } else {
            m_opacity->stop();
            m_opacity->start();
        }
    else
        update();
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

//    show();

//    m_bg_back_cache_pixmap = m_bg_back_pixmap.scaled(geometry.size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
//    m_bg_font_cache_pixmap = m_bg_font_pixmap.scaled(geometry.size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
//    this->update();
    if(m_used_pure_color)
        setBg(m_color_to_be_set);
    else
        setBg(m_current_bg_path);
}

void DesktopWindow::initShortcut() {
    // shotcut
    return;
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

    scaleBg(g);

    updateScreenVisible();
}

void DesktopWindow::updateScreenVisible()
{
    m_tabletmode = Peony::GlobalSettings::getInstance()->getValue(TABLET_MODE).toBool();

    if (true == m_tabletmode) {
        //pad mode desktop should hide, pla
        hide();
    } else {
        //PC mode desktop will show,
        if (m_screen == qApp->primaryScreen()) {
            //primary screen must be show
            if (auto view = qobject_cast<DesktopIconView *>(centralWidget())) {
                show();
            }
        } else {
            // if desktop is mirror mode the slave screen will hide, or show empty desktop.
            if (m_screen->geometry() == qApp->primaryScreen()->geometry()) {
                hide();
            } else {
                show();
            }
        }
    }

    return;
}
