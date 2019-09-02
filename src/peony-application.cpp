#include "peony-application.h"
#include "menu-iface.h"

#include <QDebug>
#include <QDir>
#include <QPluginLoader>
#include <QString>
#include <QMenu>

PeonyApplication::PeonyApplication(int &argc, char *argv[]) : QApplication (argc, argv)
{
    //check if first run
    //if not send message to server
    //else
    //  load plgin
    //  read from command line
    //  do with args
    QDir pluginsDir(qApp->applicationDirPath());
    qDebug()<<pluginsDir;
    pluginsDir.cdUp();
    pluginsDir.cd("testdir");
    pluginsDir.setFilter(QDir::Files);

    qDebug()<<pluginsDir.entryList().count();
    Q_FOREACH(QString fileName, pluginsDir.entryList(QDir::Files)) {
        qDebug()<<fileName;
        QPluginLoader pluginLoader(pluginsDir.absoluteFilePath(fileName));
        qDebug()<<pluginLoader.fileName();
        qDebug()<<pluginLoader.metaData();
        qDebug()<<pluginLoader.load();
        QObject *plugin = pluginLoader.instance();
        qDebug()<<"test start";
        if (plugin) {
            Peony::MenuInterface *iface = qobject_cast<Peony::MenuInterface *>(plugin);
            if (iface) {
                qDebug()<<iface->testPlugin();

                QWidget *widget = new QWidget;
                widget->setAttribute(Qt::WA_DeleteOnClose);
                QMenu *menu = new QMenu(widget);
                QStringList l;
                auto fileActions = iface->menuActions(Peony::MenuInterface::File, l);
                for (auto action : fileActions) {
                    action->setParent(menu);
                }
                menu->addActions(fileActions);

                auto volumeActions = iface->menuActions(Peony::MenuInterface::Volume, l);
                for (auto action: volumeActions) {
                    action->setParent(menu);
                }
                menu->addActions(volumeActions);

                widget->setContextMenuPolicy(Qt::CustomContextMenu);
                widget->connect(widget, &QWidget::customContextMenuRequested, [menu](const QPoint &pos){
                    //menu->exec(pos);
                    menu->exec(QCursor::pos());
                });
                widget->show();
            }
        }
        qDebug()<<"testEnd";
    }
}
