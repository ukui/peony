#include "mainwindow.h"
#include "file-operation/file-move-operation.h"
#include "file-operation/file-node.h"
#include "file-operation/file-node-reporter.h"
#include "connect-server-dialog.h"
#include "gerror-wrapper.h"

#include "file-operation/file-operation-progress-wizard.h"

#include <QDebug>
#include <QMessageBox>

#include <QToolBar>
#include <QLabel>
#include <QThread>
#include <QThreadPool>

#include <QProgressBar>
#include <QProgressDialog>

#include <QDialog>

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
    QAction *startAction = new QAction("start", t);
    t->addAction(startAction);

    QStringList srcUris;
    srcUris<<"file:///home/lanyue/test";
    QString destUri = "file:///home/lanyue/test2";
    //Peony::FileNodeReporter *reporter = new Peony::FileNodeReporter;

    connect(startAction, &QAction::triggered, [=]{
        Peony::FileMoveOperation *moveOp = new Peony::FileMoveOperation(srcUris, destUri);
        moveOp->setForceUseFallback();
        moveOp->connect(moveOp, &Peony::FileMoveOperation::errored, this,
                        &MainWindow::handleError, Qt::BlockingQueuedConnection);

        Peony::FileOperationProgressWizard *wizard = new Peony::FileOperationProgressWizard;
        wizard->connect(moveOp, &Peony::FileMoveOperation::operationStarted,
                        wizard, &Peony::FileOperationProgressWizard::show, Qt::BlockingQueuedConnection);
        wizard->connect(moveOp, &Peony::FileMoveOperation::addOne,
                        wizard, &Peony::FileOperationProgressWizard::onElementFoundOne);
        wizard->connect(moveOp, &Peony::FileMoveOperation::operationPrepared,
                        wizard, &Peony::FileOperationProgressWizard::switchToProgressPage);
        wizard->connect(moveOp, &Peony::FileMoveOperation::fileMoved,
                        wizard, &Peony::FileOperationProgressWizard::onFileOperationProgressedOne);
        //operationFinished has a few time delay because there are many resources need be released and deconstructor.
        wizard->connect(moveOp, &Peony::FileMoveOperation::operationProgressed,
                        wizard, &Peony::FileOperationProgressWizard::onFileOperationProgressedAll);
        wizard->connect(moveOp, &Peony::FileMoveOperation::srcFileDeleted,
                        wizard, &Peony::FileOperationProgressWizard::onElementClearOne);

        connect(wizard, &Peony::FileOperationProgressWizard::cancelled,
                moveOp, &Peony::FileMoveOperation::cancel);
        connect(moveOp, &Peony::FileOperation::operationStartRollbacked,
                wizard, &Peony::FileOperationProgressWizard::switchToRollbackPage);
        connect(moveOp, &Peony::FileMoveOperation::rollbacked,
                wizard, &Peony::FileOperationProgressWizard::onFileRollbacked);

        wizard->connect(moveOp, &Peony::FileMoveOperation::operationFinished,
                        wizard, &QDialog::accepted);
        connect(wizard, &QDialog::accepted, wizard, &Peony::FileOperationProgressWizard::deleteLater);

        QThreadPool::globalInstance()->start(moveOp);
    });
}

MainWindow::~MainWindow()
{

}

QVariant MainWindow::handleError(const QString &srcUri,
                                 const QString &destDirUri,
                                 const Peony::GErrorWrapperPtr &err)
{
    qDebug()<<"mainwindow handle err"<<err.get()->message();
    QDialog dlg;

    QLabel l(srcUri + " to " + destDirUri + " " + err.get()->message());
    dlg.setExtension(&l);
    dlg.showExtension(true);
    dlg.exec();
    //blocked
    return QVariant(Peony::FileOperation::IgnoreOne);
}
