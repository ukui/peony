#include "mainwindow.h"
#include "file-operation/file-move-operation.h"
#include "file-operation/file-node.h"
#include "file-operation/file-node-reporter.h"
#include "gerror-wrapper.h"

#include <QDebug>
#include <QMessageBox>

#include <QToolBar>
#include <QLabel>
#include <QThread>
#include <QThreadPool>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setFixedSize(600, 480);
    QToolBar *t = new QToolBar(this);
    addToolBar(t);
    //quint64 *offset_value = new quint64(0);
    QLabel *offset_label = new QLabel(t);
    QLabel *current_uri_label = new QLabel(t);
    t->setOrientation(Qt::Vertical);
    t->addWidget(current_uri_label);
    t->addWidget(offset_label);

    QStringList srcUris;
    srcUris<<"file:///home/lanyue/test";
    QString destUri = "file:///home/lanyue/test2";
    //Peony::FileNodeReporter *reporter = new Peony::FileNodeReporter;
    Peony::FileMoveOperation *moveOp = new Peony::FileMoveOperation(srcUris, destUri);
    //moveOp->setForceUseFallback();
    connect(moveOp, &Peony::FileMoveOperation::errored, [](const Peony::GErrorWrapperPtr &err){
        qDebug()<<"err"<<err.get()->message();
        return QVariant(Peony::FileOperation::IgnoreAll);
    });
    QThreadPool pool;
    pool.start(moveOp);
}

MainWindow::~MainWindow()
{

}
