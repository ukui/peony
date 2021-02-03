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

#include <KWindowSystem>

#include <QDateTime>
#include <QDBusInterface>
#include <QDebug>

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
        qDebug() << "menu request in desktop window";
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
        //use account background first
        auto accountBack = getAccountBackground();
        if (accountBack != "" && QFile::exists(accountBack))
            setBg(accountBack);
        else
        {
            setBg(getCurrentBgPath());
            //update user background
            setAccountBackground();
        }
        // do not animate while window first create.
        m_opacity->setCurrentTime(m_opacity->totalDuration());
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
                //comment to fix name not change but file changed issue
//                if (m_current_bg_path == bg_path)
//                    return;
                qDebug() << "set a new bg picture:" <<bg_path;
                this->setBg(bg_path);
                //comment to fix name not change but file changed issue
//                if (getAccountBackground() != m_current_bg_path)
//                {
                    setAccountBackground();
//                }
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

QString DesktopWindow::getAccountBackground()
{
    uid_t uid = getuid();
    QDBusInterface iface("org.freedesktop.Accounts", "/org/freedesktop/Accounts",
                         "org.freedesktop.Accounts",QDBusConnection::systemBus());

    QDBusReply<QDBusObjectPath> userPath = iface.call("FindUserById", (qint64)uid);
    if(!userPath.isValid())
        qWarning() << "Get UserPath error:" << userPath.error();
    else {
        QDBusInterface userIface("org.freedesktop.Accounts", userPath.value().path(),
                                 "org.freedesktop.DBus.Properties", QDBusConnection::systemBus());
        QDBusReply<QDBusVariant> backgroundReply = userIface.call("Get", "org.freedesktop.Accounts.User", "BackgroundFile");
        if(backgroundReply.isValid())
            return  backgroundReply.value().variant().toString();
    }
    return "";
}

void DesktopWindow::setAccountBackground()
{
    QDBusInterface * interface = new QDBusInterface("org.freedesktop.Accounts",
                                     "/org/freedesktop/Accounts",
                                     "org.freedesktop.Accounts",
                                     QDBusConnection::systemBus());

    if (!interface->isValid()){
        qCritical() << "Create /org/freedesktop/Accounts Client Interface Failed " << QDBusConnection::systemBus().lastError();
        return;
    }

    QDBusReply<QDBusObjectPath> reply =  interface->call("FindUserByName", g_get_user_name());
    QString userPath;
    if (reply.isValid()){
        userPath = reply.value().path();
    }
    else {
        qCritical() << "Call 'GetComputerInfo' Failed!" << reply.error().message();
        return;
    }

    QDBusInterface * useriFace = new QDBusInterface("org.freedesktop.Accounts",
                                                    userPath,
                                                    "org.freedesktop.Accounts.User",
                                                    QDBusConnection::systemBus());

    if (!useriFace->isValid()){
        qCritical() << QString("Create %1 Client Interface Failed").arg(userPath) << QDBusConnection::systemBus().lastError();
        return;
    }

    QDBusMessage msg = useriFace->call("SetBackgroundFile", m_current_bg_path);
    qDebug() << "setAccountBackground path:" <<m_current_bg_path;
    if (!msg.errorMessage().isEmpty())
        qDebug() << "update user background file error: " << msg.errorMessage();
}

const QString DesktopWindow::getCurrentBgPath()
{
    // FIXME: implement custom bg settings storage
    if (m_current_bg_path.isEmpty() || m_current_bg_path == "") {
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
    if (m_bg_settings)
    {
        //add same path check to break endless call
        if (m_bg_settings->get(PICTRUE).isNull() || m_bg_settings->get(PICTRUE) != bgPath)
            m_bg_settings->set(PICTRUE, bgPath);
    }
    else{
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
    qDebug() << "geometryChangedProcess:" <<geometry <<topLevelWidget()->winId();
    KWindowSystem::setState(topLevelWidget()->winId(), NET::States(NET::Desktop|NET::KeepBelow));
    QTimer::singleShot(500, this, [=](){
        auto view = PeonyDesktopApplication::getIconView();
        if (view->topLevelWidget()->isVisible()) {
            qDebug() << "geometryChangedProcess visible view:" <<view->topLevelWidget()->winId();
            KWindowSystem::raiseWindow(view->topLevelWidget()->winId());
            KWindowSystem::setState(view->topLevelWidget()->winId(), NET::States(NET::Desktop|NET::KeepAbove));
        }
    });
}

void DesktopWindow::updateView() {
    auto avaliableGeometry = m_screen->availableGeometry();
    auto geomerty = m_screen->geometry();
    qDebug() << "updateView:" <<avaliableGeometry<<geomerty;
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

    qDebug() << "updateWinGeometry: args:" <<screenName <<screenSize<<g<<vg<<ag<<this->geometry();

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
        qDebug() << "non primaryScreen:" <<m_screen->geometry() <<qApp->primaryScreen()->geometry();
        if (m_screen->geometry() == qApp->primaryScreen()->geometry())
        {
            this->hide();
            qWarning() << "error: non primaryScreen geometry same with primaryScreen ! " <<m_screen->geometry();
        }
        else
            this->show();
    }

    //updateView();
}
