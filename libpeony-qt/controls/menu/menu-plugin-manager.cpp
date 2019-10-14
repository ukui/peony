#include "menu-plugin-manager.h"

#include <QDir>
#include <QPluginLoader>

#include <QDebug>
#include <QApplication>

using namespace Peony;

static MenuPluginManager *global_instance = nullptr;

MenuPluginManager::MenuPluginManager(QObject *parent) : QObject(parent)
{
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
        MenuPluginInterface *test = qobject_cast<MenuPluginInterface*>(plugin);
        qDebug()<<test->name();
        m_hash.insert(test->name(), test);
    }
}

MenuPluginManager::~MenuPluginManager()
{

}

MenuPluginManager *MenuPluginManager::getInstance()
{
    if (!global_instance) {
        global_instance = new MenuPluginManager;
    }
    return global_instance;
}

void MenuPluginManager::close()
{
    this->deleteLater();
}

const QStringList MenuPluginManager::getPluginIds()
{
    return m_hash.keys();
}

MenuPluginInterface *MenuPluginManager::getPlugin(const QString &pluginId)
{
    return m_hash.value(pluginId);
}
