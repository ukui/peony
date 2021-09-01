#include "progress-widget.h"
#include <QProgressBar>
#include <QLabel>
#include <QGridLayout>
#include <QHBoxLayout>

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
    //m_processBar->setFixedSize(this->width()/5*4,8);
    m_processBar->setRange(0,100);
    m_processBar->setValue(m_iTime);
    m_processBar->setStyleSheet("border-radius:10px;"
                                    "background: Grey;");

    m_processBar->setTextVisible(false);

    m_nameLabel = new QLabel(this);
    m_nameLabel->setText(m_strAppName);
    //m_nameLabel->setFixedSize(this->width()/5*2,this->height()/2);
    m_nameLabel->setAlignment(Qt::AlignLeft);

    m_iconLabel = new QLabel(this);
   // m_iconLabel->setFixedSize(this->width()/5,this->height());
    QPixmap pix;
    pix.load(m_strAppIcon);
    pix = pix.scaled(m_iconLabel->width(), m_iconLabel->height());
    m_iconLabel->setPixmap(pix);

    m_timeLabel = new QLabel(this);
    m_timeLabel->setText(QString("%1小时%2分钟").arg(m_iTime/60).arg(m_iTime%60));
   // m_timeLabel->setFixedSize(this->width()/5*2,this->height()/2);
    m_timeLabel->setAlignment(Qt::AlignRight);

    m_timeGridLayout = new QGridLayout;
    m_timeGridLayout->addWidget(m_iconLabel,0,0,2,1);
    m_timeGridLayout->addWidget(m_nameLabel,0,1,1,2);
    m_timeGridLayout->addWidget(m_timeLabel,0,2,1,2);
    m_timeGridLayout->addWidget(m_processBar,1,1,1,4);
   // gridLayout->setMargin(80);
    m_timeGridLayout->setSpacing(1);
    this->setLayout(m_timeGridLayout);
}
void  ProgressWidget::paintSlot(TABLETAPP &app)
{
    qDebug("ProgressWidget::paintSlot :time:%s ,name:%s, path:%s/n",app.iTime,m_strAppName.data(),m_strAppIcon.data());
    this->m_iTime = app.iTime;
    this->m_strAppName = app.appName;
    this->m_strAppIcon = app.appIcon;
    m_timeLabel->setText(QString("%1小时%2分钟").arg(m_iTime/60).arg(m_iTime%60));
    m_processBar->setValue(m_iTime);
    m_nameLabel->setText(m_strAppName);
    QPixmap pix;
    pix.load(m_strAppIcon);
    pix = pix.scaled(m_iconLabel->width(), m_iconLabel->height());
    m_iconLabel->setPixmap(pix);
}
