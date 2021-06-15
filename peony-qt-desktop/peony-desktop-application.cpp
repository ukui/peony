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

#include "peony-desktop-application.h"

#include "fm-dbus-service.h"
#include "desktop-menu-plugin-manager.h"

#include "volume-manager.h"

#include "desktop-icon-view.h"

#include "primary-manager.h"
#include "plasma-shell-manager.h"
#include "desktop-menu.h"

#include "desktopbackground.h"
#include "desktop-background-manager.h"
#include "desktopbackgroundwindow.h"

#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QTimer>

#include <QProcess>
#include <QFile>
#include <QLabel>
#include <QtConcurrent>

#include <QTranslator>
#include <QLocale>

#include <QSystemTrayIcon>
#include <QGSettings>

#include <gio/gio.h>

#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusMessage>
#include <QDBusReply>
#include <QDesktopServices>

#include <QScreen>
#include <QTimeLine>

#include <KWindowSystem>

#define KYLIN_USER_GUIDE_PATH "/"
#define KYLIN_USER_GUIDE_SERVICE QString("com.kylinUserGuide.hotel_%1").arg(getuid())
#define KYLIN_USER_GUIDE_INTERFACE "com.guide.hotel"

#define DESKTOP_MEDIA_HANDLE "org.gnome.desktop.media-handling"

static bool has_desktop = false;
static bool has_daemon = false;
static bool has_background = false;
static QRect max_size = QRect(0, 0, 0, 0);
static Peony::DesktopIconView *desktop_icon_view = nullptr;
static PrimaryManager *screensMonitor = nullptr;
/*!
 * \brief virtualDesktopWindow
 * \deprecated
 */
static DesktopBackground *virtualDesktopWindow = nullptr;

//record of desktop start time
qint64 PeonyDesktopApplication::peony_desktop_start_time = 0;

void guessContentTypeCallback(GObject* object,GAsyncResult *res,gpointer data);
static void volume_mount_cb (GObject* source, GAsyncResult* res, gpointer udata);

void trySetDefaultFolderUrlHandler() {
    //NOTE:
    //There is a bug in qt concurrent. If we use QtConcurrent::run()
    //to start a async function in the init stage, the qapplication will
    //not quit when we call "peony-qt-desktop -q" in command line.
    //
    //To solve this problem, the simplest way is delaying a while to execute the
    //async lambda function.
    QTimer::singleShot(1000, []() {
        QtConcurrent::run([=]() {
            GList *apps = g_app_info_get_all_for_type("inode/directory");
            bool hasPeonyQtAppInfo = false;
            GList *l = apps;
            while (l) {
                GAppInfo *info = static_cast<GAppInfo*>(l->data);
                QString cmd = g_app_info_get_executable(info);
                if (cmd.contains("peony")) {
                    hasPeonyQtAppInfo = true;
                    g_app_info_set_as_default_for_type(info, "inode/directory", nullptr);
                    break;
                }
                l = l->next;
            }
            if (apps) {
                g_list_free_full(apps, g_object_unref);
            }

            if (!hasPeonyQtAppInfo) {
                GAppInfo *peony_qt = g_app_info_create_from_commandline("peony",
                                     nullptr,
                                     G_APP_INFO_CREATE_SUPPORTS_URIS,
                                     nullptr);
                g_app_info_set_as_default_for_type(peony_qt, "inode/directory", nullptr);
                g_object_unref(peony_qt);
            }
            return;
        });
    });
}

QRect caculateVirtualDesktopGeometry() {
    QRegion screensRegion;
    for (auto screen : qApp->screens()) {
        screensRegion += screen->geometry();
    }

    if (screensMonitor) {
        int x = screensMonitor->getScreenGeometry("x");
        int y = screensMonitor->getScreenGeometry("y");
        int width = screensMonitor->getScreenGeometry("width");
        int height = screensMonitor->getScreenGeometry("height");
        screensRegion += QRect(x, y, width, height);
    }

    auto rect = screensRegion.boundingRect();
    if (rect.size().width()> rect.size().height())
    {
        rect = QRect(0, 0, rect.size().width(), rect.size().width());
    }
    else
    {
        rect = QRect(0, 0, rect.size().height(), rect.size().height());
    }
    if (rect.size().width()> max_size.size().width())
    {
        max_size = rect;
    }
    return max_size;
}

