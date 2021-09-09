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
#include "peony-log.h"
#include "desktop-global-settings.h"
#include "peony-desktop-dbus-service.h"

#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QTimer>
#include <QGraphicsEffect>
#include <QPushButton>
#include <QStatusBar>

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

    PEONY_DESKTOP_LOG_WARN("PeonyDesktopApplication enter..........");

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
        PEONY_DESKTOP_LOG_WARN("it is Primary screen");
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
    }

    connect(this, &SingleApplication::layoutDirectionChanged, this, &PeonyDesktopApplication::layoutDirectionChangedProcess);
    connect(this, &SingleApplication::primaryScreenChanged, this, &PeonyDesktopApplication::primaryScreenChangedProcess);
    connect(this, &SingleApplication::screenAdded, this, &PeonyDesktopApplication::screenAddedProcess);
    connect(this, &SingleApplication::screenRemoved, this, &PeonyDesktopApplication::screenRemovedProcess);

    //parse cmd
    PEONY_DESKTOP_LOG_WARN("before parse cmd............");
    auto message = this->arguments().join(' ').toUtf8();
    parseCmd(this->instanceId(), message, isPrimary());

    auto volumeManager = Peony::VolumeManager::getInstance();
    connect(volumeManager,&Peony::VolumeManager::mountAdded,this,[=](const std::shared_ptr<Peony::Mount> &mount){
        //auto open dir for inserted dvd.
        GMount *newMount = (GMount*)g_object_ref(mount->getGMount());
        g_mount_guess_content_type(newMount,FALSE,NULL,guessContentTypeCallback,NULL);
    });
    connect(volumeManager,&Peony::VolumeManager::volumeRemoved,this,&PeonyDesktopApplication::volumeRemovedProcess);
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

    QCommandLineOption studyOption(QStringList()<<"s"<<"study", tr("Open learning center."));
    parser.addOption(studyOption);

    if (isPrimary) {
        PEONY_DESKTOP_LOG_WARN("parse cmd: it is primary screen");
        if (m_firstParse) {
            auto helpOption = parser.addHelpOption();
            auto versionOption = parser.addVersionOption();
            m_firstParse = false;
            //第一次启动桌面时，加载管理器
            this->initManager();
            //初始化gsetting
            this->initGSettings();
        }

        Q_UNUSED(id)
        const QStringList args = QString(msg).split(' ');

        parser.process(args);
        if (parser.isSet(quitOption)) {
            QTimer::singleShot(1, [=]() {
                PEONY_DESKTOP_LOG_WARN("peony desktop exited");
                qApp->quit();
            });
            return;
        }

        if (parser.isSet(daemonOption)) {
            if (!has_daemon) {
                qDebug()<<"-d";
                PEONY_DESKTOP_LOG_WARN("has paramer d");

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

        this->updateGSettingValues();

        if (parser.isSet(desktopOption)) {
            //TODO 2021.08.07 判断当前系统类型，桌面模式加载桌面图标模式，平板模式加载平板模式桌面
            if (!has_desktop) {
                PEONY_DESKTOP_LOG_WARN("has parameter w");
                //FIXME: load menu plugin
                //FIXME: take over desktop displaying
                for(auto screen : this->screens())
                {
                    addWindow(screen);
                }

                this->changePrimaryWindowDesktop(DesktopType::StudyCenter, AnimationType::LeftToRight);
            }
            has_desktop = true;
        }

        if (parser.isSet(studyOption)) {
            if (!has_desktop) {
                qWarning() << "[PeonyDesktopApplication::parseCmd] peony-qt-desktop is not running!";
                QTimer::singleShot(1, [=]() {
                    PEONY_DESKTOP_LOG_WARN("peony desktop exited");
                    qApp->quit();
                });
                return;
            } else {
                this->changePrimaryWindowDesktop(DesktopType::StudyCenter, AnimationType::LeftToRight);
            }
        }

        connect(this, &QApplication::paletteChanged, this, [=](const QPalette &pal) {
            for (auto w : allWidgets()) {
                w->setPalette(pal);
                w->update();
            }
        });
    } else {
        PEONY_DESKTOP_LOG_WARN("parse cmd: it is not primary screen");
        auto helpOption = parser.addHelpOption();
        auto versionOption = parser.addVersionOption();

        if (arguments().count() < 2) {
            parser.showHelp();
        }
        parser.process(arguments());

        sendMessage(msg);
    }
}

void PeonyDesktopApplication::addWindow(QScreen *screen, bool checkPrimary)
{
    Peony::DesktopWindow *window;
    if (checkPrimary) {
        bool isPrimary = isPrimaryScreen(screen);
        window = m_windowManager->createWindowForScreen(screen, isPrimary);
        if (isPrimary)
        {
            DesktopWidgetBase *desktop = nullptr;
            //通过系统类型获取桌面
            qDebug() << "===当前桌面模式：" << m_isTabletMode;
            if(m_isTabletMode) {
                //平板模式
                desktop = m_desktopManager->getDesktopByType(DesktopType::Tablet);
            } else {
                //桌面模式
                desktop = m_desktopManager->getDesktopByType(DesktopType::Desktop);
            }
            window->setWindowDesktop(desktop);
            window->updateView();
//            m_windowManager->updateWindowView(screen);
            m_primaryScreen = screen;
            connect(desktop, &DesktopWidgetBase::moveToOtherDesktop, this, &PeonyDesktopApplication::changePrimaryWindowDesktop);
            //connect(window, &Peony::DesktopWindow::changeBg, this, &PeonyDesktopApplication::changeBgProcess);
        }
    } else {
        window = m_windowManager->createWindowForScreen(screen, false);
    }

    connect(window, &Peony::DesktopWindow::checkWindow, this, &PeonyDesktopApplication::checkWindowProcess);
    //window->showFullScreen();
    //m_window_list<<window;

    m_windowManager->updateAllWindowGeometry();
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
    Peony::DesktopWindow *rawPrimaryWindow = m_windowManager->getWindowByScreen(m_primaryScreen);
    Peony::DesktopWindow *currentPrimaryWindow = m_windowManager->getWindowByScreen(screen);
//    for(auto win : m_window_list)
//    {
//        if (win->centralWidget())
//            rawPrimaryWindow = win;
//
//        if (win->getScreen() == screen)
//            currentPrimayWindow = win;
//        //need exchange window screen
////        if (win->getView() && win->getScreen() != screen)
////        {
////            preMainScreen = win->getScreen();
////            need_exchange = true;
////            break;
////        }
//    }

    if (rawPrimaryWindow && currentPrimaryWindow) {
        //TODO 主屏幕切换时，将桌面交换，并且将背景图片也进行交换（当每个窗口能单独设置背景时），将能交换的属性都交换
        m_primaryScreen = screen;

        //主桌面
        DesktopWidgetBase *primaryDesktop = rawPrimaryWindow->getCurrentDesktop();
        //副桌面
        DesktopWidgetBase *secondaryDesktop = currentPrimaryWindow->getCurrentDesktop();

        rawPrimaryWindow->setWindowDesktop(secondaryDesktop);
        currentPrimaryWindow->setWindowDesktop(primaryDesktop);

        rawPrimaryWindow->updateView();
        currentPrimaryWindow->updateView();
        PEONY_DESKTOP_LOG_WARN("current primay win hide and show");

        rawPrimaryWindow->hide();
        currentPrimaryWindow->hide();

        //rawPrimaryWindow->show();
        currentPrimaryWindow->show();
    }
    return;
    //TODO 安全删除以下代码 20210817
    //do not check window need exchange
    //cause we always put the desktop icon view into window
    //which in current primary screen.
    if (need_exchange)
    {
        for(auto win : m_windowManager->windowList())
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
    if (screen != nullptr) {
        if (screen->geometry().topLeft() != m_primaryScreen->geometry().topLeft()) {
            //屏幕设置为镜像屏时，不添加窗口
            PEONY_DESKTOP_LOG_WARN("screen add process");
            this->addWindow(screen, false);
        }
        qDebug() << "screenAdded" << screen->name() << screen << m_windowManager->getWindowNumber()
                 << screen->availableSize();
    }
}

void PeonyDesktopApplication::screenRemovedProcess(QScreen *screen)
{
    //if (screen != nullptr)
    //qDebug()<<"screenRemoved"<<screen->name()<<screen->serialNumber();

    if (screen == m_primaryScreen) {
        //TODO 主屏被移除,将桌面移动到别的屏幕
    }

    m_windowManager->removeWindowByScreen(screen);
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
    //TODO 安全删除以下代码 20210817
    for(auto win : m_windowManager->windowList())
    {
        //fix duplicate screen cover main screen view problem
        if (win->getScreen() != this->primaryScreen())
        {
            if (win->getScreen()->geometry() == this->primaryScreen()->geometry())
            {
                PEONY_DESKTOP_LOG_WARN("win set visible false");
                win->setVisible(false);
            }
            else {
                win->setVisible(true);
                PEONY_DESKTOP_LOG_WARN("win set visible true");
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
}

void PeonyDesktopApplication::initManager()
{
    m_windowManager = WindowManager::getInstance(this);
    m_desktopManager = DesktopManager::getInstance(true,this);
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
    if (m_animationIsRunning) {
        return;
    }
    m_animationIsRunning = true;
    this->updateGSettingValues();
    //NOTE 只在主屏幕上切换桌面
    qDebug() << "[PeonyDesktopApplication::changePrimaryWindowDesktop] start process";
    //TODO 在切换过程中不准打开开始菜单
    //桌面模式打开开始菜单
    //桌面模式切换平板模式
    //桌面模式切换学习中心

    //平板模式切换桌面模式
    //平板模式切换学习中心

    //学习中心切换平板模式
    //学习中心切换桌面模式

    DesktopWindow *primaryWindow = m_windowManager->getWindowByScreen(m_primaryScreen);
    if (!primaryWindow) {
        m_animationIsRunning = false;
        qWarning() << "[PeonyDesktopApplication::changePrimaryWindowDesktop] primary window not found!";
        return;
    }

    DesktopWidgetBase *currentDesktop = primaryWindow->getCurrentDesktop();

    if (!currentDesktop) {
        m_animationIsRunning = false;
        qWarning() << "[PeonyDesktopApplication::changePrimaryWindowDesktop] primary window desktop not found!";
        return;
    }

    //TODO 检测这两个桌面的类型是否相同，相同则 return

    //获取一个桌面并指定父窗口
    DesktopWidgetBase *nextDesktop = getNextDesktop(targetType, primaryWindow);

    if (!nextDesktop) {
        m_animationIsRunning = false;
        qWarning() << "[PeonyDesktopApplication::changePrimaryWindowDesktop] nextDesktop is nullptr!";
        return;
    }

    if (nextDesktop->isActivated()) {
        m_animationIsRunning = false;
        qWarning() << "[PeonyDesktopApplication::changePrimaryWindowDesktop] nextDesktop is activated!";
        return;
    }
    //保存原始效果以解决动画冲突问题
//    QWidget *currentEffectBackup = saveEffectWidget(currentDesktop);
//    QWidget *nextEffectBackup    = saveEffectWidget(nextDesktop);

    //断开发送请求桌面的链接，防止频繁发送消息
    disconnect(currentDesktop, &DesktopWidgetBase::moveToOtherDesktop, this, &PeonyDesktopApplication::changePrimaryWindowDesktop);

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
    showAnimation->setDuration(300);
    QSequentialAnimationGroup *animationGroup = new QSequentialAnimationGroup(this);
    animationGroup->addAnimation(exitAnimation);
    animationGroup->addAnimation(showAnimation);

    connect(animationGroup, &QSequentialAnimationGroup::finished, this, [=] {
        animationGroup->clear();
        animationGroup->deleteLater();
    });

    connect(exitAnimation, &QPropertyAnimation::finished, this, [=] {
//        currentDesktop->setGraphicsEffect(currentEffectBackup->graphicsEffect());
        currentDesktop->setActivated(false);
//        delete currentEffectBackup;
    });

    //TODO 在退出动画完成前将下一个桌面设置为低透明度，在桌面退出完成后，使用动画设置为不透明
    connect(showAnimation, &QPropertyAnimation::finished, this, [=] {
//        nextDesktop->setGraphicsEffect(nextEffectBackup->graphicsEffect());
        primaryWindow->setWindowDesktop(nextDesktop);

        m_windowManager->updateAllWindowGeometry();
        connect(nextDesktop, &DesktopWidgetBase::moveToOtherDesktop, this, &PeonyDesktopApplication::changePrimaryWindowDesktop);
//        delete nextEffectBackup;
        m_animationIsRunning = false;
    });

    if (this->getPropertyNameByAnimation(targetAnimation) == PropertyName::WindowOpacity) {
        //透明度动画的开始位置就在屏幕上
        nextDesktop->setGeometry(primaryScreenRect);
    } else {
        nextDesktop->setGeometry(nextDesktopStartRect);
//        nextDesktop->setHidden(false);
    }

    animationGroup->start();
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
                nextRect = QRect((currentDesktopRect.x() - screenRect.width()), 0, screenRect.width(), screenRect.height());
                break;
            case AnimationType::RightToLeft:
                nextRect = QRect((currentDesktopRect.x() + screenRect.width()), 0, screenRect.width(), screenRect.height());
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

QPropertyAnimation *PeonyDesktopApplication::createPropertyAnimation(AnimationType animationType, DesktopWidgetBase *object, QRect &startRect, QRect &endRect)
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

    qDebug() << "===createPropertyAnimation：" << animation->propertyName();
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

Peony::DesktopWidgetBase *PeonyDesktopApplication::getNextDesktop(DesktopType targetType, DesktopWindow *parentWindow)
{
    m_mutex.lock();
    //获取一个桌面并指定父窗口
    DesktopWidgetBase *nextDesktop = m_desktopManager->getDesktopByType(targetType, parentWindow);

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
    if (m_isTabletMode) {
        this->changePrimaryWindowDesktop(DesktopType::StudyCenter, AnimationType::LeftToRight);
    } else {
        this->changePrimaryWindowDesktop(DesktopType::Desktop, AnimationType::LeftToRight);
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

    PeonyDesktopDbusService *desktopDbusService = new PeonyDesktopDbusService(this);

    connect(desktopDbusService, &Peony::PeonyDesktopDbusService::blurBackGroundSignal, this, [=](quint32 status) {
        qDebug() << "[PeonyDesktopApplication::blurBackGroundSignal] received signal, blur:" << status;
        if (m_windowManager) {
            DesktopWindow *window = m_windowManager->getWindowByScreen(m_primaryScreen);
            if (status == 1) {
                //当需要模糊时才进行判断，否则取消模糊效果
                updateGSettingValues();
                if (m_isTabletMode) {
                    if (m_animationIsRunning) {
                        //当dbus信号到达时，使用多线程判断桌面切换动画是否还在进行中
                        QFutureWatcher<void> *watcher = new QFutureWatcher<void>;
                        connect(watcher, &QFutureWatcher<void>::finished, this, [=] {
                            window->blurBackground(true);
                            delete watcher;
                        });

                        QFuture<void> thread = QtConcurrent::run([&] {
                            QThread::msleep(50);
                            while (1) {
                                if (!m_animationIsRunning) {
                                    break;
                                }
                                qDebug() << "[blurBackGroundSignal] animation is running";
                                QThread::msleep(50);
                            }
                            qDebug() << "[blurBackGroundSignal] animation is over";
                        });
                        watcher->setFuture(thread);
                    } else {
                        window->blurBackground(true);
                    }
                }
            } else {
                window->blurBackground(false);
            }
        }
    });

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
