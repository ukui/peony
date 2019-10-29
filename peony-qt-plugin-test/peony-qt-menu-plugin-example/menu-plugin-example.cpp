#include "menu-plugin-example.h"

#include <QAction>
#include <QMenu>

#include <QDebug>

using namespace Peony;

MenuPluginExample::MenuPluginExample(QObject *parent) : QObject(parent)
{
}

QString MenuPluginExample::testPlugin()
{
    qDebug()<<"menu test plugin1";
    return QString("MenuPluginExample");
}

QList<QAction *> MenuPluginExample::menuActions(Types types, const QString &uri, const QStringList &selectionUris)
{
    //return QList<QAction *>();
    Q_UNUSED(types);
    Q_UNUSED(uri);
    Q_UNUSED(selectionUris);
    QList<QAction *> actions;
    QAction *action = new QAction(QIcon::fromTheme("search"), tr("plugin-sub-menu test"));

    actions<<action;
    QMenu *menu = new QMenu(action->parentWidget());
    connect(action, &QAction::destroyed, [=](){
        qDebug()<<"delete sub menu";
        menu->deleteLater();
    });
    menu->addAction("sub test1");
    menu->addSeparator();
    menu->addAction("sub test2");
    action->setMenu(menu);
    QAction *action2 = new QAction(QIcon::fromTheme("media-eject"), tr("plugin-action test"));
    connect(action2, &QAction::triggered, [=](){
        qDebug()<<"action triggered";
    });
    actions<<action2;

    return actions;
}
