#include "mainwindow.h"
#include <QApplication>
//#include "gobject-template.h"

#include "side-bar-model.h"
#include "side-bar-abstract-item.h"
#include <QTreeView>
#include <QHeaderView>
#include "volume-manager.h"

#include "side-bar-proxy-filter-sort-model.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    Peony::SideBarModel *model = new Peony::SideBarModel;
    Peony::SideBarProxyFilterSortModel *proxy_model = new Peony::SideBarProxyFilterSortModel;
    proxy_model->setSourceModel(model);
    QTreeView *v = new QTreeView;
    QObject::connect(v, &QTreeView::expanded, [=](const QModelIndex &parent){
        auto item = proxy_model->itemFromIndex(parent);
        item->findChildrenAsync();
    });
    QObject::connect(v, &QTreeView::collapsed, [=](const QModelIndex &parent){
        auto item = proxy_model->itemFromIndex(parent);
        item->clearChildren();
    });

    QObject::connect(v, &QTreeView::clicked, [=](const QModelIndex &index){
        if (index.column() == 1) {
            //unmount?
            Peony::SideBarAbstractItem *item = static_cast<Peony::SideBarAbstractItem*>(index.internalPointer());
            if (item->isMounted()) {
                Peony::VolumeManager::unmount(item->uri());
            }
        }
    });
    v->setModel(proxy_model);
    //v->setExpandsOnDoubleClick(false);
    //v->setAnimated(true);
    v->show();
    v->expand(proxy_model->index(0, 0));
    v->expand(proxy_model->index(1, 0));
    v->expand(proxy_model->index(2, 0));
    //v->expandAll();
    v->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    v->header()->setVisible(false);

    return a.exec();
}
