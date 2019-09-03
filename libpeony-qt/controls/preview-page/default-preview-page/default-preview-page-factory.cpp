#include "default-preview-page-factory.h"
#include "default-preview-page.h"

using namespace Peony;

DefaultPreviewPageFactory *globalInstance = nullptr;

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

QWidget *DefaultPreviewPageFactory::createPreviewPage(const QString &uri, PreviewType type)
{
    Q_UNUSED(type)
    return new DefaultPreviewPage(uri);
}
