/*
 * Peony-Qt
 *
 * Copyright (C) 2020, Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Authors: Yue Lan <lanyue@kylinos.cn>
 *
 */

#include "about-dialog.h"
#include "ui_about-dialog.h"
#include "xatom-helper.h"
#include <locale.h>
#include <libintl.h>
#include <QDebug>
#include <QPushButton>
#include <QPainter>
#include <QPainterPath>
#include <QtMath>

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);
    initUI();
}

AboutDialog::~AboutDialog()
{
    delete ui;
}

void AboutDialog::initUI()
{
	QPalette palette;
	QTextCursor textCursor;
	QTextBlockFormat blockFormat;

	palette.setColor(QPalette::Highlight, QColor("#E54A50"));
	blockFormat.setLineHeight(24, QTextBlockFormat::SingleHeight);//font-line-hight

    setAutoFillBackground(true);
    setBackgroundRole(QPalette::Base);

    //bug#101149 使用窗管
    MotifWmHints hints;
    hints.flags = MWM_HINTS_FUNCTIONS | MWM_HINTS_DECORATIONS;
    hints.functions = MWM_FUNC_ALL;
    hints.decorations = MWM_DECOR_BORDER;
    XAtomHelper::getInstance()->setWindowMotifHint(winId(), hints);

    ui->logoLabel->setPixmap(QIcon::fromTheme("system-file-manager").pixmap(24,24));

    ui->titleLabel->setText(tr("Peony"));

    ui->closeBtn->setFlat(true);
    ui->closeBtn->setProperty("isIcon", true);
	ui->closeBtn->setPalette(palette);
    ui->closeBtn->setFixedSize(QSize(30, 30));
    ui->closeBtn->setProperty("isWindowButton", 2);
    ui->closeBtn->setProperty("useIconHighlightEffect", 0x8);
    ui->closeBtn->setIcon(QIcon::fromTheme("window-close-symbolic"));

    ui->iconLabel->setPixmap(QIcon::fromTheme("system-file-manager").pixmap(96,96));
    auto font = qApp->font();
    QFont namefont = font;
    namefont.setBold(true);
    namefont.setPointSize(font.pointSize()*1.28);

    ui->nameLabel->setFont(namefont);
    ui->nameLabel->setText(tr("Peony"));

    ui->versionLabel->setFont(font);
//    ui->versionLabel->setStyleSheet("color:#595959;");
    ui->versionLabel->setText(QString(tr("Version number: %1")).arg(getCurrentVersion()));

//	ui->briefTextedit->setFont(font14);
	ui->briefTextedit->setReadOnly(true);
//    ui->briefTextedit->setStyleSheet("color:#595959;");

	textCursor = ui->briefTextedit->textCursor();	//before the setText().
    ui->briefTextedit->setText(tr("Peony is a graphical software to help users manage system files. "
                                "It provides common file operation functions for users, such as file viewing, "
                                "file copy, paste, cut, delete, rename, file selection, application opening, "
                                "file search, file sorting, file preview, etc. it is convenient for users to "
                                "manage system files intuitively on the interface."));
	textCursor.setBlockFormat(blockFormat);
    ui->briefTextedit->setTextCursor(textCursor);

    if (QGSettings::isSchemaInstalled("org.ukui.style")) {
        m_gSettings = new QGSettings("org.ukui.style", QByteArray(), this);
        connect(m_gSettings, &QGSettings::changed, this, [=](const QString &key) {
            if (key == "styleName") {
                setSupportText();
            }
            else if("systemFontSize" == key)
            {
                auto appfont = qApp->font();
                QFont namefont = appfont;
                namefont.setBold(true);
                namefont.setPointSize(appfont.pointSize()*1.28);
                ui->nameLabel->setFont(namefont);
            }
        });
    }
    this->setSupportText();
    ui->openlinkLabel->setOpenExternalLinks(true);
//    ui->openlinkLabel->setStyleSheet("color:#595959;");
	textCursor.setBlockFormat(blockFormat);
}

void AboutDialog::setSupportText()
{
    QString text = tr("Service & Support: ") + "<a href=\"mailto://support@kylinos.cn\" style=\"color:"
                   + convertRGB16HexStr(palette().color(QPalette::ButtonText))
                   + ";\">support@kylinos.cn</a><br/>";
    ui->openlinkLabel->setText(text);
}

QString AboutDialog::convertRGB16HexStr(const QColor &color)
{
    if (color == Qt::transparent)
        return "#000000";//透明则显示黑色
    QString red = QString("%1").arg(color.red(),2,16,QChar('0'));
    QString green = QString("%1").arg(color.green(),2,16,QChar('0'));
    QString blue = QString("%1").arg(color.blue(),2,16,QChar('0'));
    QString hexStr = "#" + red + green + blue;
    return hexStr;
}

QString AboutDialog::getCurrentVersion()
{
    //use self define main version
    return VERSION;

    FILE *pp = NULL;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    char *q = NULL;
    QString version = tr("none");

    pp = popen("dpkg -l peony", "r");
    if(NULL == pp)
        return version;

    while((read = getline(&line, &len, pp)) != -1){
        q = strrchr(line, '\n');
        *q = '\0';

        QString content = line;
        QStringList list = content.split(" ");

        list.removeAll("");

        if (list.size() >= 3)
            version = list.at(2);
    }

    free(line);
    pclose(pp);
    return version;
}

void AboutDialog::on_closeBtn_clicked()
{
    close();
}

