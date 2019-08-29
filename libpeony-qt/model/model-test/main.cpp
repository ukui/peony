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

#include <search-vfs-register.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SearchVFSRegister::registSearchVFS();
    MainWindow w;
    QToolBar t;
    QLineEdit e;
    Peony::PathBarModel pm;
    Peony::PathCompleter c;
    c.setModel(&pm);
    e.setCompleter(&c);

    w.addToolBar(Qt::TopToolBarArea, &t);
    t.addWidget(&e);

    QLabel l;
    l.setWordWrap(true);
    l.setMaximumWidth(480);
    l.setText("This example is telling you how to use peony-qt's search vfs."
              "\n"
              "Search uri split to args by sperator '&', the arguments of search uri supported:"
              "\n"
              "search_uri={@uri1}(,{@uri2},...)"
              "\n"
              "recursive=<0>/<1>"
              "\n"
              "save=<0>/<1>"
              "\n"
              "case_sensitive=<0>/<1>"
              "\n"
              "name_regexp={string}"
              "\n"
              "content_regexp={string}");

    w.setCentralWidget(&l);

    e.setText("search:///search_uris=file://" + QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "&name_regexp=test&recursive=1");
    e.connect(&e, &QLineEdit::returnPressed, [&](){
        QListView *v = new QListView;

        /// set view attribute
        v->setAttribute(Qt::WA_DeleteOnClose);
        v->setWordWrap(true);
        v->setViewMode(QListView::IconMode);
        v->setGridSize(QSize(96, 96));
        v->setResizeMode(QListView::Adjust);

        /// set select action
        v->setSelectionMode(QAbstractItemView::ExtendedSelection);

        /// set dnd actions
        v->setDragDropMode(QAbstractItemView::DragDrop);
        v->setDefaultDropAction(Qt::MoveAction);

        Peony::FileItemModel *model = new Peony::FileItemModel(v);
        model->setPositiveResponse();
        model->setRootUri(e.text());
        Peony::FileItemProxyFilterSortModel *proxy_model = new Peony::FileItemProxyFilterSortModel(model);
        proxy_model->setSourceModel(model);

        /// resort when load directory finished
        model->connect(model, &Peony::FileItemModel::findChildrenFinished, [=](){
            proxy_model->sort(0);
        });

        /// re-sort when file(s) moved in/out.
        model->connect(model, &Peony::FileItemModel::updated, [=](){
            proxy_model->sort(0);
        });

        /// double clicked for location change.
        v->setEditTriggers(QListView::EditKeyPressed);
        v->connect(v, &QListView::doubleClicked, [=](const QModelIndex &index){
            auto item = proxy_model->itemFromIndex(index);
            if (item->hasChildren()) {
                model->setRootUri(item->uri());
            }
        });

        v->setModel(proxy_model);
        v->show();
    });

    w.show();

    return a.exec();
}
