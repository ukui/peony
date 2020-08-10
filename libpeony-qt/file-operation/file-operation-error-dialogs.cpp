#include "file-operation-error-dialogs.h"

Peony::FileOperationErrorDialogConflict::FileOperationErrorDialogConflict(Peony::FileOperationError *error, FileOperationErrorDialogBase *parent)
    : FileOperationErrorDialogBase(error, parent)
{

}

Peony::FileOperationErrorDialogConflict::~FileOperationErrorDialogConflict()
{

}

//bool Peony::FileOperationErrorDialogConflict::handle()
//{

//}

// FIXME://DELETE
int Peony::FileOperationErrorDialogConflict::handleError(const QString &srcUri, const QString &destDirUri, const Peony::GErrorWrapperPtr &err, bool isCritical)
{

}

Peony::FileInformationLabel::FileInformationLabel(QWidget *parent) : QWidget(parent)
{
    setFixedSize(m_fix_width, m_fix_heigth);
    setContentsMargins(0, 0, 0, 0);
}
