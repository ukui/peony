#include "tablet-side-bar-factory.h"

#include "intel-navigation-side-bar.h"

using namespace Peony::Intel;
using namespace Intel;

TabletSideBarFactory::TabletSideBarFactory(QObject *parent) : QObject(parent)
{

}

const QStringList TabletSideBarFactory::keys()
{
    QStringList keys;
    keys.append("V10SP1-edu");
    keys.append("Intel");
    return keys;
}

Peony::SideBar *TabletSideBarFactory::create(QWidget *parent)
{
    return new NavigationSideBarContainer(parent);
}

Peony::PluginInterface::PluginType TabletSideBarFactory::pluginType()
{
    return SideBarPlugin;
}

const QString TabletSideBarFactory::name()
{
    return "Intel";
}

const QString TabletSideBarFactory::description()
{
    return tr("Intel Side Bar");
}

const QIcon TabletSideBarFactory::icon()
{
    return QIcon::fromTheme("file-manager");
}

void TabletSideBarFactory::setEnable(bool enable)
{

}

bool TabletSideBarFactory::isEnable()
{
    return true;
}
