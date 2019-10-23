#ifndef PLUGINIFACE_H
#define PLUGINIFACE_H

#include <QString>
#include <QIcon>

//#define PluginInterface_iid "org.ukui.peony-qt.PluginInterface"

namespace Peony {

class PluginInterface
{
public:
    enum PluginType
    {
        Invalid,
        MenuPlugin,
        PreviewPagePlugin,
        DirectoryViewPlugin,
        ToolBarPlugin,
        PropertiesWindowPlugin,
        Other
    };

    virtual ~PluginInterface() {}

    virtual PluginType pluginType() = 0;

    virtual const QString name() = 0;
    virtual const QString description() = 0;
    virtual const QIcon icon() = 0;
    virtual void setEnable(bool enable) = 0;
    virtual bool isEnable() = 0;
};

}

#endif // PLUGINIFACE_H
