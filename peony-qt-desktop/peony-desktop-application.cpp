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

#include "fm-dbus-service.h"
#include "desktop-menu-plugin-manager.h"

#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QTimer>

#include <QProcess>
#include <QFile>
#include <QLabel>
#include <QtConcurrent>

#include <QTranslator>
#include <QLocale>

#include <gio/gio.h>

static bool has_desktop = false;
static bool has_daemon = false;

void trySetDefaultFolderUrlHandler() {
    QtConcurrent::run([=](){
        GAppInfo *peony_qt = g_app_info_create_from_commandline("peony-qt",
                                                                nullptr,
                                                                G_APP_INFO_CREATE_SUPPORTS_URIS,
                                                                nullptr);
        g_app_info_set_as_default_for_type(peony_qt, "inode/directory", nullptr);
        g_object_unref(peony_qt);
    });
}

PeonyDesktopApplication::PeonyDesktopApplication(int &argc, char *argv[], const char *applicationName) : SingleApplication (argc, argv, applicationName, true)
{
    QTranslator *t = new QTranslator(this);
    t->load("/usr/share/libpeony-qt/libpeony-qt_"+QLocale::system().name());
    QApplication::installTranslator(t);
    QTranslator *t2 = new QTranslator(this);
    t2->load("/usr/share/peony-qt-desktop/peony-qt-desktop_"+QLocale::system().name());
    QApplication::installTranslator(t2);

    if (this->isPrimary()) {
        qDebug()<<"isPrimary screen";
        connect(this, &SingleApplication::receivedMessage, [=](quint32 id, QByteArray msg){
            this->parseCmd(id, msg, true);
        });
        QFile file(":/desktop-icon-view.qss");
        file.open(QFile::ReadOnly);
        setStyleSheet(QString::fromLatin1(file.readAll()));
        file.close();
        Peony::DesktopMenuPluginManager::getInstance();
    }

    connect(this, &SingleApplication::layoutDirectionChanged, this, &PeonyDesktopApplication::layoutDirectionChangedProcess);
    connect(this, &SingleApplication::primaryScreenChanged, this, &PeonyDesktopApplication::primaryScreenChangedProcess);
    connect(this, &SingleApplication::screenAdded, this, &PeonyDesktopApplication::screenAddedProcess);
    connect(this, &SingleApplication::screenRemoved, this, &PeonyDesktopApplication::screenRemovedProcess);

    //parse cmd
    auto message = this->arguments().join(' ').toUtf8();
    parseCmd(this->instanceId(), message, isPrimary());
}

void PeonyDesktopApplication::parseCmd(quint32 id, QByteArray msg, bool isPrimary)
{
    QCommandLineParser parser;

    QCommandLineOption quitOption(QStringList()<<"q"<<"quit", tr("Close the peony-qt desktop window"));
    parser.addOption(quitOption);

    QCommandLineOption daemonOption(QStringList()<<"d"<<"deamon", tr("Take over the dbus service."));
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
            QTimer::singleShot(1, [=](){
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
                connect(service, &Peony::FMDBusService::showItemsRequest, [=](const QStringList &urisList){
                    QProcess p;
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
                    p.setProgram("peony-qt");
                    p.setArguments(QStringList()<<"--show-items"<<urisList);
                    p.startDetached();
#else
                    p.startDetached("peony-qt", QStringList()<<"--show-items"<<urisList, nullptr);
#endif
                });
                connect(service, &Peony::FMDBusService::showFolderRequest, [=](const QStringList &urisList){
                    QProcess p;
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
                    p.setProgram("peony-qt");
                    p.setArguments(QStringList()<<"--show-folders"<<urisList);
                    p.startDetached();
#else
                    p.startDetached("peony-qt", QStringList()<<"--show-folders"<<urisList, nullptr);
#endif
                });
                connect(service, &Peony::FMDBusService::showItemPropertiesRequest, [=](const QStringList &urisList){
                    QProcess p;
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
                    p.setProgram("peony-qt");
                    p.setArguments(QStringList()<<"--show-properties"<<urisList);
                    p.startDetached();
#else
                    p.startDetached("peony-qt", QStringList()<<"--show-properties"<<urisList, nullptr);
#endif
                });
            }
            has_daemon = true;
        }

        if (parser.isSet(desktopOption)) {
            if (!has_desktop) {
                //FIXME: load menu plugin
                //FIXME: take over desktop displaying
                for(auto screen : this->screens())
                {
                    addWindow(screen);
                }
            }
            has_desktop = true;
        }
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
            connect(window, &Peony::DesktopWindow::changeBg, this, &PeonyDesktopApplication::changeBgProcess);
        }
    } else {
        window = new Peony::DesktopWindow(screen, false);
    }

    connect(window, &Peony::DesktopWindow::checkWindow, this, &PeonyDesktopApplication::checkWindowProcess);
    window->showFullScreen();
    m_window_list<<window;
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

    bool need_exchange = false;
    QScreen *preMainScreen = nullptr;
    for(auto win : m_window_list)
    {
        //need exchange window screen
        if (win->getView() && win->getScreen() != screen)
        {
            preMainScreen = win->getScreen();
            need_exchange = true;
            break;
        }
    }

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

    addWindow(screen, false);
}

void PeonyDesktopApplication::screenRemovedProcess(QScreen *screen)
{
    if (screen != nullptr)
        qDebug()<<"screenRemoved"<<screen->name()<<screen->serialNumber();

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
    if (screen == this->primaryScreen())
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
