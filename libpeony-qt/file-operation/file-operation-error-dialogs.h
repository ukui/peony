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
    float m_text_heigth = 44;

    float m_ok_x = 410;
    float m_ok_y = 132;
    float m_ok_w = 120;
    float m_ok_h = 36;

    QLabel* m_icon = nullptr;
    QLabel* m_text = nullptr;
    QPushButton* m_ok = nullptr;
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

    virtual void handle (FileOperationError& error) override;

private:
    float m_margin = 9;
    float m_margin_lr = 26;
    float m_fix_width = 580;
    float m_fix_height = 498;

    float m_tip_y = 55;
    float m_tip_height = 50;

    float m_file_info1_top = 119;
    float m_file_info2_top = 247;
    float m_file_info_height = 116;

    float m_ck_btn_top = 385;
    float m_ck_btn_heigth = 18;

    float m_btn_top = 442;
    float m_btn_width = 120;
    float m_btn_heigth = 36;
    float m_btn_ok_margin_left = 434;
    float m_btn_cancel_margin_left = 298;

    QLabel* m_title = nullptr;
    QPushButton* m_mini = nullptr;
    QPushButton* m_close = nullptr;

    QLabel* m_tip = nullptr;
    FileInformationLabel* m_file_label1 = nullptr;
    FileInformationLabel* m_file_label2 = nullptr;

    QLabel* m_ck_label = nullptr;
    QCheckBox* m_ck_box = nullptr;

    QPushButton* m_ok = nullptr;
    QPushButton* m_rename = nullptr;    // The renaming feature also needs some grooming, such as how to rename after selecting "Do something similar later"
    QPushButton* m_cancel = nullptr;

    FileRenameDialog* m_rename_dialog = nullptr;

    bool m_is_backup = false;
    bool m_is_backup_all = false;

    bool m_is_replace = false;          // replace or ignore, true is replace
    bool m_do_same_operation = false;   // Then do the same thing with the same error
};

/**
 * Some of the widgets in the error pop-up box
 */

/*!
 * \brief file information label
 */
class FileInformationLabel : public QFrame
{
    Q_OBJECT
public:
    explicit FileInformationLabel(QWidget* parent = nullptr);
    ~FileInformationLabel();

Q_SIGNALS:
    void active ();

public:
    float getIconSize();
    void setActive (bool active);
    void setOpName (QString name);
    void setPixmap (QPixmap pixmap);
    void setFileSize (QString fileSize);
    void setFileName (QString fileName);
    void setFileLocation (QString path);
    void setFileModifyTime (QString modify);

protected:
    void paintEvent(QPaintEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event)override;

private:
    float m_fix_width = 528;
    float m_fix_heigth = 116;

    //
    float m_pic_x = 16;
    float m_pic_y = 21;
    float m_pic_size = 56;

    // pic name
    float m_pic_name_x = 24;
    float m_pic_name_y = 79;
    float m_pic_name_w = 40;
    float m_pic_name_h = 24;

    // file information
    QLabel* m_file_information = nullptr;
    float m_file_name_x = 92;
    float m_file_name_y = 12;
    float m_file_name_w = 420;
    float m_file_name_h = 92;

    QPixmap m_icon;
    bool m_active = false;
    QString m_op_name = nullptr;
    QString m_file_name = nullptr;
    QString m_file_size = nullptr;
    QString m_modify_time = nullptr;
    QString m_file_location = nullptr;
};

/*!
 * \brief rename dialog
 */
class FileRenameDialog : public QDialog
{
    Q_OBJECT
public:
    enum RenameType
    {
        USER_INPUT,
        AUTO_INSCREASE,
    };
    explicit FileRenameDialog(QWidget* parent = nullptr);
    ~FileRenameDialog();

Q_SIGNALS:
    void customRename (RenameType nameType, QString name);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event)override;
    void mousePressEvent(QMouseEvent *event)override;

private:
    float m_margin = 9;
    float m_fix_width = 550;
    float m_fix_heigth = 188;
    float m_fix_heigth_2 = 228;

    float m_header_btn_size = 26;

    QLabel* m_tip = nullptr;
    QLineEdit* m_name = nullptr;
    QPushButton* m_ok = nullptr;
    QLabel* m_name_label = nullptr;
    QPushButton* m_cancel = nullptr;
    QCheckBox* m_if_custom = nullptr;

};
};

#endif // FILEOPERATIONERRORDIALOGREGULAR_H
