#include "icon-view-factory.h"
#include "icon-view.h"

using namespace Peony;

static IconViewFactory *globalInstance = nullptr;

IconViewFactory *IconViewFactory::getInstance()
{
    if (!globalInstance) {
        globalInstance = new IconViewFactory;
    }
    return globalInstance;
}

IconViewFactory::IconViewFactory(QObject *parent) : QObject (parent)
{

}

IconViewFactory::~IconViewFactory()
{

}

DirectoryViewIface *IconViewFactory::create()
{
    return nullptr;
}
