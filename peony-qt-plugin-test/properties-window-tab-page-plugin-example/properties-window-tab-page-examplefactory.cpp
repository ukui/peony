#include "properties-window-tab-page-examplefactory.h"
#include <QLabel>

PropertiesWindowTabPageExampleFactory::PropertiesWindowTabPageExampleFactory(QObject *parent) : QObject (parent)
{
}

PropertiesWindowTabPageExampleFactory::~PropertiesWindowTabPageExampleFactory()
{

}

QWidget *PropertiesWindowTabPageExampleFactory::createTabPage(const QStringList &uris)
{
    QStringList l = uris;
    auto label = new QLabel(tr("This page is an external plugin page.\n"
                               "uris:\n"
                               "%1").arg(l.join(",\n")));
    label->setWordWrap(true);
    return label;
}
