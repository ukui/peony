#include "mainwindow.h"
#include <QApplication>
//#include "gobject-template.h"

#include "side-bar-model.h"
#include "side-bar-abstract-item.h"
#include <QTreeView>
#include <QHeaderView>
#include "volume-manager.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    Peony::SideBarModel *model = new Peony::SideBarModel;
    QTreeView *v = new QTreeView;
    QObject::connect(v, &QTreeView::expanded, [=](const QModelIndex &parent){
        auto item = static_cast<Peony::SideBarAbstractItem*>(parent.internalPointer());
        item->findChildrenAsync();
    });
    QObject::connect(v, &QTreeView::collapsed, [=](const QModelIndex &parent){
        auto item = static_cast<Peony::SideBarAbstractItem*>(parent.internalPointer());
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
    v->setModel(model);
    v->setExpandsOnDoubleClick(false);
    //v->setAnimated(true);
    v->show();
    v->expand(model->index(0, 0));
    v->expand(model->index(1, 0));
    v->expand(model->index(2, 0));
    v->header()->setSectionResizeMode(QHeaderView::ResizeToContents);

    return a.exec();
}
