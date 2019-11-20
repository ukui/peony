#include "directory-view-extensions-test-plugin.h"
#include "directory-view-widget.h"

#include <QLabel>

#include <QVBoxLayout>

#include <QDebug>

using namespace Peony;

DirectoryViewExtensionsTestPlugin::DirectoryViewExtensionsTestPlugin(QObject *parent) : QObject(parent)
{

}

//void DirectoryViewExtensionsTestPlugin::fillDirectoryView(DirectoryViewWidget *view)
//{
//    auto layout = new QVBoxLayout(view);
//    layout->addWidget(new QLabel("test icon view", nullptr));
//    view->setLayout(layout);
//}

DirectoryViewWidget *DirectoryViewExtensionsTestPlugin::create()
{
    qDebug()<<"create directory view";
    auto w = new DirectoryViewWidget;
    auto layout = new QVBoxLayout(w);
    layout->addWidget(new QLabel("test directory view", w));
    w->setLayout(layout);

    return w;
}