PeonyDesktopApplication::PeonyDesktopApplication(int &argc, char *argv[], const char *applicationName) : SingleApplication (argc, argv, applicationName, true)
{
    setApplicationVersion(QString("v%1").arg(VERSION));
    //setApplicationDisplayName(tr("Peony-Qt Desktop"));

    QTranslator *t = new QTranslator(this);
    t->load("/usr/share/libpeony-qt/libpeony-qt_"+QLocale::system().name());
    QApplication::installTranslator(t);
    QTranslator *t2 = new QTranslator(this);
    t2->load("/usr/share/peony-qt-desktop/peony-qt-desktop_"+QLocale::system().name());
    QApplication::installTranslator(t2);
    QTranslator *t3 = new QTranslator(this);
    t3->load("/usr/share/qt5/translations/qt_"+QLocale::system().name());
    QApplication::installTranslator(t3);
    setApplicationName(tr("peony-qt-desktop"));

    if (this->isPrimary()) {
        qDebug()<<"isPrimary screen";
        connect(this, &SingleApplication::receivedMessage, [=](quint32 id, QByteArray msg) {
            this->parseCmd(id, msg, true);
        });
        QFile file(":/desktop-icon-view.qss");
        file.open(QFile::ReadOnly);
        setStyleSheet(QString::fromLatin1(file.readAll()));
        file.close();

        //add 5 seconds delay to load plugins
        //try to fix first time enter desktop right menu not show open terminal issue
        QTimer::singleShot(5000, [=]() {
            Peony::DesktopMenuPluginManager::getInstance();
        });

        /*
        QSystemTrayIcon *trayIcon = new QSystemTrayIcon(this);
        auto volumeManager = Peony::VolumeManager::getInstance();
        connect(volumeManager, &Peony::VolumeManager::driveConnected, this, [=](const std::shared_ptr<Peony::Drive> &drive){
            trayIcon->show();
            trayIcon->showMessage(tr("Drive"), tr("%1 connected").arg(drive->name()));
        });
        connect(volumeManager, &Peony::VolumeManager::driveDisconnected, this, [=](const std::shared_ptr<Peony::Drive> &drive){
            trayIcon->show();
            trayIcon->showMessage(tr("Drive"), tr("%1 disconnected").arg(drive->name()));
        });

        connect(trayIcon, &QSystemTrayIcon::messageClicked, trayIcon, &QSystemTrayIcon::hide);
        */

        // auto mount local driver
        GVolumeMonitor* vm = g_volume_monitor_get ();
        if (vm) {
            GList* drives = g_volume_monitor_get_connected_drives(vm);
            if (drives) {
                for (GList* i = drives; nullptr != i; i = i->next) {
                    GDrive * d = static_cast<GDrive*>(i->data);
                    GList* volumes = g_drive_get_volumes(d);
                    if (volumes) {
                        for (GList* j = volumes; nullptr != j; j = j->next) {
                            GVolume* v = static_cast<GVolume*>(j->data);
                            g_autofree char* uuid = g_volume_get_uuid(v);
                            if (0 != strcmp("2691-6AB8", uuid)) {
                                g_volume_mount(v, G_MOUNT_MOUNT_NONE, nullptr, nullptr, volume_mount_cb, nullptr);
                            }

                            g_object_unref(v);
                        }
                        g_list_free(volumes);
                    }
                    g_object_unref(d);
                }
                g_list_free(drives);
            }
        }
        g_object_unref(vm);
    }

    connect(this, &SingleApplication::layoutDirectionChanged, this, &PeonyDesktopApplication::layoutDirectionChangedProcess);
//    connect(this, &SingleApplication::primaryScreenChanged, this, &PeonyDesktopApplication::primaryScreenChangedProcess);
    connect(this, &SingleApplication::screenAdded, this, &PeonyDesktopApplication::screenAddedProcess);
    connect(this, &SingleApplication::screenRemoved, this, &PeonyDesktopApplication::screenRemovedProcess);

    //parse cmd
    auto message = this->arguments().join(' ').toUtf8();
    parseCmd(this->instanceId(), message, isPrimary());

    auto volumeManager = Peony::VolumeManager::getInstance();
    connect(volumeManager,&Peony::VolumeManager::mountAdded,this,[=](const std::shared_ptr<Peony::Mount> &mount){
        // auto open dir for inserted dvd.
        GMount* newMount = (GMount*) g_object_ref(mount->getGMount());
        //special Volumn of 839 M upgrade part can not mount
        if (mount->uuid() != "2691-6AB8")
            g_mount_guess_content_type(newMount, FALSE, NULL, guessContentTypeCallback, NULL);

        // mount
    });
    connect(volumeManager,&Peony::VolumeManager::volumeRemoved,this,&PeonyDesktopApplication::volumeRemovedProcess);
    // 获取max_size初始值
    caculateVirtualDesktopGeometry();
}

