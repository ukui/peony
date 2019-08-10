#include "mainwindow.h"
#include "file-operation/file-move-operation.h"
#include "file-operation/file-node.h"
#include "file-operation/file-node-reporter.h"
#include "connect-server-dialog.h"
#include "gerror-wrapper.h"

#include "file-operation/file-operation-progress-wizard.h"
#include "file-operation-error-dialog.h"

#include <QDebug>
#include <QMessageBox>

#include <QToolBar>
#include <QLabel>
#include <QThread>
#include <QThreadPool>

#include <QProgressBar>
#include <QProgressDialog>

#include <QDialog>
#include <QFileDialog>
#include <QUrl>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
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

    connect(startAction, &QAction::triggered, [=]{
        QMessageBox::question(nullptr, tr("source files"), tr("choose source files，"
                                                              "use cancel to finish the choices."));
        QFileDialog srcdlg;
        srcdlg.setFileMode(QFileDialog::ExistingFiles);
        srcdlg.setAcceptMode(QFileDialog::AcceptSave);
        srcdlg.setViewMode(QFileDialog::List);

        QStringList srcUris;
        srcdlg.exec();
        for (auto uri : srcdlg.selectedUrls()) {
            srcUris<<uri.toString();
        }

        QMessageBox::question(nullptr, tr("dest dir"), tr("choose dest dir,"
                                                          "use cancel to finish the choices."));
        QFileDialog destdlg;
        destdlg.setAcceptMode(QFileDialog::AcceptSave);
        destdlg.setFileMode(QFileDialog::Directory);

        QString destUri;
        destdlg.exec();
        destUri = destdlg.selectedUrls().at(0).url();

        Peony::FileMoveOperation *moveOp = new Peony::FileMoveOperation(srcUris, destUri);
        moveOp->setForceUseFallback();

        Peony::FileOperationErrorDialog *dlg = new Peony::FileOperationErrorDialog;
        moveOp->connect(moveOp, &Peony::FileMoveOperation::errored,
                        dlg, &Peony::FileOperationErrorDialog::handleError,
                        Qt::BlockingQueuedConnection);

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
