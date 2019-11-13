#include "gtk2-style-enhanced-plugin.h"
#include "complementary-style.h"

using namespace Peony;

Gtk2StyleEnhancedPlugin::Gtk2StyleEnhancedPlugin(QObject *parent) : QObject(parent)
{
}

QProxyStyle *Gtk2StyleEnhancedPlugin::getStyle()
{
    return ComplementaryStyle::getStyle();
}
