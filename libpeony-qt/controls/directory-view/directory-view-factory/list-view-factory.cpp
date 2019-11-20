#include "list-view-factory.h"
#include "list-view.h"

using namespace Peony;

static ListViewFactory *globalInstance = nullptr;
static ListViewFactory2 *globalInstance2 = nullptr;

ListViewFactory *ListViewFactory::getInstance()
{
    if (!globalInstance) {
        globalInstance = new ListViewFactory;
    }
    return globalInstance;
}

ListViewFactory::ListViewFactory(QObject *parent) : QObject (parent)
{

}

ListViewFactory::~ListViewFactory()
{

}

DirectoryViewIface *ListViewFactory::create()
{
    return new Peony::DirectoryView::ListView;
}

//List View 2
ListViewFactory2 *ListViewFactory2::getInstance()
{
    if (!globalInstance2) {
        globalInstance2 = new ListViewFactory2;
    }
    return globalInstance2;
}

ListViewFactory2::ListViewFactory2(QObject *parent) : QObject (parent)
{

}

ListViewFactory2::~ListViewFactory2()
{

}

DirectoryViewWidget *ListViewFactory2::create()
{
    return new Peony::DirectoryView::ListView2;
}
