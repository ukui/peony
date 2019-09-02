#include "menuplugin.h"
#include <QDebug>

using namespace Peony;

QString MenuPluginTest1::testPlugin()
{
    qDebug()<<"menu test plugin1";
    return QString("MenuPluginTest1");
}

QList<QAction *> MenuPluginTest1::menuActions(Type type, const QStringList &uris)
{
    Q_UNUSED(type);
    Q_UNUSED(uris);
    QList<QAction *> actions;
    switch (type) {
    case MenuInterface::File: {
        QAction *action = new QAction(QIcon::fromTheme("search"), tr("test"));
        actions<<action;
        return actions;
    }
    case MenuInterface::Volume: {
        QAction *action = new QAction(QIcon::fromTheme("media-eject"), tr("test-volume"));
        actions<<action;
        return actions;
    }
    default:
        return actions;
    }
}
