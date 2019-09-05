#include "default-preview-page-factory.h"
#include "default-preview-page.h"

using namespace Peony;

static DefaultPreviewPageFactory *globalInstance = nullptr;

DefaultPreviewPageFactory *DefaultPreviewPageFactory::getInstance()
{
    if (!globalInstance) {
        globalInstance = new DefaultPreviewPageFactory;
    }
    return globalInstance;
}

DefaultPreviewPageFactory::DefaultPreviewPageFactory(QObject *parent) : QObject(parent)
{

}

DefaultPreviewPageFactory::~DefaultPreviewPageFactory()
{

}

PreviewPageIface *DefaultPreviewPageFactory::createPreviewPage()
{
    return new DefaultPreviewPage;
}
