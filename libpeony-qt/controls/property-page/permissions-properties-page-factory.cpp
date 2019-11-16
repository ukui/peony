#include "permissions-properties-page-factory.h"
#include "permissions-properties-page.h"

#include <QLabel>

using namespace Peony;

PermissionsPropertiesPageFactory *global_instance = nullptr;

PermissionsPropertiesPageFactory::PermissionsPropertiesPageFactory(QObject *parent) : QObject(parent)
{

}

bool PermissionsPropertiesPageFactory::supportUris(const QStringList &uris)
{
    return uris.count() == 1;
}

QWidget *PermissionsPropertiesPageFactory::createTabPage(const QStringList &uris)
{
    return new PermissionsPropertiesPage(uris);
}

void PermissionsPropertiesPageFactory::closeFactory()
{
    this->deleteLater();
}

PermissionsPropertiesPageFactory *PermissionsPropertiesPageFactory::getInstance()
{
    if (!global_instance)
        global_instance = new PermissionsPropertiesPageFactory;
    return global_instance;
}
