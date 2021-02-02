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
#ifndef FILEOPERATIONERRORDIALOGREGULAR_H
#define FILEOPERATIONERRORDIALOGREGULAR_H

#include <QLabel>
#include <QDialog>
#include <QCheckBox>
#include <QLineEdit>
#include <QScrollArea>
#include "file-operation-error-dialog-base.h"

namespace Peony {

class FileRenameDialog;
class FileInformationLabel;
class FileOperationErrorDialogConflict;


/*!
 * \brief Factory class for error handling pop-up boxes
 *
 */
class PEONYCORESHARED_EXPORT FileOperationErrorDialogFactory
{
public:
    static FileOperationErrorHandler* getDialog (FileOperationError& errInfo);
};

/*!
 * \brief Error warning pop-up box
 * ED_WARNING
 */
class PEONYCORESHARED_EXPORT FileOperationErrorDialogWarning : public FileOperationErrorDialogBase
{
    Q_OBJECT
    Q_INTERFACES(Peony::FileOperationErrorHandler)
public:
    explicit FileOperationErrorDialogWarning(FileOperationErrorDialogBase *parent = nullptr);
    ~FileOperationErrorDialogWarning()override;

    virtual void handle (FileOperationError& error) override;

private:
    float m_margin = 9;
    float m_pic_top = 63;
    float m_margin_lr = 26;
    float m_pic_size = 48;
    float m_fix_width = 550;
    float m_fix_height = 188;

    float m_text_y = 65;
    float m_text_heigth = 60;

    float m_ok_x = 410;
    float m_ok_y = 132;
    float m_ok_w = 120;
    float m_ok_h = 36;

    float m_cancel_x = 280;
    float m_cancel_y = 132;
    float m_cancel_w = 120;
    float m_cancel_h = 36;

    QLabel* m_icon = nullptr;
    QLabel* m_text = nullptr;
    QScrollArea* m_text_scroll = nullptr;
    QPushButton* m_ok = nullptr;
    QPushButton* m_cancel = nullptr;
};

/*!
 * \brief Dialog box for handling file conflicts
 *
 */
class PEONYCORESHARED_EXPORT FileOperationErrorDialogConflict : public FileOperationErrorDialogBase
{
    Q_OBJECT
    Q_INTERFACES(Peony::FileOperationErrorHandler)
public:
    explicit FileOperationErrorDialogConflict(FileOperationErrorDialogBase *parent = nullptr);
    ~FileOperationErrorDialogConflict() override;

    void setTipFilename (QString name);
    void setTipFileicon (QString icon);

    virtual void handle (FileOperationError& error) override;

private:
    float           m_margin = 9;
    float           m_margin_lr = 26;
    float           m_fix_width = 550;
    float           m_fix_height = 192;

    // file icon
    float           m_file_x = 26;
    float           m_file_y = 60;
    float           m_file_size = 48;

    float           m_tip_x = 85;
    float           m_tip_y = 62;
    float           m_tip_width = 430;
    float           m_tip_height = 50;

    // replace
    float           m_rp_btn_x = 414;
    float           m_rp_btn_y = 140;
    float           m_rp_btn_width = 120;
    float           m_rp_btn_height = 36;

    // ignore
    float           m_ig_btn_x = 278;
    float           m_ig_btn_y = 140;
    float           m_ig_btn_width = 120;
    float           m_ig_btn_height = 36;

    // backup
    float           m_bk_btn_x = 142;
    float           m_bk_btn_y = 140;
    float           m_bk_btn_width = 120;
    float           m_bk_btn_height = 36;

    // Then do the same thing
    float           m_sm_btn_x = 16;
    float           m_sm_btn_y = 150;
    float           m_sm_btn_width = 200;
    float           m_sm_btn_height = 20;

    float           m_ck_btn_top = 385;
    float           m_ck_btn_heigth = 18;

    float           m_btn_top = 80;
    float           m_btn_width = 120;
    float           m_btn_heigth = 36;
    float           m_btn_ok_margin_left = 434;
    float           m_btn_cancel_margin_left = 298;

    QLabel*         m_tip = nullptr;
    QLabel*         m_title = nullptr;
    QLabel*         m_file_icon = nullptr;

    QString         m_file_name;
    QString         m_file_icon_name;

    QPushButton*    m_rp_btn = nullptr;
    QPushButton*    m_ig_btn = nullptr;
    QPushButton*    m_bk_btn = nullptr;
    QCheckBox*      m_sm_ck = nullptr;

    bool            m_ignore = false;
    bool            m_backup = false;
    bool            m_replace = false;
    bool            m_do_same = false;
};

/*!
 * \brief Error warning pop-up box
 * ED_NOT_SUPPORTED
 */
class PEONYCORESHARED_EXPORT FileOperationErrorDialogNotSupported : public FileOperationErrorDialogBase
{
    Q_OBJECT
    Q_INTERFACES(Peony::FileOperationErrorHandler)
public:
    explicit FileOperationErrorDialogNotSupported(FileOperationErrorDialogBase *parent = nullptr);
    ~FileOperationErrorDialogNotSupported()override;

    virtual void handle (FileOperationError& error) override;

private:
    float m_margin = 9;
    float m_pic_top = 63;
    float m_margin_lr = 26;
    float m_pic_size = 48;
    float m_fix_width = 550;
    float m_fix_height = 188;

    float m_text_y = 65;
    float m_text_heigth = 60;

    float m_ok_x = 410;
    float m_ok_y = 132;
    float m_ok_w = 120;
    float m_ok_h = 36;

    float m_cancel_x = 280;
    float m_cancel_y = 132;
    float m_cancel_w = 120;
    float m_cancel_h = 36;

    float           m_sm_btn_x = 16;
    float           m_sm_btn_y = 150;
    float           m_sm_btn_width = 200;
    float           m_sm_btn_height = 20;

    QLabel* m_icon = nullptr;
    QLabel* m_text = nullptr;
    QScrollArea* m_text_scroll = nullptr;
    QPushButton* m_ok = nullptr;
    QPushButton* m_cancel = nullptr;

    QCheckBox*      m_sm_ck = nullptr;
};

};

#endif // FILEOPERATIONERRORDIALOGREGULAR_H
