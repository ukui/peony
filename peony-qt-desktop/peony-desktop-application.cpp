#include "peony-desktop-application.h"

#include "fm-dbus-service.h"

#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QTimer>

#include <QProcess>
#include <QFile>
#include <QLabel>

static bool has_desktop = false;
static bool has_daemon = false;

PeonyDesktopApplication::PeonyDesktopApplication(int &argc, char *argv[], const char *applicationName) : SingleApplication (argc, argv, applicationName, true)
{
    m_screen_list= this->screens();
    if (this->isPrimary()) {
        qDebug()<<"isPrimary screen";
        connect(this, &SingleApplication::receivedMessage, [=](quint32 id, QByteArray msg){
            this->parseCmd(id, msg, true);
        });
        QFile file(":/desktop-icon-view.qss");
        file.open(QFile::ReadOnly);
        setStyleSheet(QString::fromLatin1(file.readAll()));
        file.close();
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
                //FIXME: take over org.freedesktop.FileManager1
                Peony::FMDBusService *service = new Peony::FMDBusService(this);
                connect(service, &Peony::FMDBusService::showItemsRequest, [=](const QStringList &urisList){
                    QProcess p;
                    p.setProgram("peony-qt");
                    p.setArguments(QStringList()<<"--show-items"<<urisList);
                    p.startDetached();
                    qDebug()<<"1";
                });
                connect(service, &Peony::FMDBusService::showFolderRequest, [=](const QStringList &urisList){
                    QProcess p;
                    p.setProgram("peony-qt");
                    p.setArguments(QStringList()<<"--show-forders"<<urisList);
                    p.startDetached();
                    qDebug()<<"2";
                });
                connect(service, &Peony::FMDBusService::showItemPropertiesRequest, [=](const QStringList &urisList){
                    QProcess p;
                    p.setProgram("peony-qt");
                    p.setArguments(QStringList()<<"--show-properties"<<urisList);
                    p.startDetached();
                    qDebug()<<"3";
                });
            }
            has_daemon = true;
        }

        if (parser.isSet(desktopOption)) {
            if (!has_desktop) {
                //FIXME: load menu plugin
                //FIXME: take over desktop displaying
                for(auto screen : m_screen_list)
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

void PeonyDesktopApplication::addWindow(QScreen *screen)
{
    bool is_primary = isPrimaryScreen(screen);
    Peony::DesktopWindow *window = new Peony::DesktopWindow(screen, is_primary);
    window->showFullScreen();
    m_window_list<<window;
    if (is_primary)
    {
        connect(window, &Peony::DesktopWindow::changeBg, this, &PeonyDesktopApplication::changeBgProcess);
    }
}

void PeonyDesktopApplication::layoutDirectionChangedProcess(Qt::LayoutDirection direction)
{
    qDebug()<<"layoutDirectionChangedProcess"<<direction;
}

void PeonyDesktopApplication::primaryScreenChangedProcess(QScreen *screen)
{
    if (screen != nullptr)
        qDebug()<<"primaryScreenChangedProcess"<<screen->name()<<screen->size()<<screen->availableSize()<<screen->virtualGeometry();

    updateWindowGeometry();
}

void PeonyDesktopApplication::updateWindowGeometry()
{
    for (auto window : m_window_list) {
        qDebug()<<"updateWindowGeometry:"<<window->getScreen()->geometry()<<window->getScreen()->virtualGeometry();
        window->setGeometry(window->getScreen()->geometry());
        window->updateView();
    }
}

void PeonyDesktopApplication::screenAddedProcess(QScreen *screen)
{
    if (screen != nullptr)
        qDebug()<<"screenAdded"<<screen->name()<<screen<<m_window_list.size()<<screen->availableSize();

    addWindow(screen);
    if (! m_screen_list.contains(screen)) //new expanded screen
         m_screen_list<<screen;

    for(auto m_screen : m_screen_list)
    {
        qDebug()<<"screenAddedProcess m_screen:"<<m_screen->geometry()<<m_screen->virtualGeometry();
    }
    for(auto top_screen : this->screens())
    {
        if (top_screen)
            qDebug()<<"screenAddedProcess top_screen:"<<top_screen->geometry()<<top_screen->virtualGeometry();
    }
}

void PeonyDesktopApplication::screenRemovedProcess(QScreen *screen)
{
    if (screen != nullptr)
        qDebug()<<"screenRemoved"<<screen->name()<<screen->serialNumber();

    //window manage
    for(auto win :m_window_list)
    {
        if (win->getScreen() == screen)
        {
            m_window_list.removeOne(win);
            win->deleteLater();
        }
    }
    m_screen_list.removeOne(screen);
    for(auto m_screen : m_screen_list)
    {
        qDebug()<<"screenRemovedProcess:"<<m_screen->geometry()<<m_screen->virtualGeometry();
    }
    for(auto top_screen : this->screens())
    {
        if (top_screen)
            qDebug()<<"screenRemovedProcess top_screen:"<<top_screen->geometry()<<top_screen->virtualGeometry();
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
    for (auto win : m_window_list) {
        if (!isPrimaryScreen(win->getScreen()))
            win->setBg(bgPath);
    }
}
