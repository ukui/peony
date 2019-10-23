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

bool BasicPropertiesPageFactory::supportUri(const QString &uri)
{
    //FIXME:
    return true;
}

void BasicPropertiesPageFactory::closeFactory()
{
    deleteLater();
}

QWidget *BasicPropertiesPageFactory::createTabPage(const QString &uri)
{
    BasicPropertiesPage *p = new BasicPropertiesPage(uri);
    return p;
}
