#include "content-preview-page-factory.h"
#include "content-preview-page.h"

ContentPreviewPageFactory::ContentPreviewPageFactory(QObject *parent) : QObject (parent)
{

}

ContentPreviewPageFactory::~ContentPreviewPageFactory()
{

}

Peony::PreviewPageIface *ContentPreviewPageFactory::createPreviewPage()
{
    return new ContentPreviewPage;
}
