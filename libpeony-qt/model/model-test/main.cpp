#include "mainwindow.h"
#include <QApplication>
#include <file-item-proxy-filter-sort-model.h>
#include <file-item-model.h>
#include <path-bar-model.h>
#include <path-completer.h>
#include <file-item.h>

#include <QListView>
#include <QLineEdit>
#include <QToolBar>
#include <QLabel>

#include <QStandardPaths>
#include <QRegExp>
#include <QDebug>

#include <QThread>

#include <search-vfs-register.h>

#include "side-bar-model.h"
#include "side-bar-proxy-filter-sort-model.h"
#include "side-bar-abstract-item.h"
#include <QTreeView>
#include <QHeaderView>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QMainWindow w;

    Peony::SideBarModel model;

    Peony::SideBarProxyFilterSortModel proxy_model;
    proxy_model.setSourceModel(&model);

    QTreeView v;
    v.setSortingEnabled(true);
    v.setExpandsOnDoubleClick(false);
    v.header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    v.header()->setVisible(false);

    v.setModel(&proxy_model);

    v.connect(&v, &QTreeView::expanded, [&](const QModelIndex &index){
        auto item = proxy_model.itemFromIndex(index);
        item->findChildrenAsync();
    });
    v.connect(&v, &QTreeView::collapsed, [&](const QModelIndex &index){
        auto item = proxy_model.itemFromIndex(index);
        item->clearChildren();
    });
    v.connect(&v, &QTreeView::clicked, [&](const QModelIndex &index){
        if (index.column() == 1) {
            auto item = proxy_model.itemFromIndex(index);
            if (item->isMounted()) {
                item->unmount();
            }
        }
    });

    v.expandAll();

    w.setCentralWidget(&v);
    w.show();

    return a.exec();
}