Peony::DesktopIconView *PeonyDesktopApplication::getIconView()
{
    if (!desktop_icon_view)
        desktop_icon_view = new Peony::DesktopIconView(/*virtualDesktopWindow*/);
    return desktop_icon_view;
}

bool PeonyDesktopApplication::userGuideDaemonRunning()
{
    QDBusConnection conn = QDBusConnection::sessionBus();

    if (!conn.isConnected())
        return false;

    QDBusReply<QString> reply = conn.interface()->call("GetNameOwner", KYLIN_USER_GUIDE_SERVICE);

    return reply != "";
}

void PeonyDesktopApplication::showGuide(const QString &appName)
{
    if (!userGuideDaemonRunning()) {
        QUrl url = QUrl("help:ubuntu-kylin-help", QUrl::TolerantMode);
        QDesktopServices::openUrl(url);
        return;
    }

    bool bRet  = false;

    QDBusMessage m = QDBusMessage::createMethodCall(KYLIN_USER_GUIDE_SERVICE,
                     KYLIN_USER_GUIDE_PATH,
                     KYLIN_USER_GUIDE_INTERFACE,
                     "showGuide");
    m << appName;

    QDBusMessage response = QDBusConnection::sessionBus().call(m);
    if (response.type()== QDBusMessage::ReplyMessage)
    {
        //bRet = response.arguments().at(0).toBool();
    }
    else {
        QUrl url = QUrl("help:ubuntu-kylin-help", QUrl::TolerantMode);
        QDesktopServices::openUrl(url);
    }
}

void PeonyDesktopApplication::gotoSetBackground()
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

void PeonyDesktopApplication::relocateIconView()
{
    //FIXME:
    if (screensMonitor) {
//        getIconView()->setVisible(false);
        int x = screensMonitor->getScreenGeometry("x");
        int y = screensMonitor->getScreenGeometry("y");
        int width = screensMonitor->getScreenGeometry("width");
        int height = screensMonitor->getScreenGeometry("height");
        QRect geometry = QRect(x, y, width, height);
        if (geometry.isEmpty()) {
            qCritical()<<"can not get primary screen info from ukui-settings daemon";
            for (auto window : m_bg_windows) {
                if (window->screen() == qApp->primaryScreen()) {
                    getIconView()->setFixedSize(qApp->primaryScreen()->size());
                    getIconView()->setParent(window);
                }
            }
        } else {
            if (m_primaryScreenSettingsTimeLine->state() == QTimeLine::Running) {
                m_primaryScreenSettingsTimeLine->setCurrentTime(0);
            } else {
                m_primaryScreenSettingsTimeLine->start();
            }
        }
    }
}

