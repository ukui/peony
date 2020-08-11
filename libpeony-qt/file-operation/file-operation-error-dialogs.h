#ifndef FILEOPERATIONERRORDIALOGREGULAR_H
#define FILEOPERATIONERRORDIALOGREGULAR_H

#include <QLabel>
#include <QDialog>
#include <QCheckBox>
#include "file-operation-error-dialog-base.h"

namespace Peony {

class FileInformationLabel;

class PEONYCORESHARED_EXPORT FileOperationErrorDialogConflict : public FileOperationErrorDialogBase
{
    Q_OBJECT
    Q_INTERFACES(Peony::FileOperationErrorHandler)
public:
    FileOperationErrorDialogConflict(FileOperationError* error, FileOperationErrorDialogBase *parent = nullptr);
    ~FileOperationErrorDialogConflict() override;

//    virtual bool handle () override;

    // FIXME://DELETE
    virtual int handleError(const QString &srcUri,
                                 const QString &destDirUri,
                                 const GErrorWrapperPtr &err,
                                 bool isCritical = false) override;
private:
    float m_fix_width = 580;
    float m_fix_height = 498;
    float m_margin = 9;
    float m_margin_lr = 26;

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
    QPushButton* m_rename = nullptr;
    QPushButton* m_cancel = nullptr;
};

class FileInformationLabel : public QWidget
{
    Q_OBJECT

public:
    explicit FileInformationLabel(QWidget* parent = nullptr);
    ~FileInformationLabel();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    float m_fix_width = 528;
    float m_fix_heigth = 116;

    //
    float m_pic_x = 16;
    float m_pic_y = 21;
    float m_pic_size = 56;

    // pic name
    float m_pic_name_x = 26;
    float m_pic_name_y = 79;
    float m_pic_name_w = 38;
    float m_pic_name_h = 24;

    // file information
    QLabel* m_file_information = nullptr;
    float m_file_name_x = 92;
    float m_file_name_y = 12;
    float m_file_name_w = 420;
    float m_file_name_h = 92;

    QIcon m_icon;
    QString m_op_name = "replace";
    QString m_file_name = "wen jian ming";
    QString m_file_size = "asdasd zi jie";
    QString m_modify_time = "2020-08-19";
    QString m_file_location = "file:///home/xxx/xxx/xxx/xxx/xxx/xx/ss/ss/ss/ss/ss/ss/ss/ss/ss/ss/ss/ss/ss/ss/ss/ss/ss";
};
};

#endif // FILEOPERATIONERRORDIALOGREGULAR_H
