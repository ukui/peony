#include "file-operation-error-dialog.h"

#include <QFormLayout>
#include <QLabel>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QButtonGroup>

#include <QDesktopWidget>
#include <QApplication>

using namespace Peony;

FileOperationErrorDialog::FileOperationErrorDialog(QWidget *parent) : QDialog(parent)
{
    //center to desktop.
    setParent(QApplication::desktop());
    setWindowFlag(Qt::Dialog);
    setWindowTitle(tr("File Operation Error"));
    setWindowIcon(QIcon::fromTheme("system-error"));
    m_layout = new QFormLayout(this);
    m_layout->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);
    m_layout->setRowWrapPolicy(QFormLayout::WrapAllRows);
    m_layout->setLabelAlignment(Qt::AlignRight);
    m_layout->setFormAlignment(Qt::AlignLeft);

    m_err_line = new QLabel("unkwon", this);
    m_src_line = new QLabel("null", this);
    m_dest_line = new QLabel("null", this);

    m_layout->addRow(tr("Error message:"), m_err_line);
    m_layout->addRow(tr("Source File:"), m_src_line);
    m_layout->addRow(tr("Dest File:"), m_dest_line);

    m_button_box = new QDialogButtonBox(this);
    m_button_box2 = new QDialogButtonBox(this);
    QPushButton *ignoreOneBt = new QPushButton(tr("Ignore"), m_button_box);
    QPushButton *ignoreAllBt = new QPushButton(tr("Ignore All"), m_button_box);
    QPushButton *overwriteOneBt = new QPushButton(tr("Overwrite"), m_button_box);
    QPushButton *overwriteAllBt = new QPushButton(tr("Overwrite All"), m_button_box);
    QPushButton *backupOneBt = new QPushButton(tr("Backup"), m_button_box);
    QPushButton *backupAllBt = new QPushButton(tr("Backup All"), m_button_box);
    QPushButton *retryBt = new QPushButton(tr("&Retry"), m_button_box);
    QPushButton *cancelBt = new QPushButton(tr("&Cancel"), m_button_box);

    btGroup = new QButtonGroup(this);
    btGroup->addButton(ignoreOneBt, 1);
    btGroup->addButton(ignoreAllBt, 2);
    btGroup->addButton(overwriteOneBt, 3);
    btGroup->addButton(overwriteAllBt, 4);
    btGroup->addButton(backupOneBt, 5);
    btGroup->addButton(backupAllBt, 6);
    btGroup->addButton(retryBt, 7);
    btGroup->addButton(cancelBt, 8);

    connect(btGroup, SIGNAL(buttonClicked(int)), this, SLOT(done(int)));

    m_button_box->addButton(ignoreOneBt, QDialogButtonBox::ActionRole);
    m_button_box->addButton(ignoreAllBt, QDialogButtonBox::ActionRole);
    m_button_box->addButton(overwriteOneBt, QDialogButtonBox::ActionRole);
    m_button_box->addButton(overwriteAllBt, QDialogButtonBox::ActionRole);
    m_button_box2->addButton(backupOneBt, QDialogButtonBox::ActionRole);
    m_button_box2->addButton(backupAllBt, QDialogButtonBox::ActionRole);
    m_button_box2->addButton(retryBt, QDialogButtonBox::ActionRole);
    m_button_box2->addButton(cancelBt, QDialogButtonBox::ActionRole);

    m_layout->addWidget(m_button_box);
    m_layout->addWidget(m_button_box2);

    setLayout(m_layout);
}

FileOperationErrorDialog::~FileOperationErrorDialog()
{

}

FileOperationErrorHandler::~FileOperationErrorHandler()
{

}

QVariant FileOperationErrorDialog::handleError(const QString &srcUri,
                                               const QString &destDirUri,
                                               const GErrorWrapperPtr &err,
                                               bool isCritical)
{
    for (int i = 2; i < 8; i++) {
        btGroup->button(i)->setVisible(!isCritical);
    }

    m_src_line->setText(srcUri);
    m_dest_line->setText(destDirUri);
    m_err_line->setText(err.get()->message());

    //FIXME: how to uniform the button with dynamically?
    //the default size won't change before the dialog has shown.
    //do i need compute the longest text width? but the text is 18in...
    for (int i = 1; i < 9; i++) {
        btGroup->button(i)->setFixedWidth(100);
    }

    int val = exec();
    switch (val) {
    case 1: {
        return QVariant(FileOperation::IgnoreOne);
    }
    case 2: {
        return QVariant(FileOperation::IgnoreAll);
    }
    case 3: {
        return QVariant(FileOperation::OverWriteOne);
    }
    case 4: {
        return QVariant(FileOperation::OverWriteAll);
    }
    case 5: {
        return QVariant(FileOperation::BackupOne);
    }
    case 6: {
        return QVariant(FileOperation::BackupAll);
    }
    case 7: {
        return QVariant(FileOperation::Retry);
    }
    case 8: {
        return QVariant(FileOperation::Cancel);
    }
    default: {
        return QVariant(FileOperation::IgnoreAll);
    }
    }
}
