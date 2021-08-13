#ifndef OOBE_PLUGIN_INFACE_H
#define OOBE_PLUGIN_INFACE_H

#include <QtPlugin>
#include <functional>
#include <QString>
#include <QObject>

class QWidget;
class kyMenuPluginInterface
{

public:
    virtual ~kyMenuPluginInterface() {}
    virtual const QString name() = 0;
    virtual const QString description() = 0;
    virtual QWidget *createWidget(QWidget *parent)=0;
};
Q_DECLARE_INTERFACE (kyMenuPluginInterface, "org.kyMenuPlugin.AbstractInterface")
#endif
