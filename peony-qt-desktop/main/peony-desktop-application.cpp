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

#include "plasma-shell-manager.h"
#include "desktop-menu.h"

#include "desktop-global-settings.h"
#include "desktop-background-manager.h"
#include "desktop-background-window.h"

#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QTimer>

#include <QProcess>
#include <QFile>
#include <QLabel>
#include <QGraphicsEffect>
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

PeonyDesktopApplication::PeonyDesktopApplication(int &argc, char *argv[], const QString &applicationName) : QtSingleApplication (applicationName, argc, argv)
{
    qDebug()<<"peony desktop constructor start";
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
    setWindowIcon(QIcon::fromTheme("system-file-manager"));

    if (!this->isRunning()) {
        qDebug()<<"isPrimary screen";
        connect(this, &QtSingleApplication::messageReceived, [=](QString msg) {
            this->parseCmd(msg, true);
        });
        QFile file(":/desktop-icon-view.qss");
        file.open(QFile::ReadOnly);
        setStyleSheet(QString::fromLatin1(file.readAll()));
        file.close();

        //add 5 seconds delay to load plugins
        //try to fix first time enter desktop right menu not show open terminal issue
        //fix need wait 5 seconds open terminal issue, link to bug#75654
        //目前新的加载流程不需要等待那么久，加载出桌面时，应用都注册好了
        QTimer::singleShot(1000, this, [=]() {
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
        qDebug()<<"auto mount local volumes";
        GVolumeMonitor* vm = g_volume_monitor_get ();
        if (vm) {
            GList* drives = g_volume_monitor_get_connected_drives(vm);
            if (drives) {
                for (GList* i = drives; nullptr != i; i = i->next) {
                    GDrive * d = static_cast<GDrive*>(i->data);
                    if (G_IS_DRIVE(d)) {
                        GList* volumes = g_drive_get_volumes(d);
                        if (volumes) {
                            for (GList* j = volumes; nullptr != j; j = j->next) {
                                GVolume* v = static_cast<GVolume*>(j->data);
                                if (G_IS_VOLUME(v)) {
                                    g_autofree char* uuid = g_volume_get_uuid(v);
                                    if (0 != g_strcmp0("2691-6AB8", uuid)) {
                                        g_volume_mount(v, G_MOUNT_MOUNT_NONE, nullptr, nullptr, volume_mount_cb, nullptr);
                                    }
                                    g_object_unref(v);
                                }
                            }
                            g_list_free(volumes);
                        }
                        g_object_unref(d);
                    }
                }
                g_list_free(drives);
            }
        }
        g_object_unref(vm);
    }

    //parse cmd
    qDebug()<<"parse cmd";
    auto message = this->arguments().join(' ').toUtf8();
    parseCmd(message, !isRunning());

    qDebug()<<"monitor volumes change";
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
    //caculateVirtualDesktopGeometry();
    qDebug()<<"peony desktop application constructor end";
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
    //old version use -b, new version use -m Wallpaper as para，fix bug#94132
    p.setArguments(QStringList()<<"-m" <<"Wallpaper");
#if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
    p.startDetached();
#else
    p.startDetached("ukui-control-center", QStringList()<<"-m" <<"Wallpaper");
#endif
    p.waitForFinished(-1);
}

void PeonyDesktopApplication::parseCmd(QString msg, bool isPrimary)
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

    QCommandLineOption studyOption(QStringList()<<"s"<<"study", tr("Open learning center."));
    parser.addOption(studyOption);

    if (isPrimary) {
        if (m_firstParse) {
            auto helpOption = parser.addHelpOption();
            auto versionOption = parser.addVersionOption();
            m_firstParse = false;
        }

        const QStringList args = QString(msg).split(' ');

        parser.process(args);
        if (parser.isSet(quitOption)) {
            QTimer::singleShot(1000, this, [=]() {
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

        if (parser.isSet(studyOption)) {
            if (!has_desktop) {
                qWarning() << "[PeonyDesktopApplication::parseCmd] peony-qt-desktop is not running!";
                QTimer::singleShot(1, [=]() {
                    qApp->quit();
                });
                return;
            } else {
                //fix bug:#84724
                if (m_windowManager->getWindowByScreen(m_primaryScreen)->getCurrentDesktop()->getDesktopType() == DesktopType::StudyCenter) {
                    this->changePrimaryWindowDesktop(DesktopType::StudyCenter, AnimationType::RightToLeft);
                } else {
                    this->changePrimaryWindowDesktop(DesktopType::StudyCenter, AnimationType::LeftToRight);
                }
            }
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

void PeonyDesktopApplication::setupBgAndDesktop()
{
    if (!has_background) {
        this->initGSettings();
        this->initManager();

        setupDesktop();
        has_desktop = true;
        //在桌面初始化完成后开始初始化dbus
        initDBusService();
    }
    has_background = true;
}

void PeonyDesktopApplication::setupDesktop()
{
    qDebug() << "[PeonyDesktopApplication::setupDesktop]";
    //初始化主屏
    m_primaryScreen = QApplication::primaryScreen();
    for (auto screen : qApp->screens()) {
        addBgWindow(screen);
    }
    this->initWindowDesktop();
    this->raiseWindows();

    connect(this, &PeonyDesktopApplication::screenAdded, this, &PeonyDesktopApplication::screenAddedProcess);
    connect(this, &PeonyDesktopApplication::screenRemoved, this, &PeonyDesktopApplication::screenRemovedProcess);
    connect(this, &PeonyDesktopApplication::primaryScreenChanged, this, &PeonyDesktopApplication::primaryScreenChangedProcess);
    connect(this, &PeonyDesktopApplication::layoutDirectionChanged, this, &PeonyDesktopApplication::layoutDirectionChangedProcess);
}

void PeonyDesktopApplication::addBgWindow(QScreen *screen)
{
    if (m_windowManager->createWindowForScreen(screen)) {
        qDebug() << "[PeonyDesktopApplication::addBgWindow]" << screen->geometry() <<  (screen == QApplication::primaryScreen());
        connect(screen, &QScreen::geometryChanged, this, &PeonyDesktopApplication::raiseWindows);
        connect(screen, &QScreen::destroyed, this, [=]() {
            if (screen == m_primaryScreen) {
                m_primaryScreen = QApplication::primaryScreen();
            }
            m_windowManager->removeWindowByScreen(screen);
            updateDesktop();
        });
    }
}

void PeonyDesktopApplication::initWindowDesktop()
{
    for (auto window : m_windowManager->windowList()) {
        DesktopWidgetBase *desktop = nullptr;
        if (window->screen() == m_primaryScreen) {
            if (DesktopGlobalSettings::globalInstance()->getCurrentProjectName() == V10_SP1_EDU) {
                desktop = m_desktopManager->getDesktopByType(DesktopType::StudyCenter, window);
                m_learningCenterActivated = true;
            } else {
                desktop = m_desktopManager->getDesktopByType(DesktopType::Desktop, window);
            }
        } else {
            //添加副桌面
            //desktop = m_desktopManager->getDesktopByType(DesktopType::Desktop, window);
        }

        if (desktop) {
            connect(desktop, &Peony::DesktopWidgetBase::moveToOtherDesktop,
                    this, &PeonyDesktopApplication::changePrimaryWindowDesktop);

            window->setWindowDesktop(desktop);
        }
    }
}

void PeonyDesktopApplication::updateDesktop()
{
    qDebug() << "[PeonyDesktopApplication::updateDesktop]";
    for (auto window : m_windowManager->windowList()) {
        if (!window->getCurrentDesktop()) {
            DesktopWidgetBase *desktop = nullptr;
            if (window->screen() == m_primaryScreen) {
                desktop = m_desktopManager->getDesktopByType(DesktopType::Desktop, window);
            } else {
                //添加副桌面
                //window->setWindowDesktop(m_desktopManager->getDesktopByType(DesktopType::Tablet, window));
            }

            if (desktop) {
                connect(desktop, &Peony::DesktopWidgetBase::moveToOtherDesktop,
                        this, &PeonyDesktopApplication::changePrimaryWindowDesktop);
                window->setWindowDesktop(desktop);
            }
        }
    }

    this->raiseWindows();
}

void PeonyDesktopApplication::layoutDirectionChangedProcess(Qt::LayoutDirection direction)
{
    //not regular operate, to complete later
    qDebug()<<"layoutDirectionChangedProcess"<<direction;
}

void PeonyDesktopApplication::primaryScreenChangedProcess(QScreen *screen)
{
    qDebug() << "[PeonyDesktopApplication::primaryScreenChangedProcess]";
    if ((screen == nullptr) || (m_primaryScreen == nullptr)) {
        return;
    }

    if (screen == m_primaryScreen) {
        return;
    }

    Peony::DesktopBackgroundWindow *rawPrimaryWindow = m_windowManager->getWindowByScreen(m_primaryScreen);
    Peony::DesktopBackgroundWindow *currentPrimaryWindow = m_windowManager->getWindowByScreen(screen);

    if (rawPrimaryWindow && currentPrimaryWindow) {
        m_primaryScreen = screen;
        rawPrimaryWindow->hide();
        currentPrimaryWindow->hide();
        //主桌面
        DesktopWidgetBase *primaryDesktop = rawPrimaryWindow->takeCurrentDesktop();
        //副桌面
        DesktopWidgetBase *secondaryDesktop = currentPrimaryWindow->takeCurrentDesktop();

        rawPrimaryWindow->setWindowDesktop(secondaryDesktop);
        currentPrimaryWindow->setWindowDesktop(primaryDesktop);

        rawPrimaryWindow->show();
        currentPrimaryWindow->show();

        KWindowSystem::raiseWindow(currentPrimaryWindow->winId());
        if (primaryDesktop) {
            primaryDesktop->onPrimaryScreenChanged();
        }
    }
}

void PeonyDesktopApplication::screenAddedProcess(QScreen *screen)
{
    if (screen != nullptr) {
        qDebug() << "[PeonyDesktopApplication::screenAddedProcess] new screen:" << screen->geometry()
                 << "primary screen:" << m_primaryScreen->geometry();;
        //新建窗口
        this->addBgWindow(screen);
        this->updateDesktop();
    }
}

void PeonyDesktopApplication::raiseWindows()
{
    for (QScreen *screen : QApplication::screens()) {
        if (!m_windowManager->getWindowByScreen(screen)) {
            //为screen绑定窗口
            this->addBgWindow(screen);
        }

        if (screen == QApplication::primaryScreen()) {
            //新的主屏
            if (screen != m_primaryScreen) {
                this->primaryScreenChangedProcess(screen);
            }
        } else {
            //判断屏幕是否跟主屏重合
            if (screen->geometry().topLeft() == QApplication::primaryScreen()->geometry().topLeft()) {
                m_windowManager->getWindowByScreen(screen)->hide();
                continue;
            }
        }

        m_windowManager->getWindowByScreen(screen)->show();
        KWindowSystem::raiseWindow(m_windowManager->getWindowByScreen(screen)->winId());
    }
}

void PeonyDesktopApplication::screenRemovedProcess(QScreen *screen)
{
    qDebug() << "[PeonyDesktopApplication::screenRemovedProcess]" << screen->geometry();
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
                if(unixDevice && !strcmp(guessType[n],"x-content/bootable-media") && !strstr(unixDevice,"/dev/sr"))
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

void PeonyDesktopApplication::initManager()
{
    DesktopGlobalSettings::globalInstance(this);
    //初始化背景管理器
    DesktopBackgroundManager::globalInstance();

    m_windowManager  = WindowManager::getInstance(this);
    m_desktopManager = DesktopManager::getInstance(false,this);
}

QWidget *PeonyDesktopApplication::saveEffectWidget(QWidget *target)
{
    QWidget *saveEffectWidget = new QWidget(target);
    saveEffectWidget->resize(1,1);
    saveEffectWidget->setVisible(false);
    saveEffectWidget->hide();
    saveEffectWidget->setGraphicsEffect(target->graphicsEffect());
    return saveEffectWidget;
}

void PeonyDesktopApplication::changePrimaryWindowDesktop(DesktopType targetType, AnimationType targetAnimation)
{
    if (!DesktopGlobalSettings::globalInstance()->allowSwitchDesktop()) {
        qDebug() << "[PeonyDesktopApplication::changePrimaryWindowDesktop] not allowed !";
        return;
    }
    if (m_animationIsRunning) {
        return;
    }
    m_animationIsRunning = true;
    this->updateGSettingValues();
    //NOTE 只在主屏幕上切换桌面
    qDebug() << "[PeonyDesktopApplication::changePrimaryWindowDesktop] start process";

    Peony::DesktopBackgroundWindow *primaryWindow = m_windowManager->getWindowByScreen(m_primaryScreen);
    if (!primaryWindow) {
        m_animationIsRunning = false;
        qWarning() << "[PeonyDesktopApplication::changePrimaryWindowDesktop] primary window not found!";
        return;
    }

    Peony::DesktopWidgetBase *currentDesktop = primaryWindow->getCurrentDesktop();

    if (!currentDesktop) {
        m_animationIsRunning = false;
        qWarning() << "[PeonyDesktopApplication::changePrimaryWindowDesktop] primary window desktop not found!";
        return;
    }

    //获取一个桌面并指定父窗口
    Peony::DesktopWidgetBase *nextDesktop = nullptr;
    if (targetAnimation == AnimationType::CenterToEdge) {
        nextDesktop = m_desktopManager->getDesktopByType(targetType, primaryWindow);
        startScreenshotAnimation(primaryWindow, currentDesktop, nextDesktop);

    } else {
        nextDesktop = getNextDesktop(targetType, primaryWindow);
        startPropertyAnimation(primaryWindow, currentDesktop, nextDesktop, targetAnimation);
    }
}

void PeonyDesktopApplication::startPropertyAnimation(Peony::DesktopBackgroundWindow *primaryWindow,
                                                     Peony::DesktopWidgetBase *currentDesktop,
                                                     Peony::DesktopWidgetBase *nextDesktop,
                                                     AnimationType targetAnimation)
{
    if (!nextDesktop) {
        m_animationIsRunning = false;
        currentDesktop->setPause(false);
        qWarning() << "[PeonyDesktopApplication::changePrimaryWindowDesktop] nextDesktop is nullptr!";
        return;
    }

    if (nextDesktop->isActivated()) {
        m_animationIsRunning = false;
        currentDesktop->setPause(false);
        qWarning() << "[PeonyDesktopApplication::changePrimaryWindowDesktop] nextDesktop is activated!";
        return;
    }

    //断开发送请求桌面的链接，防止频繁发送消息
    disconnect(currentDesktop, &Peony::DesktopWidgetBase::moveToOtherDesktop, this, &PeonyDesktopApplication::changePrimaryWindowDesktop);

    QRect primaryScreenRect = m_primaryScreen->geometry();
    QRect currentDesktopStartRect = currentDesktop->geometry();

    QRect currentDesktopEndRect = this->createRectForAnimation(primaryScreenRect, currentDesktopStartRect,
                                                               currentDesktop->getExitAnimationType(), true);
    QRect nextDesktopStartRect = this->createRectForAnimation(primaryScreenRect, currentDesktopStartRect,
                                                              targetAnimation, false);

    //消失动画
    QPropertyAnimation *exitAnimation = this->createPropertyAnimation(currentDesktop->getExitAnimationType(),
                                                                      currentDesktop,
                                                                      currentDesktopStartRect,
                                                                      currentDesktopEndRect);

    //出现动画
    QPropertyAnimation *showAnimation = this->createPropertyAnimation(targetAnimation, nextDesktop,
                                                                      nextDesktopStartRect, primaryScreenRect);
    QParallelAnimationGroup *animationGroup = new QParallelAnimationGroup(this);
    animationGroup->addAnimation(exitAnimation);
    animationGroup->addAnimation(showAnimation);

    connect(animationGroup, &QSequentialAnimationGroup::finished, this, [=] {
        qDebug() << "[PeonyDesktopApplication::changePrimaryWindowDesktop] animation is stop";
        m_animationIsRunning = false;
        primaryWindow->setWindowDesktop(nextDesktop);
        connect(nextDesktop, &Peony::DesktopWidgetBase::moveToOtherDesktop, this, &PeonyDesktopApplication::changePrimaryWindowDesktop);
        animationGroup->clear();
        animationGroup->deleteLater();
        if (m_desktopDbusService) {
            Q_EMIT m_desktopDbusService->desktopChangedSignal(nextDesktop->getDesktopType());
        }
    });

    nextDesktop->beforeInitDesktop();
    if (this->getPropertyNameByAnimation(targetAnimation) == PropertyName::WindowOpacity) {
        //透明度动画的开始位置就在屏幕上
        nextDesktop->setGeometry(primaryScreenRect);
    } else {
        nextDesktop->setGeometry(nextDesktopStartRect);
        nextDesktop->showDesktop();
        nextDesktop->onPrimaryScreenChanged();
    }

    animationGroup->start();
}

void PeonyDesktopApplication::startScreenshotAnimation(Peony::DesktopBackgroundWindow *primaryWindow,
                                                       Peony::DesktopWidgetBase *currentDesktop,
                                                       Peony::DesktopWidgetBase *nextDesktop)
{
    bool currentCustom = currentDesktop->hasCustomAnimation();
    bool nextCustom = false;
    int duration = DesktopGlobalSettings::globalInstance()->getValue(DESKTOP_ANIMATION_DURATION).toInt();
    QRect rect(0, 0, primaryWindow->width(), primaryWindow->height());

    if (nextDesktop) {
        nextCustom = nextDesktop->hasCustomAnimation();

        nextDesktop->setGeometry(rect);
        nextDesktop->getRealDesktop()->setGeometry(rect);
        nextDesktop->beforeInitDesktop();
        nextDesktop->onPrimaryScreenChanged();
    }

    disconnect(currentDesktop, &Peony::DesktopWidgetBase::moveToOtherDesktop, this,
               &PeonyDesktopApplication::changePrimaryWindowDesktop);

    if (currentCustom && nextCustom) {
        currentDesktop->startAnimation(false);
        QTimer::singleShot(duration, this, [=]() {
            connect(nextDesktop, &Peony::DesktopWidgetBase::moveToOtherDesktop, this,
                                             &PeonyDesktopApplication::changePrimaryWindowDesktop);

            primaryWindow->setWindowDesktop(nextDesktop);
            m_animationIsRunning = false;
        });
        return;
    }

    QPixmap frontend;
    QPixmap backend;
    auto animationDesktop = new Peony::AnimationWidget(primaryWindow);

    if (!currentCustom && nextCustom) {
        //下一个桌面是自定义
        frontend = currentDesktop->generatePixmap();
        //运行时间减半
        animationDesktop->half();
        animationDesktop->initAnimation(frontend, backend);

        connect(animationDesktop, &AnimationWidget::finished, this, [=] {
            connect(nextDesktop, &Peony::DesktopWidgetBase::moveToOtherDesktop, this,
                    &PeonyDesktopApplication::changePrimaryWindowDesktop);
            primaryWindow->setWindowDesktop(nextDesktop);

            QTimer::singleShot(duration, this, [=]() {
                m_animationIsRunning = false;
            });
        });

        primaryWindow->setWindowDesktop(animationDesktop);
        animationDesktop->start();

    } else if (currentCustom && !nextCustom) {
        //两种情况，获取到的下一个桌面为空或者不存在自定义动画
        if (nextDesktop) {
            frontend = nextDesktop->generatePixmap();
            //从小到大，反转时间线。反转时间线后，时间自动减半
            animationDesktop->reverse();
            animationDesktop->initAnimation(frontend, backend);

            connect(animationDesktop, &AnimationWidget::finished, this, [=] {
                connect(nextDesktop, &Peony::DesktopWidgetBase::moveToOtherDesktop, this,
                        &PeonyDesktopApplication::changePrimaryWindowDesktop);
                primaryWindow->setWindowDesktop(nextDesktop);
                m_animationIsRunning = false;
            });

            currentDesktop->startAnimation(false);
            QTimer::singleShot(duration, this, [=]() {
                primaryWindow->setWindowDesktop(animationDesktop);
                animationDesktop->start();
            });

        } else {
            connect(currentDesktop, &Peony::DesktopWidgetBase::moveToOtherDesktop, this,
                       &PeonyDesktopApplication::changePrimaryWindowDesktop);
            m_animationIsRunning = false;
            delete animationDesktop;
            return;
        }
    } else {
        //两种情况，获取到的下一个桌面为空或者
        frontend = currentDesktop->generatePixmap();
        if (nextDesktop) {
            backend = nextDesktop->generatePixmap();
        }
        //完全接管截图动画,不需要反转也不需要减半
        animationDesktop->initAnimation(frontend, backend);

        primaryWindow->setWindowDesktop(animationDesktop);

        connect(animationDesktop, &AnimationWidget::finished, this, [=] {
            if (nextDesktop) {
                connect(nextDesktop, &Peony::DesktopWidgetBase::moveToOtherDesktop, this,
                        &PeonyDesktopApplication::changePrimaryWindowDesktop);
                primaryWindow->setWindowDesktop(nextDesktop);
            } else {
                connect(currentDesktop, &Peony::DesktopWidgetBase::moveToOtherDesktop, this,
                        &PeonyDesktopApplication::changePrimaryWindowDesktop);
                primaryWindow->setWindowDesktop(currentDesktop);
            }
            m_animationIsRunning = false;
        });

        animationDesktop->start();
    }
}

QRect PeonyDesktopApplication::createRectForAnimation(QRect &screenRect, QRect &currentDesktopRect, AnimationType animationType, bool isExit)
{
    QRect nextRect;
    if (isExit) {
        switch (animationType) {
            case AnimationType::LeftToRight:
                nextRect = QRect(screenRect.width(), 0, screenRect.width(), screenRect.height());
                break;
            case AnimationType::RightToLeft:
                nextRect = QRect(-screenRect.width(), 0, screenRect.width(), screenRect.height());
                break;
            case AnimationType::CenterToEdge:
                nextRect = screenRect;
                break;
            case AnimationType::EdgeToCenter: {
                //rect的高度为窗口高度的四分之一
                quint32 x = double(screenRect.width() * 1.0 * 0.375);
                quint32 y = double(screenRect.height() * 1.0 * 0.375);
                quint32 w = screenRect.width() * 0.25;
                quint32 h = screenRect.height() * 0.25;
                nextRect = QRect(x, y, w, h);
                break;
            }
            case AnimationType::OpacityFull:
            case AnimationType::OpacityLess:
            default:
                nextRect = screenRect;
                break;
        }
    } else {
        switch (animationType) {
            case AnimationType::LeftToRight:
                nextRect = QRect(-screenRect.width(), 0, screenRect.width(), screenRect.height());
                break;
            case AnimationType::RightToLeft:
                nextRect = QRect(screenRect.width(), 0, screenRect.width(), screenRect.height());
                break;
            case AnimationType::CenterToEdge: {
                //起点rect的高度为窗口高度的四分之一
                quint32 x = double(screenRect.width() * 1.0 * 0.375);
                quint32 y = double(screenRect.height() * 1.0 * 0.375);
                quint32 w = screenRect.width() * 0.25;
                quint32 h = screenRect.height() * 0.25;
                nextRect = QRect(x, y, w, h);
                break;
            }
            case AnimationType::EdgeToCenter:
                nextRect = screenRect;
                break;
            case AnimationType::OpacityFull:
            case AnimationType::OpacityLess:
            default:
                nextRect = screenRect;
                break;
        }
    }

    return nextRect;
}

QPropertyAnimation *PeonyDesktopApplication::createPropertyAnimation(AnimationType animationType, Peony::DesktopWidgetBase *object, QRect &startRect, QRect &endRect)
{
    //动画时间 xx ms
    quint32 duration = 500;

    PropertyName propertyName = this->getPropertyNameByAnimation(animationType);
    //TODO 添加并实现其他动画类型 ...
    //TODO 尝试改为更高级的动画框架实现

    QPropertyAnimation *animation = nullptr;

    switch (propertyName) {
        case PropertyName::Pos: {
            animation = new QPropertyAnimation(object, "pos");

            animation->setStartValue(QPoint(startRect.x(), startRect.y()));
            animation->setEndValue(QPoint(endRect.x(), endRect.y()));
            animation->setDuration(duration);
            animation->setEasingCurve(QEasingCurve::InOutCubic);
            break;
        }
        case PropertyName::Geometry: {
            animation = new QPropertyAnimation(object, "geometry");

            animation->setStartValue(startRect);
            animation->setEndValue(endRect);
            animation->setDuration(duration);
            animation->setEasingCurve(QEasingCurve::InOutCubic);
            break;
        }
        case PropertyName::WindowOpacity: {
            QGraphicsOpacityEffect *opacityEffect = new QGraphicsOpacityEffect(object);

            animation = new QPropertyAnimation(opacityEffect, "opacity");

            if (animationType == AnimationType::OpacityFull) {
                opacityEffect->setOpacity(0);
                object->setWindowOpacity(0);
                animation->setStartValue(0);
                animation->setEndValue(1);

            } else if (animationType == AnimationType::OpacityLess) {
                opacityEffect->setOpacity(1);
                object->setWindowOpacity(1.0);
                animation->setStartValue(1);
                animation->setEndValue(0);
            }

            object->setGraphicsEffect(opacityEffect);
            animation->setEasingCurve(QEasingCurve::InOutCubic);
            animation->setDuration(duration);

            break;
        }
        default:
            break;
    }

    return animation;
}

PropertyName PeonyDesktopApplication::getPropertyNameByAnimation(AnimationType animationType)
{
    //出现动画类型
    switch (animationType) {
        case AnimationType::LeftToRight:
        case AnimationType::RightToLeft:
            return PropertyName::Pos;

        case AnimationType::EdgeToCenter:
        case AnimationType::CenterToEdge:
            return PropertyName::Geometry;

        case AnimationType::OpacityFull:
        case AnimationType::OpacityLess:
            return PropertyName::WindowOpacity;

        default:
            return PropertyName::Pos;
    }
}

Peony::DesktopWidgetBase *PeonyDesktopApplication::getNextDesktop(DesktopType targetType, Peony::DesktopBackgroundWindow *parentWindow)
{
    m_mutex.lock();
    //获取一个桌面并指定父窗口
    Peony::DesktopWidgetBase *nextDesktop = m_desktopManager->getDesktopByType(targetType, parentWindow);

    if (targetType == DesktopType::StudyCenter) {
        if (m_learningCenterActivated) {
            if (m_isTabletMode) {
                nextDesktop = m_desktopManager->getDesktopByType(DesktopType::Tablet, parentWindow);
            } else {
                nextDesktop = m_desktopManager->getDesktopByType(DesktopType::Desktop, parentWindow);
            }
            m_learningCenterActivated = false;
        } else {
            m_learningCenterActivated = true;
        }
    } else {
        //切换其他模式桌面时，将标志位置 false
        m_learningCenterActivated = false;
    }
    m_mutex.unlock();

    return nextDesktop;
}

void PeonyDesktopApplication::changeDesktop()
{
    AnimationType type = AnimationType::LeftToRight;
    if (DesktopGlobalSettings::globalInstance()->getCurrentProjectName() != V10_SP1_EDU) {
        type = AnimationType::CenterToEdge;
    }

    if (m_windowManager->getWindowByScreen(m_primaryScreen)->getCurrentDesktop()->getDesktopType() == DesktopType::StudyCenter) {
        this->changePrimaryWindowDesktop(DesktopType::StudyCenter, type);
        return;
    }
    if (m_isTabletMode) {
        this->changePrimaryWindowDesktop(DesktopType::Tablet, type);
    } else {
        this->changePrimaryWindowDesktop(DesktopType::Desktop, type);
    }
}

void PeonyDesktopApplication::initGSettings()
{
    m_isTabletMode = false; //默认值 false
//    if (QGSettings::isSchemaInstalled(TABLET_SCHEMA)) {
//        m_tabletModeGSettings = new QGSettings(TABLET_SCHEMA);
//        m_isTabletMode = m_tabletModeGSettings->get(TABLET_MODE).toBool();
//
//        connect(m_tabletModeGSettings, &QGSettings::changed, this, [=](const QString &key) {
//            if (key == TABLET_MODE) {
//                m_isTabletMode = m_tabletModeGSettings->get(key).toBool();
//                this->changeDesktop();
//            }
//        });
//    }

    //dbus
    m_statusManagerDBus = new QDBusInterface(DBUS_STATUS_MANAGER_IF, "/" ,DBUS_STATUS_MANAGER_IF,QDBusConnection::sessionBus(),this);
    qDebug() << "[PeonyDesktopApplication::initGSettings] init statusManagerDBus" << m_statusManagerDBus->isValid();
    if (m_statusManagerDBus) {
        if (m_statusManagerDBus->isValid()) {
            //平板模式切换
            connect(m_statusManagerDBus, SIGNAL(mode_change_signal(bool)), this, SLOT(updateTabletModeValue(bool)));
        }
    }

//    QDBusConnection::sessionBus().connect(DBUS_STATUS_MANAGER_IF, "/" ,DBUS_STATUS_MANAGER_IF,"mode_change_signal",
//                                          this, SLOT(updateTabletModeValue(bool)));

    this->updateGSettingValues();
}

void PeonyDesktopApplication::updateTabletModeValue(bool mode)
{
    m_isTabletMode = mode;

    this->changeDesktop();
}

void PeonyDesktopApplication::updateGSettingValues()
{
//    if (m_tabletModeGSettings) {
//        m_isTabletMode = m_tabletModeGSettings->get(TABLET_MODE).toBool();
//    }

    if (m_statusManagerDBus) {
        QDBusReply<bool> message_a = m_statusManagerDBus->call("get_current_tabletmode");
        qDebug() << "[PeonyDesktopApplication::updateGSettingValues] get_current_tabletmode：" << message_a.value();;
        if (message_a.isValid()) {
            m_isTabletMode = message_a.value();
        }
    }
}

void PeonyDesktopApplication::initDBusService()
{
    if (DesktopGlobalSettings::globalInstance()->getCurrentProjectName() == V10_SP1_EDU) {
        m_desktopDbusService = new PeonyDesktopDbusService(this);
    }
}
