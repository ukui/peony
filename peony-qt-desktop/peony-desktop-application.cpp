#include "peony-desktop-application.h"

#include "desktop-window.h"
#include "fm-dbus-service.h"

#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QTimer>

#include <QProcess>
#include <QFile>

static bool has_desktop = false;
static bool has_daemon = false;

PeonyDesktopApplication::PeonyDesktopApplication(int &argc, char *argv[], const char *applicationName) : SingleApplication (argc, argv, applicationName, true)
{
    if (this->isPrimary()) {
        connect(this, &SingleApplication::receivedMessage, [=](quint32 id, QByteArray msg){
            this->parseCmd(id, msg, true);
        });
        QFile file(":/desktop-icon-view.qss");
        file.open(QFile::ReadOnly);
        setStyleSheet(QString::fromLatin1(file.readAll()));
        file.close();
    }

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
        Q_UNUSED(id)
        const QStringList args = QString(msg).split(' ');

        parser.parse(args);
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
                Peony::DesktopWindow *window = new Peony::DesktopWindow;
                window->showFullScreen();
            }
            has_desktop = true;
        }
    } else {
        auto helpOption = parser.addHelpOption();
        auto versionOption = parser.addVersionOption();

        if (arguments().count() < 2) {
            parser.showHelp();
        }
        parser.process(arguments());

        sendMessage(msg);
    }

}
