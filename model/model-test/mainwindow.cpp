#include "mainwindow.h"

#include <QLineEdit>
#include <QLayout>
#include <memory>

#include "file-info.h"
#include "file-item.h"
#include "file-item-model.h"
#include "file-item-proxy-filter-sort-model.h"
#include "file-enumerator.h"
#include "file-info-job.h"
#include "gerror-wrapper.h"
#include <QTreeView>
#include <QToolBar>

#include <QTimer>

#include <QDebug>

#include "file-info-manager.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QIcon::setThemeName("ukui-icon-theme");
    QLineEdit *line = new QLineEdit("computer:///", this);
    QToolBar *toolbar = new QToolBar(this);
    toolbar->addWidget(line);
    addToolBar(Qt::TopToolBarArea, toolbar);

    //job
    /*
    connect(line, &QLineEdit::returnPressed, [=](){
        auto info = Peony::FileInfo::fromUri(line->text());
        Peony::FileInfoJob *job = new Peony::FileInfoJob(info);
        job->setAutoDelete();
        job->connect(job, &Peony::FileInfoJob::queryAsyncFinished, [=](bool successed){
            if (successed) {
                qDebug()<<info->displayName();
            }
        });
        job->queryAsync();
    });
    */

    //enumerator
    /*
    connect(line, &QLineEdit::returnPressed, [=](){
        Peony::FileEnumerator *enumerator = new Peony::FileEnumerator;
        enumerator->setEnumerateDirectory(line->text());
        connect(enumerator, &Peony::FileEnumerator::prepared, [=](std::shared_ptr<Peony::GErrorWrapper> err){
            if (err) {
                qDebug()<<err->message();
                return ;
            }
            connect(enumerator, &Peony::FileEnumerator::enumerateFinished, [=](bool successed){
                if (successed) {
                    auto files = enumerator->getChildren();
                    for (auto file : files) {
                        qDebug()<<file->uri();
                    }
                    delete enumerator;
                }
            });
            enumerator->enumerateAsync();
        });
        enumerator->prepare();
    });
    */

    //model/view

    connect(line, &QLineEdit::returnPressed, [=](){
        Peony::FileItemModel *model = new Peony::FileItemModel(this);

        Peony::FileItem *item = new Peony::FileItem(Peony::FileInfo::fromUri(line->text().toUtf8().constData()), nullptr, model, this);
        model->setRootItem(item);

        QTreeView *v = new QTreeView();
        v->setAttribute(Qt::WA_DeleteOnClose);
        v->setModel(model);
        model->setParent(v);
        v->show();
        connect(v, &QTreeView::expanded, [=](const QModelIndex &index){
            auto item = model->itemFromIndex(index);
            item->findChildrenAsync();
        });
        connect(v, &QTreeView::collapsed, [=](const QModelIndex &index){
            auto item = model->itemFromIndex(index);
            item->clearChildren();
        });

        QTreeView *pv = new QTreeView;
        Peony::FileItemProxyFilterSortModel *proxy_model = new Peony::FileItemProxyFilterSortModel(pv);
        proxy_model->setSourceModel(model);
        pv->setAttribute(Qt::WA_DeleteOnClose);
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
