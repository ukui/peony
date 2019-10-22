#include "list-view-factory.h"
#include "list-view.h"

using namespace Peony;

static ListViewFactory *globalInstance = nullptr;

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
