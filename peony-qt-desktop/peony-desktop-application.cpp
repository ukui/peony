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

#define KYLIN_USER_GUIDE_PATH "/"
#define KYLIN_USER_GUIDE_SERVICE QString("com.kylinUserGuide.hotel_%1").arg(getuid())
#define KYLIN_USER_GUIDE_INTERFACE "com.guide.hotel"

#define DESKTOP_MEDIA_HANDLE "org.gnome.desktop.media-handling"

static bool has_desktop = false;
static bool has_daemon = false;
static Peony::DesktopIconView *desktop_icon_view = nullptr;

//record of desktop start time
qint64 PeonyDesktopApplication::peony_desktop_start_time = 0;

void guessContentTypeCallback(GObject* object,GAsyncResult *res,gpointer data);

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

PeonyDesktopApplication::PeonyDesktopApplication(int &argc, char *argv[], const char *applicationName) : SingleApplication (argc, argv, applicationName, true)
{
    setApplicationVersion("v3.0.0");
    setApplicationName("peony-qt-desktop");
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

    if (this->isPrimary()) {
        qDebug()<<"isPrimary screen";
        connect(this, &SingleApplication::receivedMessage, [=](quint32 id, QByteArray msg) {
            this->parseCmd(id, msg, true);
        });
        QFile file(":/desktop-icon-view.qss");
        file.open(QFile::ReadOnly);
        setStyleSheet(QString::fromLatin1(file.readAll()));
        file.close();
        Peony::DesktopMenuPluginManager::getInstance();

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
    }

    connect(this, &SingleApplication::layoutDirectionChanged, this, &PeonyDesktopApplication::layoutDirectionChangedProcess);
    connect(this, &SingleApplication::primaryScreenChanged, this, &PeonyDesktopApplication::primaryScreenChangedProcess);
    connect(this, &SingleApplication::screenAdded, this, &PeonyDesktopApplication::screenAddedProcess);
    connect(this, &SingleApplication::screenRemoved, this, &PeonyDesktopApplication::screenRemovedProcess);

    //parse cmd
    auto message = this->arguments().join(' ').toUtf8();
    parseCmd(this->instanceId(), message, isPrimary());

    auto volumeManager = Peony::VolumeManager::getInstance();
    connect(volumeManager,&Peony::VolumeManager::mountAdded,this,[=](const std::shared_ptr<Peony::Mount> &mount){
        //auto open dir for inserted udisk or dvd.
        GMount *newMount = (GMount*)g_object_ref(mount->getGMount());
        g_mount_guess_content_type(newMount,FALSE,NULL,guessContentTypeCallback,NULL);
    });
}

