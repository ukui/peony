#include "icon-view-factory.h"
#include "icon-view.h"

using namespace Peony;

static IconViewFactory *globalInstance = nullptr;
static IconViewFactory2 *globalInstance2 = nullptr;

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
    return new Peony::DirectoryView::IconView;
}

//Factory2
IconViewFactory2 *IconViewFactory2::getInstance()
{
    if (!globalInstance2) {
        globalInstance2 = new IconViewFactory2;
    }
    return globalInstance2;
}

IconViewFactory2::IconViewFactory2(QObject *parent) : QObject (parent)
{

}

IconViewFactory2::~IconViewFactory2()
{

}

DirectoryViewWidget *IconViewFactory2::create()
{
    return new Peony::DirectoryView::IconView2;
}
