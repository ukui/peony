#include "menuplugin.h"
#include <QDebug>

using namespace Peony;

QString MenuPluginTest1::testPlugin()
{
    qDebug()<<"menu test plugin1";
    return QString("MenuPluginTest1");
}

QList<QAction *> MenuPluginTest1::menuActions(Types types, const QString &uri, const QStringList &selectionUris)
{
    Q_UNUSED(types);
    Q_UNUSED(uri);
    Q_UNUSED(selectionUris);
    QList<QAction *> actions;
    QAction *action = new QAction(QIcon::fromTheme("search"), tr("test"));
    actions<<action;
    QAction *action2 = new QAction(QIcon::fromTheme("media-eject"), tr("test-volume"));
    actions<<action2;

    return actions;
}