void PeonyDesktopApplication::parseCmd(quint32 id, QByteArray msg, bool isPrimary)
{
    QCommandLineParser parser;

    QCommandLineOption quitOption(QStringList()<<"q"<<"quit", tr("Close the peony desktop window"));
    parser.addOption(quitOption);

    QCommandLineOption daemonOption(QStringList()<<"d"<<"daemon", tr("Take over the dbus service."));
    parser.addOption(daemonOption);

    QCommandLineOption desktopOption(QStringList()<<"w"<<"desktop-window", tr("Take over the desktop displaying"));
    parser.addOption(desktopOption);

    QCommandLineOption backgroundOption(QStringList()<<"b"<<"background", tr("Setup backgrounds"));
//    parser.addOption(backgroundOption);

    if (isPrimary) {
        if (m_first_parse) {
            auto helpOption = parser.addHelpOption();
            auto versionOption = parser.addVersionOption();
            m_first_parse = false;
        }

        Q_UNUSED(id)
        const QStringList args = QString(msg).split(' ');

        parser.process(args);
        if (parser.isSet(quitOption)) {
            QTimer::singleShot(1, [=]() {
                qApp->quit();
            });
            return;
        }

        if (parser.isSet(daemonOption)) {
            if (!has_daemon) {
                has_daemon = true;
                qDebug()<<"-d";

                trySetDefaultFolderUrlHandler();

                //FIXME: take over org.freedesktop.FileManager1
                Peony::FMDBusService *service = new Peony::FMDBusService(this);
                connect(service, &Peony::FMDBusService::showItemsRequest, [=](const QStringList &urisList) {
                    QProcess p;
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
                    p.setProgram("peony");
                    p.setArguments(QStringList()<<"--show-items"<<urisList);
                    p.startDetached();
#else
                    p.startDetached("peony", QStringList()<<"--show-items"<<urisList, nullptr);
#endif
                });
                connect(service, &Peony::FMDBusService::showFolderRequest, [=](const QStringList &urisList) {
                    QProcess p;
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
                    p.setProgram("peony");
                    p.setArguments(QStringList()<<"--show-folders"<<urisList);
                    p.startDetached();
#else
                    p.startDetached("peony", QStringList()<<"--show-folders"<<urisList, nullptr);
#endif
                });
                connect(service, &Peony::FMDBusService::showItemPropertiesRequest, [=](const QStringList &urisList) {
                    QProcess p;
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
                    p.setProgram("peony");
                    p.setArguments(QStringList()<<"--show-properties"<<urisList);
                    p.startDetached();
#else
                    p.startDetached("peony", QStringList()<<"--show-properties"<<urisList, nullptr);
#endif
                });
            }
            has_daemon = true;
        }

        if (parser.isSet(desktopOption)) {
            setupBgAndDesktop();
        }

        if (parser.isSet(backgroundOption)) {
            setupBgAndDesktop();
        }

        connect(this, &QApplication::paletteChanged, this, [=](const QPalette &pal) {
            for (auto w : allWidgets()) {
                w->setPalette(pal);
                w->update();
            }
        });
    }
    else {
        auto helpOption = parser.addHelpOption();
        auto versionOption = parser.addVersionOption();

        if (arguments().count() < 2) {
            parser.showHelp();
        }
        parser.process(arguments());

        sendMessage(msg);
    }
}

void PeonyDesktopApplication::addWindow(QScreen *screen, bool checkPrimay)
{

}

void PeonyDesktopApplication::layoutDirectionChangedProcess(Qt::LayoutDirection direction)
{
    //not regular operate, to complete later
    qDebug()<<"layoutDirectionChangedProcess"<<direction;
}

void PeonyDesktopApplication::primaryScreenChangedProcess(QScreen *screen)
{
    if (screensMonitor)
        return;
    getIconView()->setGeometry(screen->availableGeometry());
    getIconView()->raise();
}

void PeonyDesktopApplication::screenAddedProcess(QScreen *screen)
{
    addWindow(screen, false);
}

void PeonyDesktopApplication::screenRemovedProcess(QScreen *screen)
{

}

bool PeonyDesktopApplication::isPrimaryScreen(QScreen *screen)
{
    if (screen == this->primaryScreen() && screen)
        return true;

    return false;
}

bool PeonyDesktopApplication::relocateIconViewInternal()
{
    int x = screensMonitor->getScreenGeometry("x");
    int y = screensMonitor->getScreenGeometry("y");
    int width = screensMonitor->getScreenGeometry("width");
    int height = screensMonitor->getScreenGeometry("height");
    QRect geometry = QRect(x, y, width, height);
    bool synced = false;
    for (auto window : m_bg_windows) {
        if (window->screen()->geometry() == geometry) {
            getIconView()->setFixedSize(geometry.size());
            getIconView()->setParent(window);
            getIconView()->setVisible(true);
            getIconView()->restoreItemsPosByMetaInfo();
            KWindowSystem::raiseWindow(window->winId());
            synced = true;
            break;
        }
    }
    if (!synced) {
        qCritical()<<"can not set primary screen info from ukui-settings daemon";
        qCritical()<<"current geometry:"<<geometry;
        for (auto screen : qApp->screens()) {
            qCritical()<<"current screen"<<screen->name()<<screen->geometry();
        }
    }
    return synced;
}

void PeonyDesktopApplication::changeBgProcess(const QString& bgPath)
{

}

void PeonyDesktopApplication::checkWindowProcess()
{

}

void PeonyDesktopApplication::updateVirtualDesktopGeometryByWindows()
{

}

