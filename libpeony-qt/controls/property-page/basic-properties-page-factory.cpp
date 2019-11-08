#include "basic-properties-page-factory.h"
#include "basic-properties-page.h"

using namespace Peony;

static BasicPropertiesPageFactory *global_instance = nullptr;

BasicPropertiesPageFactory *BasicPropertiesPageFactory::getInstance()
{
    if (!global_instance)
        global_instance = new BasicPropertiesPageFactory;
    return global_instance;
}

BasicPropertiesPageFactory::BasicPropertiesPageFactory(QObject *parent) : QObject(parent)
{

}

BasicPropertiesPageFactory::~BasicPropertiesPageFactory()
{

}

bool BasicPropertiesPageFactory::supportUris(const QStringList &uri)
{
    //FIXME:
    if (uri.first().contains("computer:///") || uri.first().contains("network:///") || uri.first().contains("trash:///"))
        return false;
    return true;
}

void BasicPropertiesPageFactory::closeFactory()
{
    deleteLater();
}

QWidget *BasicPropertiesPageFactory::createTabPage(const QStringList &uris)
{
    BasicPropertiesPage *p = new BasicPropertiesPage(uris);
    return p;
}
