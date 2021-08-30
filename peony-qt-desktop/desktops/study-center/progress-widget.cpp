#include "progress-widget.h"
#include <QProgressBar>
#include <QLabel>
#include <QGridLayout>
#include <QHBoxLayout>

ProgressWidget::ProgressWidget(const QString &strAppName,long int iTime, QWidget *parent)
    : QWidget(parent)
{
    this->m_iTime = iTime;
    this->m_strAppName = strAppName;
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
    m_processBar->setValue(50);
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
    pix.load("/home/kylin/.cache/uksc/icons/classified-ads.png");
   // pix = pix.scaled(m_iconLabel->width(), m_iconLabel->height());
    //m_iconLabel->setPixmap(pix);

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