void PeonyDesktopApplication::addBgWindow(QScreen *screen)
{
    auto window = new DesktopBackgroundWindow(screen);
    m_bg_windows.append(window);

    // recheck primary screen info. new screen might become
    // primary screen.
    if (screensMonitor) {
        int x = screensMonitor->getScreenGeometry("x");
        int y = screensMonitor->getScreenGeometry("y");
        int width = screensMonitor->getScreenGeometry("width");
        int height = screensMonitor->getScreenGeometry("height");
        QRect geometry = QRect(x, y, width, height);
        if (!geometry.isEmpty()) {
            if (screen->geometry() == geometry) {
                getIconView()->setFixedSize(geometry.size());
                getIconView()->setParent(window);
                getIconView()->setVisible(true);
                getIconView()->restoreItemsPosByMetaInfo();
                KWindowSystem::raiseWindow(window->winId());
            }
        }
    }

    window->show();
    connect(screen, &QScreen::destroyed, this, [=](){
        if (getIconView()->parent() == window) {
            getIconView()->setParent(nullptr);
        }
        relocateIconView();
        m_bg_windows.removeOne(window);
        window->deleteLater();
    });
}

void PeonyDesktopApplication::setupDesktop()
{
    if (qgetenv("DESKTOP_SESSION") == QString("ukui-wayland")) {
        screensMonitor = new PrimaryManager;
        connect(screensMonitor, &PrimaryManager::priScreenChangedSignal, this, &PeonyDesktopApplication::relocateIconView);
        m_primaryScreenSettingsTimeLine = new QTimeLine(100, screensMonitor);
        connect(m_primaryScreenSettingsTimeLine, &QTimeLine::finished, this, [=]{
            bool synced = relocateIconViewInternal();

            if (!synced) {
                QTimer::singleShot(1000, this, &PeonyDesktopApplication::relocateIconView);
            }
        });
    }
    DesktopBackgroundManager::globalInstance();
    for (auto screen : qApp->screens()) {
        addBgWindow(screen);
    }
    relocateIconView();
    connect(qApp, &QApplication::screenAdded, this, &PeonyDesktopApplication::addBgWindow);
    return;
    if (!has_desktop) {
        has_desktop = true;
        virtualDesktopWindow = new DesktopBackground;
        virtualDesktopWindow->setAttribute(Qt::WA_TranslucentBackground);
        virtualDesktopWindow->setWindowFlag(Qt::FramelessWindowHint);
        virtualDesktopWindow->setAttribute(Qt::WA_X11NetWmWindowTypeDesktop);
        virtualDesktopWindow->show();

        virtualDesktopWindow->setContextMenuPolicy(Qt::CustomContextMenu);
        virtualDesktopWindow->connect(virtualDesktopWindow, &DesktopBackground::customContextMenuRequested, virtualDesktopWindow, [=](const QPoint &pos){
            qWarning()<<pos;
            auto fixedPos = getIconView()->mapFromGlobal(pos);
            auto index = PeonyDesktopApplication::getIconView()->indexAt(fixedPos);
            if (!index.isValid()) {
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
                        PeonyDesktopApplication::gotoSetBackground();
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

                for (auto screen : qApp->screens()) {
                    if (screen->geometry().contains(pos));
                    //menu.windowHandle()->setScreen(screen);
                }
                menu.exec(pos);
                auto urisToEdit = menu.urisToEdit();
                if (urisToEdit.count() == 1) {
                    QTimer::singleShot(
                    100, this, [=]() {
                        PeonyDesktopApplication::getIconView()->editUri(urisToEdit.first());
                    });
                }
            });
        });

        PlasmaShellManager::getInstance()->setRole(virtualDesktopWindow->windowHandle(), KWayland::Client::PlasmaShellSurface::Role::Desktop);

        // check ukui wayland session, monitor ukui settings daemon dbus
        if (QString(qgetenv("DESKTOP_SESSION")) == "ukui-wayland") {
            screensMonitor = new PrimaryManager;
            connect(screensMonitor, &PrimaryManager::priScreenChangedSignal, this, [=](int x, int y, int width, int height){
                getIconView()->setGeometry(x, y, width, height);
            });
            //screensMonitor->start();
            // init
            getIconView()->show();
            int x = screensMonitor->getScreenGeometry("x");
            int y = screensMonitor->getScreenGeometry("y");
            int width = screensMonitor->getScreenGeometry("width");
            int height = screensMonitor->getScreenGeometry("height");
            QRect geometry = QRect(x, y, width, height);
            if (width == 0) {
                qCritical()<<"failed to get primary screen info from dbus service, which provided by usd";
                if (qApp->screens().count() > 0) {
                    geometry = qApp->screens().first()->geometry();
                }

                // ukui settings daemon doesn't start. setup background directly.
                setupBgAndDesktop();
            }
            getIconView()->setGeometry(geometry);
        } else {
            getIconView()->setGeometry(qApp->primaryScreen()->geometry());
        }

        // delay 3 sec to setup background anyway.
        QTimer::singleShot(3000, this, [=](){
            if (!has_background) {
                has_background = true;
                virtualDesktopWindow->initBackground();
            }
        });
    }
}

