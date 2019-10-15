#include "menuplugin.h"
#include <QDebug>

#include <QMenu>

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
    QMenu *menu = new QMenu(action->parentWidget());
    connect(action, &QAction::destroyed, [=](){
        qDebug()<<"delete sub menu";
        menu->deleteLater();
    });
    menu->addAction("sub test1");
    menu->addSeparator();
    menu->addAction("sub test2");
    action->setMenu(menu);
    QAction *action2 = new QAction(QIcon::fromTheme("media-eject"), tr("test-volume"));
    connect(action2, &QAction::triggered, [=](){
        qDebug()<<"action triggered";
    });
    actions<<action2;

    return actions;
}
