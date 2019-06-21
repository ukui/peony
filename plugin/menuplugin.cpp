#include "menuplugin.h"
#include <QDebug>

QString MenuPluginTest1::testPlugin()
{
    qDebug()<<"menu test plugin1";
    return QString("MenuPluginTest1");
}
