#include "file-operation-error-dialog-regular.h"

Peony::FileOperationErrorDialogRegular::FileOperationErrorDialogRegular(QString &title, QString &srcUri, QString &destUri,
                                                                        QString &content, QWidget *parent)
    : QDialog(parent), m_title(title),m_src_uri(srcUri), m_dest_uri(destUri), m_content(content)
{

}

Peony::FileOperationErrorDialogRegular::~FileOperationErrorDialogRegular()
{

}

bool Peony::FileOperationErrorDialogRegular::handle(Peony::FileOperationError &)
{

}

int Peony::FileOperationErrorDialogRegular::handleError(const QString &srcUri, const QString &destDirUri, const Peony::GErrorWrapperPtr &err, bool isCritical)
{

}
