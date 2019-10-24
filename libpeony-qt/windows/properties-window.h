#ifndef PROPERTIESWINDOW_H
#define PROPERTIESWINDOW_H

#include <QTabWidget>
#include "peony-core_global.h"

#include <QMap>

namespace Peony {

class PropertiesWindowTabPagePluginIface;
class PropertiesWindowPluginManager : public QObject
{
    friend class PropertiesWindow;
    Q_OBJECT

public:
    static PropertiesWindowPluginManager *getInstance();
    void release();

    bool registerFactory(PropertiesWindowTabPagePluginIface *factory);
    const QStringList getFactoryNames();
    PropertiesWindowTabPagePluginIface *getFactory(const QString &id);

private:
    explicit PropertiesWindowPluginManager(QObject *parent = nullptr);
    ~PropertiesWindowPluginManager() override;

    QHash<QString, PropertiesWindowTabPagePluginIface*> m_factory_hash;
    QMap<int, QString> m_sorted_factory_map;
};

class PEONYCORESHARED_EXPORT PropertiesWindow : public QTabWidget
{
    Q_OBJECT
public:
    explicit PropertiesWindow(const QStringList &uris, QWidget *parent = nullptr);

};

}

#endif // PROPERTIESWINDOW_H
