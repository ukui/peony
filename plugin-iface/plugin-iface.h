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
        Other
    };

    virtual ~PluginInterface() {}

    virtual PluginType pluginType() = 0;

    virtual QString name() = 0;
    virtual QString description() = 0;
    virtual QIcon icon() = 0;
    virtual void setEnable(bool enable) = 0;
    virtual bool isEnable() = 0;
};

}

#endif // PLUGINIFACE_H