void PeonyDesktopApplication::setupBgAndDesktop()
{
    if (!has_background) {
        setupDesktop();
    }
    has_background = true;
}

void guessContentTypeCallback(GObject* object, GAsyncResult *res,gpointer data)
{
    char **guessType;
    GError *error;
    QString openFolderCmd;
    GFile* root;
    char *mountUri;
    bool openFolder;
    QProcess process;
    static QString lastMountUri;

    error = NULL;
    openFolder = true;
    root = g_mount_get_default_location(G_MOUNT(object));
    mountUri = g_file_get_uri(root);

    openFolderCmd = "peony " + QString(mountUri);
    guessType = g_mount_guess_content_type_finish(G_MOUNT(object),res,&error);

    if (error) {
        g_error_free(error);
        error = NULL;
    } else {
        GDrive *drive = g_mount_get_drive(G_MOUNT(object));
        char *unixDevice = NULL;
        if(drive){
            unixDevice = g_drive_get_identifier(drive,G_DRIVE_IDENTIFIER_KIND_UNIX_DEVICE);
            g_object_unref(drive);
        }

        if(guessType && g_strv_length(guessType) > 0){
            int n;
            for(n = 0; guessType[n]; ++n){
                if(g_content_type_is_a(guessType[n],"x-content/win32-software"))
                    openFolder = false;
                if(!strcmp(guessType[n],"x-content/bootable-media") && !strstr(unixDevice,"/dev/sr"))
                    openFolder = false;
                if(!strcmp(guessType[n],"x-content/blank-dvd") || !strcmp(guessType[n],"x-content/blank-cd"))
                    openFolder = false;
                if(strstr(guessType[n],"x-content/audio-")){
                    if(!lastMountUri.compare(mountUri)){
                        lastMountUri.clear();
                        break;
                    }
                    lastMountUri = mountUri;
                }

                QString uri = mountUri;
                if (uri.startsWith("gphoto") || uri.startsWith("mtp"))
                    break;

                if(openFolder)
                    process.startDetached(openFolderCmd);

                openFolder = true;
            }
            g_strfreev(guessType);
        }else{
            //Only DvD devices are allowed to open folder automatically.
            if(unixDevice && !strcmp(unixDevice,"/dev/sr") && QGSettings::isSchemaInstalled(DESKTOP_MEDIA_HANDLE)){
                QGSettings* autoMountSettings =  new QGSettings(DESKTOP_MEDIA_HANDLE);
                if(autoMountSettings->get("automount-open").toBool()){
                    process.startDetached(openFolderCmd);
                    delete autoMountSettings;
                }
            }

        }
        g_free(unixDevice);
    }

    g_free(mountUri);
    g_object_unref(root);
    g_object_unref(object);
}

void PeonyDesktopApplication::volumeRemovedProcess(const std::shared_ptr<Peony::Volume> &volume)
{
    GVolume *gvolume = NULL;
    GDrive *gdrive = NULL;
    char *gdriveName = NULL;

    if(volume)
        gvolume = volume->getGVolume();
    if(gvolume)
        gdrive = g_volume_get_drive(gvolume);

    //Do not stop the DVD/CD driver.
    if(gdrive){
        gdriveName = g_drive_get_name(gdrive);
        if(gdriveName){
            if(strstr(gdriveName,"DVD") || strstr(gdriveName,"CD")){
                g_free(gdriveName);
                g_object_unref(gdrive);
                return;
            }
            g_free(gdriveName);
        }
    }

    //if it is possible, we stop it's drive after eject successfully.
   // if(gdrive && g_drive_can_stop(gdrive))
   //     g_drive_stop(gdrive,G_MOUNT_UNMOUNT_NONE,NULL,NULL,NULL,NULL);
};


static void volume_mount_cb (GObject* source, GAsyncResult* res, gpointer udata)
{
    g_volume_mount_finish(G_VOLUME (source), res, nullptr);

    Q_UNUSED(udata);
}
