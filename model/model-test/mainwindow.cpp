#include "mainwindow.h"

#include <QLineEdit>
#include <QLayout>
#include <memory>

#include "file-info.h"
#include "file-item.h"
#include "file-item-model.h"
#include "file-item-proxy-filter-sort-model.h"
#include <QTreeView>

#include <QTimer>

#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QIcon::setThemeName("ukui-icon-theme");
    QLineEdit *line = new QLineEdit("computer:///", this);
    layout()->addWidget(line);

    connect(line, &QLineEdit::returnPressed, [=](){
        Peony::FileItemModel *model = new Peony::FileItemModel(this);
        Peony::FileItem *item = new Peony::FileItem(Peony::FileInfo::fromUri(line->text()), nullptr, model, this);
        model->setRootItem(item);

        QTreeView *v = new QTreeView();
        v->setModel(model);
        model->setParent(v);
        v->show();
        connect(v, &QTreeView::expanded, [=](const QModelIndex &index){
            auto item = model->itemFromIndex(index);
            item->findChildrenAsync();
        });
        connect(v, &QTreeView::collapsed, [=](const QModelIndex &index){
            auto item = model->itemFromIndex(index);
            model->removeRows(0, model->rowCount(index) - 1, index);
            item->clearChildren();
        });


        Peony::FileItemProxyFilterSortModel *proxy_model = new Peony::FileItemProxyFilterSortModel;
        proxy_model->setSourceModel(model);
        QTreeView *pv = new QTreeView;
        pv->setSortingEnabled(true);
        proxy_model->sort(0);
        pv->setModel(proxy_model);
        connect(pv, &QTreeView::expanded, [=](const QModelIndex &proxyIndex){
            auto item = proxy_model->itemFromIndex(proxyIndex);
            item->findChildrenAsync();
        });
        connect(pv, &QTreeView::collapsed, [=](const QModelIndex &proxyIndex){
            auto index = proxy_model->getSourceIndex(proxyIndex);
            auto item = model->itemFromIndex(index);
            item->clearChildren();
        });
        pv->show();

    });
}

MainWindow::~MainWindow()
{

}
