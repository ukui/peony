/********************************************************************************
** Form generated from reading UI file 'connect-server-dialog.ui'
**
** Created by: Qt User Interface Compiler version 5.15.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CONNECT_2D_SERVER_2D_DIALOG_H
#define UI_CONNECT_2D_SERVER_2D_DIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_ConnectServerDialog
{
public:
    QLabel *label1;
    QLabel *label2;
    QLabel *add_label;
    QLineEdit *user_lineEdit;
    QLabel *user_label;
    QLineEdit *pwd_lineEdit;
    QLabel *pwd_label;
    QLineEdit *add_lineEdit;
    QLabel *type_label;
    QLabel *ip_label;
    QLabel *file_label;
    QComboBox *type_comboBox;
    QLineEdit *file_lineEdit;
    QLabel *port_label;
    QComboBox *port_comboBox;
    QLineEdit *ip_edit;
    QPushButton *ok_pb;
    QPushButton *cancel_pb;
    QPushButton *help_pb;

    void setupUi(QDialog *ConnectServerDialog)
    {
        if (ConnectServerDialog->objectName().isEmpty())
            ConnectServerDialog->setObjectName(QString::fromUtf8("ConnectServerDialog"));
        ConnectServerDialog->setEnabled(true);
        ConnectServerDialog->resize(430, 532);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(ConnectServerDialog->sizePolicy().hasHeightForWidth());
        ConnectServerDialog->setSizePolicy(sizePolicy);
        ConnectServerDialog->setMinimumSize(QSize(430, 532));
        ConnectServerDialog->setMaximumSize(QSize(430, 532));
        QFont font;
        font.setFamily(QString::fromUtf8("Noto Sans CJK SC"));
        font.setPointSize(14);
        ConnectServerDialog->setFont(font);
        QIcon icon;
        QString iconThemeName = QString::fromUtf8("server");
        if (QIcon::hasThemeIcon(iconThemeName)) {
            icon = QIcon::fromTheme(iconThemeName);
        } else {
            icon.addFile(QString::fromUtf8("."), QSize(), QIcon::Normal, QIcon::Off);
        }
        ConnectServerDialog->setWindowIcon(icon);
        ConnectServerDialog->setLayoutDirection(Qt::LeftToRight);
        ConnectServerDialog->setStyleSheet(QString::fromUtf8("width:430px;\n"
"height:532px;\n"
"background:rgba(255,255,255,1);\n"
"border:0;\n"
"border-radius:6px;\n"
""));
        ConnectServerDialog->setSizeGripEnabled(false);
        label1 = new QLabel(ConnectServerDialog);
        label1->setObjectName(QString::fromUtf8("label1"));
        label1->setEnabled(true);
        label1->setGeometry(QRect(28, 48, 118, 23));
        sizePolicy.setHeightForWidth(label1->sizePolicy().hasHeightForWidth());
        label1->setSizePolicy(sizePolicy);
        label1->setStyleSheet(QString::fromUtf8("width:118px;\n"
"height:23px;\n"
"font-size:20px;\n"
"font-family:Noto Sans CJK SC;\n"
"font-weight:400;\n"
"color:rgba(0,0,0,1);\n"
"line-height:30px;\n"
"border:0;\n"
""));
        label2 = new QLabel(ConnectServerDialog);
        label2->setObjectName(QString::fromUtf8("label2"));
        label2->setGeometry(QRect(28, 260, 143, 23));
        label2->setStyleSheet(QString::fromUtf8("width:143px;\n"
"height:23px;\n"
"font-size:20px;\n"
"font-family:Noto Sans CJK SC;\n"
"font-weight:400;\n"
"color:rgba(0,0,0,1);\n"
"line-height:30px;\n"
"border:0;"));
        add_label = new QLabel(ConnectServerDialog);
        add_label->setObjectName(QString::fromUtf8("add_label"));
        add_label->setGeometry(QRect(28, 400, 56, 16));
        QFont font1;
        font1.setFamily(QString::fromUtf8("Noto Sans CJK SC"));
        font1.setBold(false);
        font1.setWeight(50);
        add_label->setFont(font1);
        add_label->setStyleSheet(QString::fromUtf8("width:56px;\n"
"height:13px;\n"
"font-size:14px;\n"
"font-family:Noto Sans CJK SC;\n"
"font-weight:400;\n"
"color:rgba(0,0,0,1);\n"
"line-height:30px;\n"
"border:0;"));
        user_lineEdit = new QLineEdit(ConnectServerDialog);
        user_lineEdit->setObjectName(QString::fromUtf8("user_lineEdit"));
        user_lineEdit->setGeometry(QRect(92, 312, 300, 30));
        QFont font2;
        font2.setFamily(QString::fromUtf8("Noto Sans CJK SC"));
        font2.setPointSize(11);
        user_lineEdit->setFont(font2);
        user_lineEdit->setStyleSheet(QString::fromUtf8("width:300px;\n"
"height:30px;\n"
"padding:3px 5px;\n"
"background:rgba(255,255,255,1);\n"
"opacity:0.04;\n"
"border:1px solid rgba(62, 108, 229, 0.3);\n"
"border-radius:4px;\n"
""));
        user_label = new QLabel(ConnectServerDialog);
        user_label->setObjectName(QString::fromUtf8("user_label"));
        user_label->setGeometry(QRect(28, 318, 40, 14));
        user_label->setFont(font1);
        user_label->setStyleSheet(QString::fromUtf8("width:40px;\n"
"height:14px;\n"
"font-size:14px;\n"
"font-family:Noto Sans CJK SC;\n"
"font-weight:400;\n"
"color:rgba(0,0,0,1);\n"
"line-height:30px;\n"
"border:0;"));
        pwd_lineEdit = new QLineEdit(ConnectServerDialog);
        pwd_lineEdit->setObjectName(QString::fromUtf8("pwd_lineEdit"));
        pwd_lineEdit->setGeometry(QRect(92, 351, 300, 30));
        QFont font3;
        font3.setFamily(QString::fromUtf8("Noto Sans Mono CJK SC"));
        font3.setPointSize(11);
        pwd_lineEdit->setFont(font3);
        pwd_lineEdit->setStyleSheet(QString::fromUtf8("width:300px;\n"
"height:30px;\n"
"padding:3px 5px;\n"
"background:rgba(255,255,255,1);\n"
"opacity:0.04;\n"
"border:1px solid rgba(62, 108, 229, 0.3);\n"
"border-radius:4px;\n"
""));
        pwd_label = new QLabel(ConnectServerDialog);
        pwd_label->setObjectName(QString::fromUtf8("pwd_label"));
        pwd_label->setGeometry(QRect(28, 358, 28, 13));
        pwd_label->setFont(font1);
        pwd_label->setStyleSheet(QString::fromUtf8("width:28px;\n"
"height:13px;\n"
"font-size:14px;\n"
"font-family:Noto Sans CJK SC;\n"
"font-weight:400;\n"
"color:rgba(0,0,0,1);\n"
"line-height:30px;\n"
"border:0;\n"
""));
        add_lineEdit = new QLineEdit(ConnectServerDialog);
        add_lineEdit->setObjectName(QString::fromUtf8("add_lineEdit"));
        add_lineEdit->setGeometry(QRect(92, 390, 300, 30));
        add_lineEdit->setFont(font3);
        add_lineEdit->setStyleSheet(QString::fromUtf8("width:300px;\n"
"height:30px;\n"
"padding:3px 5px;\n"
"background:rgba(255,255,255,1);\n"
"opacity:0.04;\n"
"border:1px solid rgba(62, 108, 229, 0.3);\n"
"border-radius:4px;\n"
""));
        type_label = new QLabel(ConnectServerDialog);
        type_label->setObjectName(QString::fromUtf8("type_label"));
        type_label->setGeometry(QRect(28, 145, 30, 14));
        type_label->setFont(font1);
        type_label->setStyleSheet(QString::fromUtf8("width:28px;\n"
"height:13px;\n"
"font-size:14px;\n"
"font-family:Noto Sans CJK SC;\n"
"font-weight:400;\n"
"color:rgba(0,0,0,1);\n"
"line-height:30px;\n"
"border:0;\n"
""));
        ip_label = new QLabel(ConnectServerDialog);
        ip_label->setObjectName(QString::fromUtf8("ip_label"));
        ip_label->setGeometry(QRect(28, 105, 42, 14));
        ip_label->setMinimumSize(QSize(42, 14));
        ip_label->setMaximumSize(QSize(999999, 999999));
        ip_label->setFont(font1);
        ip_label->setStyleSheet(QString::fromUtf8("width:42px;\n"
"height:14px;\n"
"font-size:14px;\n"
"font-family:Noto Sans CJK SC;\n"
"font-weight:400;\n"
"color:rgba(0,0,0,1);\n"
"line-height:30px;\n"
"border:0;\n"
""));
        file_label = new QLabel(ConnectServerDialog);
        file_label->setObjectName(QString::fromUtf8("file_label"));
        file_label->setGeometry(QRect(28, 185, 42, 14));
        file_label->setFont(font1);
        file_label->setStyleSheet(QString::fromUtf8("width:42px;\n"
"height:14px;\n"
"font-size:14px;\n"
"font-family:Noto Sans CJK SC;\n"
"font-weight:400;\n"
"color:rgba(0,0,0,1);\n"
"line-height:30px;\n"
"border:0;\n"
""));
        type_comboBox = new QComboBox(ConnectServerDialog);
        type_comboBox->addItem(QString());
        type_comboBox->addItem(QString());
        type_comboBox->setObjectName(QString::fromUtf8("type_comboBox"));
        type_comboBox->setGeometry(QRect(92, 138, 301, 32));
        type_comboBox->setFont(font3);
        type_comboBox->setStyleSheet(QString::fromUtf8("width:300px;\n"
"height:30px;\n"
"padding:3px 5px;\n"
"background:rgba(255,255,255,1);\n"
"opacity:0.04;\n"
"border:1px solid rgba(62, 108, 229, 0.3);\n"
"border-radius:4px;\n"
""));
        file_lineEdit = new QLineEdit(ConnectServerDialog);
        file_lineEdit->setObjectName(QString::fromUtf8("file_lineEdit"));
        file_lineEdit->setGeometry(QRect(92, 178, 300, 30));
        file_lineEdit->setFont(font3);
        file_lineEdit->setStyleSheet(QString::fromUtf8("width:300px;\n"
"height:30px;\n"
"padding:3px 5px;\n"
"background:rgba(255,255,255,1);\n"
"opacity:0.04;\n"
"border:1px solid rgba(62, 108, 229, 0.3);\n"
"border-radius:4px;\n"
""));
        port_label = new QLabel(ConnectServerDialog);
        port_label->setObjectName(QString::fromUtf8("port_label"));
        port_label->setGeometry(QRect(290, 105, 27, 14));
        port_label->setFont(font1);
        port_label->setStyleSheet(QString::fromUtf8("width:27px;\n"
"height:12px;\n"
"font-size:14px;\n"
"font-family:Noto Sans CJK SC;\n"
"font-weight:400;\n"
"color:rgba(0,0,0,1);\n"
"line-height:30px;\n"
"border:0;\n"
""));
        port_comboBox = new QComboBox(ConnectServerDialog);
        port_comboBox->addItem(QString());
        port_comboBox->addItem(QString());
        port_comboBox->addItem(QString());
        port_comboBox->addItem(QString());
        port_comboBox->addItem(QString());
        port_comboBox->addItem(QString());
        port_comboBox->setObjectName(QString::fromUtf8("port_comboBox"));
        port_comboBox->setGeometry(QRect(330, 98, 61, 30));
        port_comboBox->setFont(font3);
        port_comboBox->setStyleSheet(QString::fromUtf8("width:60px;\n"
"height:30px;\n"
"padding:3px;\n"
"background:rgba(255,255,255,1);\n"
"opacity:0.04;\n"
"border:1px solid rgba(62, 108, 229, 0.3);\n"
"border-radius:4px;\n"
""));
        port_comboBox->setEditable(true);
        ip_edit = new QLineEdit(ConnectServerDialog);
        ip_edit->setObjectName(QString::fromUtf8("ip_edit"));
        ip_edit->setGeometry(QRect(92, 98, 182, 30));
        ip_edit->setStyleSheet(QString::fromUtf8("width:182px;\n"
"height:30px;\n"
"padding:3px 5px;\n"
"background:rgba(255,255,255,1);\n"
"opacity:0.04;\n"
"border:1px solid rgba(62, 108, 229, 0.3);\n"
"border-radius:4px;\n"
"font-size:14px;\n"
"font-family:Noto Sans CJK SC;\n"
"font-weight:400;\n"
"color:rgba(0,0,0,1);\n"
"line-height:30px;"));
        ok_pb = new QPushButton(ConnectServerDialog);
        ok_pb->setObjectName(QString::fromUtf8("ok_pb"));
        ok_pb->setGeometry(QRect(316, 471, 80, 30));
        ok_pb->setFont(font1);
        ok_pb->setStyleSheet(QString::fromUtf8("width:80px;\n"
"height:30px;\n"
"background:rgba(62,108,229,1);\n"
"border:1px solid rgba(62, 108, 229, 0.3);\n"
"border-radius:4px;\n"
"font-size:14px;\n"
"font-family:Noto Sans CJK SC;\n"
"font-weight:400;\n"
"color:rgba(255,255,255,1);\n"
"line-height:30px;"));
        cancel_pb = new QPushButton(ConnectServerDialog);
        cancel_pb->setObjectName(QString::fromUtf8("cancel_pb"));
        cancel_pb->setGeometry(QRect(228, 471, 80, 30));
        cancel_pb->setFont(font1);
        cancel_pb->setStyleSheet(QString::fromUtf8("width:80px;\n"
"height:30px;\n"
"background:rgba(233,233,233,1);\n"
"border:1px solid rgba(62, 108, 229, 0.3);\n"
"border-radius:4px;\n"
"font-size:14px;\n"
"font-family:Noto Sans CJK SC;\n"
"font-weight:400;\n"
"color:rgba(0,0,0,1);\n"
"line-height:30px;"));
        help_pb = new QPushButton(ConnectServerDialog);
        help_pb->setObjectName(QString::fromUtf8("help_pb"));
        help_pb->setGeometry(QRect(32, 471, 100, 30));
        help_pb->setFont(font1);
        help_pb->setStyleSheet(QString::fromUtf8("width:26px;\n"
"height:13px;\n"
"font-size:14px;\n"
"font-family:Noto Sans CJK SC;\n"
"font-weight:400;\n"
"color:rgba(0,0,0,1);\n"
"line-height:30px;\n"
"border:1px solid rgba(62, 108, 229, 0.3);\n"
""));
        QWidget::setTabOrder(port_comboBox, user_lineEdit);
        QWidget::setTabOrder(user_lineEdit, pwd_lineEdit);
        QWidget::setTabOrder(pwd_lineEdit, help_pb);
        QWidget::setTabOrder(help_pb, cancel_pb);
        QWidget::setTabOrder(cancel_pb, ok_pb);

        retranslateUi(ConnectServerDialog);
        QObject::connect(cancel_pb, SIGNAL(clicked()), ConnectServerDialog, SLOT(close()));
        QObject::connect(ok_pb, SIGNAL(clicked()), ConnectServerDialog, SLOT(accept()));
        QObject::connect(help_pb, SIGNAL(clicked()), ConnectServerDialog, SLOT(help()));

        QMetaObject::connectSlotsByName(ConnectServerDialog);
    } // setupUi

    void retranslateUi(QDialog *ConnectServerDialog)
    {
        ConnectServerDialog->setWindowTitle(QCoreApplication::translate("ConnectServerDialog", "Connect to Sever", nullptr));
        label1->setText(QCoreApplication::translate("ConnectServerDialog", "\346\234\215\345\212\241\345\231\250\350\257\246\346\203\205", nullptr));
        label2->setText(QCoreApplication::translate("ConnectServerDialog", "\347\224\250\346\210\267\350\257\246\347\273\206\344\277\241\346\201\257", nullptr));
        add_label->setText(QCoreApplication::translate("ConnectServerDialog", "\346\267\273\345\212\240\346\240\207\347\255\276", nullptr));
        user_label->setText(QCoreApplication::translate("ConnectServerDialog", "\347\224\250\346\210\267\345\220\215", nullptr));
        pwd_label->setText(QCoreApplication::translate("ConnectServerDialog", "\345\257\206\347\240\201", nullptr));
        type_label->setText(QCoreApplication::translate("ConnectServerDialog", "\347\261\273\345\236\213", nullptr));
        ip_label->setText(QCoreApplication::translate("ConnectServerDialog", "\346\234\215\345\212\241\345\231\250", nullptr));
        file_label->setText(QCoreApplication::translate("ConnectServerDialog", "\346\226\207\344\273\266\345\244\271", nullptr));
        type_comboBox->setItemText(0, QCoreApplication::translate("ConnectServerDialog", "SAMBA", nullptr));
        type_comboBox->setItemText(1, QCoreApplication::translate("ConnectServerDialog", "FTP", nullptr));

        file_lineEdit->setText(QCoreApplication::translate("ConnectServerDialog", "/", nullptr));
        port_label->setText(QCoreApplication::translate("ConnectServerDialog", "\347\253\257\345\217\243", nullptr));
        port_comboBox->setItemText(0, QCoreApplication::translate("ConnectServerDialog", "20", nullptr));
        port_comboBox->setItemText(1, QCoreApplication::translate("ConnectServerDialog", "21", nullptr));
        port_comboBox->setItemText(2, QCoreApplication::translate("ConnectServerDialog", "137", nullptr));
        port_comboBox->setItemText(3, QCoreApplication::translate("ConnectServerDialog", "138", nullptr));
        port_comboBox->setItemText(4, QCoreApplication::translate("ConnectServerDialog", "139", nullptr));
        port_comboBox->setItemText(5, QCoreApplication::translate("ConnectServerDialog", "445", nullptr));

        ip_edit->setText(QString());
        ok_pb->setText(QCoreApplication::translate("ConnectServerDialog", "\347\241\256\345\256\232", nullptr));
        cancel_pb->setText(QCoreApplication::translate("ConnectServerDialog", "\345\217\226\346\266\210", nullptr));
        help_pb->setText(QCoreApplication::translate("ConnectServerDialog", "\345\270\256\345\212\251", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ConnectServerDialog: public Ui_ConnectServerDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONNECT_2D_SERVER_2D_DIALOG_H
