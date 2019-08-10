#ifndef FILEOPERATIONERRORDIALOG_H
#define FILEOPERATIONERRORDIALOG_H

#include <QDialog>
#include "file-operation-error-handler.h"

class QFormLayout;
class QLabel;
class QDialogButtonBox;
class QButtonGroup;

namespace Peony {

class FileOperationErrorDialog : public QDialog, public FileOperationErrorHandler
{
    Q_OBJECT
    Q_INTERFACES(Peony::FileOperationErrorHandler)
public:
    explicit FileOperationErrorDialog(QWidget *parent = nullptr);
    ~FileOperationErrorDialog() override;

public Q_SLOTS:
    QVariant handleError(const QString &srcUri,
                         const QString &destDirUri,
                         const GErrorWrapperPtr &err) override;

private:
    QFormLayout *m_layout = nullptr;
    QLabel *m_src_line = nullptr;
    QLabel *m_dest_line = nullptr;
    QLabel *m_err_line = nullptr;
    QDialogButtonBox *m_button_box = nullptr;
    QDialogButtonBox *m_button_box2 = nullptr;

    QButtonGroup *btGroup = nullptr;
};

}

#endif // FILEOPERATIONERRORDIALOG_H
