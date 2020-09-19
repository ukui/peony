/********************************************************************************
** Form generated from reading UI file 'connect-server-dialog.ui'
**
** Created by: Qt User Interface Compiler version 5.12.8
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CONNECT_2D_SERVER_2D_DIALOG_H
#define UI_CONNECT_2D_SERVER_2D_DIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>

QT_BEGIN_NAMESPACE

class Ui_ConnectServerDialog
{
public:
    QGridLayout *gridLayout;
    QLineEdit *usr_edit;
    QLabel *label_3;
    QLabel *label_2;
    QLineEdit *passwd_edit;
    QLineEdit *domain_edit;
    QCheckBox *save_passwd_checkbox;
    QLabel *label;
    QDialogButtonBox *buttonBox;
    QCheckBox *anonymous_checkbox;

    void setupUi(QDialog *ConnectServerDialog)
    {
        if (ConnectServerDialog->objectName().isEmpty())
            ConnectServerDialog->setObjectName(QString::fromUtf8("ConnectServerDialog"));
        ConnectServerDialog->resize(348, 318);
        QIcon icon(QIcon::fromTheme(QString::fromUtf8("server")));
        ConnectServerDialog->setWindowIcon(icon);
        ConnectServerDialog->setLayoutDirection(Qt::LeftToRight);
        ConnectServerDialog->setSizeGripEnabled(false);
        gridLayout = new QGridLayout(ConnectServerDialog);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        usr_edit = new QLineEdit(ConnectServerDialog);
        usr_edit->setObjectName(QString::fromUtf8("usr_edit"));

        gridLayout->addWidget(usr_edit, 0, 1, 1, 1);

        label_3 = new QLabel(ConnectServerDialog);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        gridLayout->addWidget(label_3, 2, 0, 1, 1);

        label_2 = new QLabel(ConnectServerDialog);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout->addWidget(label_2, 1, 0, 1, 1);

        passwd_edit = new QLineEdit(ConnectServerDialog);
        passwd_edit->setObjectName(QString::fromUtf8("passwd_edit"));

        gridLayout->addWidget(passwd_edit, 1, 1, 1, 1);

        domain_edit = new QLineEdit(ConnectServerDialog);
        domain_edit->setObjectName(QString::fromUtf8("domain_edit"));

        gridLayout->addWidget(domain_edit, 2, 1, 1, 1);

        save_passwd_checkbox = new QCheckBox(ConnectServerDialog);
        save_passwd_checkbox->setObjectName(QString::fromUtf8("save_passwd_checkbox"));
        save_passwd_checkbox->setLayoutDirection(Qt::RightToLeft);

        gridLayout->addWidget(save_passwd_checkbox, 6, 0, 1, 1);

        label = new QLabel(ConnectServerDialog);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout->addWidget(label, 0, 0, 1, 1);

        buttonBox = new QDialogButtonBox(ConnectServerDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        gridLayout->addWidget(buttonBox, 8, 0, 1, 2);

        anonymous_checkbox = new QCheckBox(ConnectServerDialog);
        anonymous_checkbox->setObjectName(QString::fromUtf8("anonymous_checkbox"));
        anonymous_checkbox->setLayoutDirection(Qt::RightToLeft);

        gridLayout->addWidget(anonymous_checkbox, 7, 0, 1, 1);


        retranslateUi(ConnectServerDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), ConnectServerDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), ConnectServerDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(ConnectServerDialog);
    } // setupUi

    void retranslateUi(QDialog *ConnectServerDialog)
    {
        ConnectServerDialog->setWindowTitle(QApplication::translate("ConnectServerDialog", "Connect to Sever", nullptr));
        label_3->setText(QApplication::translate("ConnectServerDialog", "Domain", nullptr));
        label_2->setText(QApplication::translate("ConnectServerDialog", "Password", nullptr));
        save_passwd_checkbox->setText(QApplication::translate("ConnectServerDialog", "Save Password", nullptr));
        label->setText(QApplication::translate("ConnectServerDialog", "User", nullptr));
        anonymous_checkbox->setText(QApplication::translate("ConnectServerDialog", "Anonymous", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ConnectServerDialog: public Ui_ConnectServerDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONNECT_2D_SERVER_2D_DIALOG_H