Peony::DesktopIconView *PeonyDesktopApplication::getIconView()
{
    if (!desktop_icon_view)
        desktop_icon_view = new Peony::DesktopIconView;
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

void PeonyDesktopApplication::parseCmd(quint32 id, QByteArray msg, bool isPrimary)
{
    QCommandLineParser parser;

    QCommandLineOption quitOption(QStringList()<<"q"<<"quit", tr("Close the peony desktop window"));
    parser.addOption(quitOption);

    QCommandLineOption daemonOption(QStringList()<<"d"<<"daemon", tr("Take over the dbus service."));
    parser.addOption(daemonOption);

    QCommandLineOption desktopOption(QStringList()<<"w"<<"desktop-window", tr("Take over the desktop displaying"));
    parser.addOption(desktopOption);

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
            if (!has_desktop) {
                //FIXME: load menu plugin
                //FIXME: take over desktop displaying
                getIconView();
                for(auto screen : this->screens())
                {
                    addWindow(screen);
                }
            }
            has_desktop = true;
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
    Peony::DesktopWindow *window;
    if (checkPrimay) {
        bool is_primary = isPrimaryScreen(screen);
        window = new Peony::DesktopWindow(screen, is_primary);
        if (is_primary)
        {
            window->setCentralWidget(desktop_icon_view);
            window->updateView();
            //connect(window, &Peony::DesktopWindow::changeBg, this, &PeonyDesktopApplication::changeBgProcess);
        }
    } else {
        window = new Peony::DesktopWindow(screen, false);
    }

    connect(window, &Peony::DesktopWindow::checkWindow, this, &PeonyDesktopApplication::checkWindowProcess);
    //window->showFullScreen();
    m_window_list<<window;

    for (auto window : m_window_list) {
        window->updateWinGeometry();
    }
}

void PeonyDesktopApplication::layoutDirectionChangedProcess(Qt::LayoutDirection direction)
{
    //not regular operate, to complete later
    qDebug()<<"layoutDirectionChangedProcess"<<direction;
}

void PeonyDesktopApplication::primaryScreenChangedProcess(QScreen *screen)
{
    if (screen != nullptr)
        qDebug()<<"primaryScreenChangedProcess"<<screen->name()<<screen->geometry()<<screen->availableGeometry()<<screen->virtualGeometry();
    else {
        qWarning()<<"no primary screen!";
        return;
    }

    bool need_exchange = false;
    QScreen *preMainScreen = nullptr;
    Peony::DesktopWindow *rawPrimaryWindow = nullptr;
    Peony::DesktopWindow *currentPrimayWindow = nullptr;
    for(auto win : m_window_list)
    {
        if (win->centralWidget())
            rawPrimaryWindow = win;

        if (win->getScreen() == screen)
            currentPrimayWindow = win;
        //need exchange window screen
//        if (win->getView() && win->getScreen() != screen)
//        {
//            preMainScreen = win->getScreen();
//            need_exchange = true;
//            break;
//        }
    }

    if (rawPrimaryWindow && currentPrimayWindow) {
        currentPrimayWindow->setCentralWidget(getIconView());
        //desktop_icon_view->show();
        currentPrimayWindow->updateView();
        currentPrimayWindow->hide();
        currentPrimayWindow->show();
    }
    return;

    //do not check window need exchange
    //cause we always put the desktop icon view into window
    //which in current primary screen.
    if (need_exchange)
    {
        for(auto win : m_window_list)
        {
            //qDebug()<<"before screen info"<<win->getScreen()->name()<<win->getScreen()->geometry()<<win->geometry()<<preMainScreen->name();
            win->disconnectSignal();
            if (win->getScreen() == preMainScreen)
            {
                win->setScreen(screen);
                win->setIsPrimary(true);
            }
            else if (win->getScreen() == screen) {
                win->setScreen(preMainScreen);
                win->setIsPrimary(false);
            }
            win->connectSignal();
            win->updateWinGeometry();
            //qDebug()<<"end screen info"<<win->getScreen()->name()<<win->getScreen()->geometry()<<win->geometry();
            //if (win->getView())
            //qDebug()<<"view info:"<<win->getView();
        }
    }
}

void PeonyDesktopApplication::screenAddedProcess(QScreen *screen)
{
    if (screen != nullptr)
        qDebug()<<"screenAdded"<<screen->name()<<screen<<m_window_list.size()<<screen->availableSize();
    else {
        return;
    }

    addWindow(screen, false);
}

void PeonyDesktopApplication::screenRemovedProcess(QScreen *screen)
{
    //if (screen != nullptr)
    //qDebug()<<"screenRemoved"<<screen->name()<<screen->serialNumber();

    //window manage
    for(auto win :m_window_list)
    {
        //screen not changed
        if (win->getScreen() == screen)
        {
            qDebug()<<"remove window";
            m_window_list.removeOne(win);
            win->deleteLater();
        }
    }
}

bool PeonyDesktopApplication::isPrimaryScreen(QScreen *screen)
{
    if (screen == this->primaryScreen() && screen)
        return true;

    return false;
}

void PeonyDesktopApplication::changeBgProcess(const QString& bgPath)
{
//    for (auto win : m_window_list) {
//        if (!isPrimaryScreen(win->getScreen()))
//            win->setBg(bgPath);
//    }
}

void PeonyDesktopApplication::checkWindowProcess()
{
    //do not check windows, primary window should be handled to exchange in
    //primaryScreenChanged signal emitted.
    return;
    for(auto win : m_window_list)
    {
        //fix duplicate screen cover main screen view problem
        if (win->getScreen() != this->primaryScreen())
        {
            if (win->getScreen()->geometry() == this->primaryScreen()->geometry())
            {
                win->setVisible(false);
            }
            else {
                win->setVisible(true);
            }
        }
    }
}

void guessContentTypeCallback(GObject* object,GAsyncResult *res,gpointer data)
{
    char **guessType;
    GError *error;
    QString openFolderCmd;
    GFile* root;
    char *mountUri;
    bool openFolder;
    QProcess process;

    error = NULL;
    openFolder = true;
    root = g_mount_get_default_location(G_MOUNT(object));
    mountUri = g_file_get_uri(root);
    openFolderCmd = "peony " + QString(mountUri);
    guessType = g_mount_guess_content_type_finish(G_MOUNT(object),res,&error);

    if(error){
        g_error_free(error);
        error = NULL;
    }else{
        if(guessType && g_strv_length(guessType) > 0){
            int n;
            for(n = 0; guessType[n]; ++n){
                if(g_content_type_is_a(guessType[n],"x-content/win32-software"))
                    openFolder = false;
                if(!strcmp(guessType[n],"x-content/blank-dvd"))
                    openFolder = false;

                if(openFolder)
                    process.startDetached(openFolderCmd);

                openFolder = true;
            }
            g_strfreev(guessType);
        }else{
            GDrive *drive = g_mount_get_drive(G_MOUNT(object));
            char *unixDevice = NULL;
            if(drive){
                unixDevice = g_drive_get_identifier(drive,G_DRIVE_IDENTIFIER_KIND_UNIX_DEVICE);
                g_object_unref(drive);
            }

            //Only DvD devices are allowed to open folder automatically.
            if(unixDevice && !strcmp(unixDevice,"/dev/sr") && QGSettings::isSchemaInstalled(DESKTOP_MEDIA_HANDLE)){
                QGSettings* autoMountSettings =  new QGSettings(DESKTOP_MEDIA_HANDLE);
                if(autoMountSettings->get("automount-open").toBool()){
                    process.startDetached(openFolderCmd);
                    delete autoMountSettings;
                }
            }

            g_free(unixDevice);
        }
    }

    g_free(mountUri);
    g_object_unref(root);
    g_object_unref(object);
}
