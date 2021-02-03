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
//    QFont font14,font18;
	QTextCursor textCursor;
	QTextBlockFormat blockFormat;
//	QString addressLabel = tr("Offical Website: ");
    QString supportLabel = tr("Service & Support: ");
//	QString phoneLabel = tr("Hot Service: ");

//    font18.setPointSize(18);
//    font14.setPointSize(14);
	palette.setColor(QPalette::Highlight, QColor("#E54A50"));
	blockFormat.setLineHeight(24, QTextBlockFormat::SingleHeight);//font-line-hight

    setAutoFillBackground(true);
    setBackgroundRole(QPalette::Base);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);//modal、borderless window

    ui->logoLabel->setPixmap(QIcon::fromTheme("system-file-manager").pixmap(24,24));

//    ui->titleLabel->setFont(font14);
    ui->titleLabel->setText(tr("File Manager"));

    ui->closeBtn->setFlat(true);
	ui->closeBtn->setPalette(palette);
    ui->closeBtn->setIconSize(QSize(24,24));
    ui->closeBtn->setProperty("isWindowButton",0x2);
    ui->closeBtn->setProperty("useIconHighlightEffect",0x2);
    ui->closeBtn->setIcon(QIcon::fromTheme("window-close-symbolic"));
	
    ui->iconLabel->setPixmap(QIcon::fromTheme("system-file-manager").pixmap(96,96));

//    ui->nameLabel->setFont(font18);
    ui->nameLabel->setText(tr("File Manager"));

//    ui->versionLabel->setFont(font14);
    ui->versionLabel->setStyleSheet("color:#595959;");
    ui->versionLabel->setText(QString(tr("Version number: %1")).arg(getCurrentVersion()));

//	ui->briefTextedit->setFont(font14);
	ui->briefTextedit->setReadOnly(true);
    ui->briefTextedit->setStyleSheet("color:#595959;");
	textCursor = ui->briefTextedit->textCursor();	//before the setText().
    ui->briefTextedit->setText(tr("File manager is a graphical software to help users manage system files. "
                                "It provides common file operation functions for users, such as file viewing, "
                                "file copy, paste, cut, delete, rename, file selection, application opening, "
                                "file search, file sorting, file preview, etc. it is convenient for users to "
                                "manage system files intuitively on the interface."));
	textCursor.setBlockFormat(blockFormat);
    ui->briefTextedit->setTextCursor(textCursor);

//	ui->developTextedit->setFont(font14);
	ui->developTextedit->setReadOnly(true);
    ui->developTextedit->setStyleSheet("color:#595959;");
	textCursor = ui->developTextedit->textCursor();//before the setText();
    ui->developTextedit->setText(supportLabel + "<a href=\"mailto://support@kylinos.cn\">support@kylinos.cn</a><br/>");
	textCursor.setBlockFormat(blockFormat);
    ui->developTextedit->setTextCursor(textCursor);
}

QString AboutDialog::getCurrentVersion()
{
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

//Rounded corners and shadows
void AboutDialog::paintEvent(QPaintEvent *event)
{
    QPainterPath path;
    QPainter painter(this);

    QColor m_defaultBackgroundColor = qRgb(65, 65, 65);
    QColor m_defaultBorderColor = qRgb(69, 69, 69);
    path.setFillRule(Qt::WindingFill);
    path.addRoundedRect(10, 10, this->width() - 20, this->height() - 20, 5, 5);

    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.fillPath(path, QBrush(QColor(m_defaultBackgroundColor.red(),
        m_defaultBackgroundColor.green(),
        m_defaultBackgroundColor.blue())));

    QColor color(45, 45, 45, 50);
    for (int i = 0; i < 5; i++)
    {
        QPainterPath path;
        path.setFillRule(Qt::WindingFill);
        path.addRoundedRect(5 - i, 5 - i, this->width() - (5 - i) * 2, this->height() - (5 - i) * 2, 5, 5);
        color.setAlpha(100 - qSqrt(i) * 50);
        painter.setPen(color);
        painter.drawPath(path);
    }

    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(palette().color(QPalette::Active, QPalette::Base));
    painter.setPen(Qt::transparent);
    QRect rect = this->rect();
    rect.setX(5);
    rect.setY(5);
    rect.setWidth(rect.width() - 5);
    rect.setHeight(rect.height() - 5);
    painter.drawRoundedRect(rect, 5, 5);
}
