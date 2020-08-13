/*
 * Peony-Qt's Library
 *
 * Copyright (C) 2019, Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Authors: Jing Ding <dingjing@kylinos.cn>
 *
 */
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
