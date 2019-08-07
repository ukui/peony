#include "file-operation-progress-wizard.h"

using namespace Peony;

FileOperationProgressWizard::FileOperationProgressWizard(QWidget *parent) : QWizard(parent)
{

}

FileOperationProgressWizard::~FileOperationProgressWizard()
{

}

void FileOperationProgressWizard::onElementFound(const QString &uri, const qint64 &size)
{

}

void FileOperationProgressWizard::onElementOperationFinished(const QString &uri, const qint64 &size)
{

}

void FileOperationProgressWizard::switchToPreparedPage()
{

}

void FileOperationProgressWizard::switchToProgressPage()
{

}

//FileOperationPreparePage
FileOperationPreparePage::FileOperationPreparePage(QWidget *parent) : QWizardPage (parent)
{

}

FileOperationPreparePage::~FileOperationPreparePage()
{

}

//FileOperationProgressPage
FileOperationProgressPage::FileOperationProgressPage(QWidget *parent) : QWizardPage (parent)
{

}

FileOperationProgressPage::~FileOperationProgressPage()
{

}
