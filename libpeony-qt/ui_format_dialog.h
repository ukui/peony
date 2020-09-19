/********************************************************************************
** Form generated from reading UI file 'format_dialog.ui'
**
** Created by: Qt User Interface Compiler version 5.12.8
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FORMAT_DIALOG_H
#define UI_FORMAT_DIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_Format_Dialog
{
public:
    QLabel *label_rom_size;
    QLabel *label_system;
    QComboBox *comboBox_system;
    QLabel *label_device_name;
    QLineEdit *lineEdit_device_name;
    QCheckBox *checkBox_clean_or_not;
    QPushButton *pushButton_ok;
    QPushButton *pushButton_close;
    QComboBox *comboBox_rom_size;
    QProgressBar *progressBar_process;
    QLabel *label_process;

    void setupUi(QDialog *Format_Dialog)
    {
        if (Format_Dialog->objectName().isEmpty())
            Format_Dialog->setObjectName(QString::fromUtf8("Format_Dialog"));
        Format_Dialog->resize(487, 373);
        label_rom_size = new QLabel(Format_Dialog);
        label_rom_size->setObjectName(QString::fromUtf8("label_rom_size"));
        label_rom_size->setGeometry(QRect(30, 20, 151, 18));
        label_system = new QLabel(Format_Dialog);
        label_system->setObjectName(QString::fromUtf8("label_system"));
        label_system->setGeometry(QRect(30, 80, 151, 18));
        comboBox_system = new QComboBox(Format_Dialog);
        comboBox_system->addItem(QString());
        comboBox_system->addItem(QString());
        comboBox_system->addItem(QString());
        comboBox_system->setObjectName(QString::fromUtf8("comboBox_system"));
        comboBox_system->setGeometry(QRect(120, 80, 291, 26));
        label_device_name = new QLabel(Format_Dialog);
        label_device_name->setObjectName(QString::fromUtf8("label_device_name"));
        label_device_name->setGeometry(QRect(30, 140, 151, 18));
        lineEdit_device_name = new QLineEdit(Format_Dialog);
        lineEdit_device_name->setObjectName(QString::fromUtf8("lineEdit_device_name"));
        lineEdit_device_name->setGeometry(QRect(120, 140, 291, 26));
        checkBox_clean_or_not = new QCheckBox(Format_Dialog);
        checkBox_clean_or_not->setObjectName(QString::fromUtf8("checkBox_clean_or_not"));
        checkBox_clean_or_not->setGeometry(QRect(30, 200, 291, 24));
        pushButton_ok = new QPushButton(Format_Dialog);
        pushButton_ok->setObjectName(QString::fromUtf8("pushButton_ok"));
        pushButton_ok->setGeometry(QRect(230, 310, 80, 26));
        pushButton_close = new QPushButton(Format_Dialog);
        pushButton_close->setObjectName(QString::fromUtf8("pushButton_close"));
        pushButton_close->setGeometry(QRect(370, 310, 80, 26));
        comboBox_rom_size = new QComboBox(Format_Dialog);
        comboBox_rom_size->setObjectName(QString::fromUtf8("comboBox_rom_size"));
        comboBox_rom_size->setGeometry(QRect(120, 20, 291, 26));
        progressBar_process = new QProgressBar(Format_Dialog);
        progressBar_process->setObjectName(QString::fromUtf8("progressBar_process"));
        progressBar_process->setGeometry(QRect(30, 250, 381, 31));
        progressBar_process->setValue(24);
        label_process = new QLabel(Format_Dialog);
        label_process->setObjectName(QString::fromUtf8("label_process"));
        label_process->setGeometry(QRect(420, 250, 51, 31));

        retranslateUi(Format_Dialog);

        QMetaObject::connectSlotsByName(Format_Dialog);
    } // setupUi

    void retranslateUi(QDialog *Format_Dialog)
    {
        Format_Dialog->setWindowTitle(QApplication::translate("Format_Dialog", "Dialog", nullptr));
        label_rom_size->setText(QApplication::translate("Format_Dialog", "rom_size", nullptr));
        label_system->setText(QApplication::translate("Format_Dialog", "system", nullptr));
        comboBox_system->setItemText(0, QApplication::translate("Format_Dialog", "ntfs", nullptr));
        comboBox_system->setItemText(1, QApplication::translate("Format_Dialog", "vfat", nullptr));
        comboBox_system->setItemText(2, QApplication::translate("Format_Dialog", "ext4", nullptr));

        label_device_name->setText(QApplication::translate("Format_Dialog", "device_name", nullptr));
        checkBox_clean_or_not->setText(QApplication::translate("Format_Dialog", "clean it total", nullptr));
        pushButton_ok->setText(QApplication::translate("Format_Dialog", "ok", nullptr));
        pushButton_close->setText(QApplication::translate("Format_Dialog", "close", nullptr));
        label_process->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class Format_Dialog: public Ui_Format_Dialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FORMAT_DIALOG_H
