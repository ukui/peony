/*
 * Peony-Qt
 *
 * Copyright (C) 2021, KylinSoft Co., Ltd.
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
 */

#include "progress-widget.h"
#include "../../tablet/src/Style/style.h"
#include <QProgressBar>
#include <QLabel>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QIcon>

ProgressWidget::ProgressWidget(const TABLETAPP &AppInfo,QWidget *parent)
    : QWidget(parent)
{
    this->m_iTime = AppInfo.iTime;
    this->m_strAppName = AppInfo.appName;
    this->m_strAppIcon = AppInfo.appIcon;
    this->m_serialNumber = AppInfo.serialNumber;
    qDebug("ProgressWidget::ProgressWidget time:%d,name:%s,icon:%s/n",this->m_iTime, this->m_strAppName.toLocal8Bit().data(), this->m_strAppIcon.toLocal8Bit().data());
    initUi();
}
ProgressWidget::~ProgressWidget()
{

}
void ProgressWidget::initUi()
{
    this->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_StyledBackground,true);

    m_processBar = new QProgressBar(this);
    m_processBar->setMaximumHeight(8);
    m_processBar->setRange(0,100);
    m_processBar->setValue(m_iTime);
    m_processBar->setStyleSheet("QProgressBar{border:none;border-radius:4px;"
                                "background-color:rgba(118,118,124,0.25 );}"
                                "QProgressBar::chunk{background-color:#2FB3E8;border:none;border-radius:4px;}");
    m_processBar->setAlignment(Qt::AlignTop);

    m_processBar->setTextVisible(false);

    m_nameLabel = new QLabel(this);
    //bug#114984 文件名字能够显示缩略
    QFontMetrics fm = this->fontMetrics();
    QString elidedAppName = fm.elidedText(m_strAppName, Qt::ElideRight, m_nameLabel->width());
    m_nameLabel->setText(elidedAppName);

    m_nameLabel->setAlignment(Qt::AlignLeft|Qt::AlignBottom);

    m_iconLabel = new QLabel(this);
    m_iconLabel->setFixedSize(Style::ProgressIconSize, Style::ProgressIconSize);
    QString iconstr= m_strAppIcon;
    iconstr.remove(".png");
    iconstr.remove(".svg");
    QIcon icon=QIcon::fromTheme(iconstr);
    QPixmap pix = icon.pixmap(m_iconLabel->width(), m_iconLabel->height());
    m_iconLabel->setPixmap(pix);

    m_timeLabel = new QLabel(this);
    int iHour = m_iTime/60;
    int iMin = m_iTime%60;
    if(0 < iHour)
    {
        m_timeLabel->setText(QString("%1小时%2分钟").arg(iHour).arg(iMin));
    }
    else
    {
        m_timeLabel->setText(QString("%1分钟").arg(iMin));
    }

    m_timeLabel->setAlignment(Qt::AlignRight|Qt::AlignBottom);

    m_timeGridLayout = new QGridLayout;
    m_timeGridLayout->addWidget(m_iconLabel,0,0,4,1);
    m_timeGridLayout->addWidget(m_nameLabel,0,1,1,2);
    m_timeGridLayout->addWidget(m_timeLabel,0,3,1,2);
    m_timeGridLayout->addWidget(m_processBar,2,1,1,4);
    m_timeGridLayout->setContentsMargins(0,0,0,0);
   // m_timeGridLayout->setSpacing(1);
    this->setLayout(m_timeGridLayout);
}
void  ProgressWidget::paintSlot(TABLETAPP &app)
{
    qDebug("ProgressWidget::paintSlot :time:%d ,name:%s, path:%s/n",app.iTime,m_strAppName.toLocal8Bit().data(),m_strAppIcon.toLocal8Bit().data());
    this->m_iTime = app.iTime;
    this->m_strAppName = app.appName;
    this->m_strAppIcon = app.appIcon;
    int iHour = m_iTime/60;
    int iMin = m_iTime%60;
    if(0 < iHour)
    {
        m_timeLabel->setText(QString("%1小时%2分钟").arg(iHour).arg(iMin));
    }
    else
    {
        m_timeLabel->setText(QString("%1分钟").arg(iMin));
    }

    m_processBar->setValue(m_iTime);
    //bug#114984 文件名字能够显示缩略
    QFontMetrics fm = this->fontMetrics();
    QString elidedAppName = fm.elidedText(m_strAppName, Qt::ElideRight, m_nameLabel->width());
    m_nameLabel->setText(elidedAppName);

    QString iconstr= m_strAppIcon;
    iconstr.remove(".png");
    iconstr.remove(".svg");
    QIcon icon=QIcon::fromTheme(iconstr);
    //bug#114984 平板的图标太大
    QPixmap pix = icon.pixmap(m_iconLabel->width(), m_iconLabel->height());
    m_iconLabel->setPixmap(pix.scaled(m_iconLabel->width(), m_iconLabel->height(), Qt::KeepAspectRatio));
}

void ProgressWidget::setMaximum(int iMaxValue)
{
    if(iMaxValue <= 0 )
    {
        iMaxValue = 100;
    }
    m_processBar->setMaximum(iMaxValue);
}

void ProgressWidget::changeThemeSlot(QString strTheme)
{
    if (strTheme == "ukui-dark")
    {
        //深色主题
        m_timeLabel->setStyleSheet("QLabel{color:rgba(255,255,255,0.45);font-size:16px}");
    }
    else
    {
        //浅色主题
        m_timeLabel->setStyleSheet("QLabel{color:rgba(38,38,38,0.45);font-size:16px}");
    }
}
