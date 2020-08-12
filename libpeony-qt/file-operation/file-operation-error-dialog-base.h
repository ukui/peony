#ifndef FILEOPERATIONERRORDIALOGBASE_H
#define FILEOPERATIONERRORDIALOGBASE_H

#include <QDialog>
#include "file-operation-error-handler.h"

namespace Peony {
class PEONYCORESHARED_EXPORT FileOperationErrorDialogBase : public QDialog, public FileOperationErrorHandler
{
    Q_OBJECT
    Q_INTERFACES(Peony::FileOperationErrorHandler)
public:
    FileOperationErrorDialogBase(QDialog *parent);
    ~FileOperationErrorDialogBase() override;

Q_SIGNALS:
    void cancel();

protected:
    void paintEvent(QPaintEvent*) override;
    void mouseMoveEvent(QMouseEvent *event)override;
    void mousePressEvent(QMouseEvent *event)override;

protected:
    FileOperationError*         m_error = nullptr;

private:
    float                       m_btn_size = 20;
    float                       m_margin_tp = 9;
    float                       m_margin_lr = 16;
    float                       m_btn_margin = 5;
    float                       m_header_height = 30;
};
};


#endif // FILEOPERATIONERRORDIALOGBASE_H
