#include "study-status-widget.h"
#include <unistd.h>
#include <QLabel>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QStringList>
#include <iterator>
#include <QDebug>
#include <QPainter>
#include <QDBusInterface>
#include <QStandardPaths>
#include <QDateTime>
#include <QPushButton>
#include <QScrollArea>
#include <QScrollBar>
#include "progress-widget.h"
//#include "progress-item-delegate.h"

#include "../../tablet/data/tablet-app-manager.h"

StudyStatusWidget::StudyStatusWidget(QList<TABLETAPP> appList, QWidget *parent) :
    QWidget(parent)
{
    m_appList= appList;
    initWidget();
}

StudyStatusWidget::~StudyStatusWidget()
{
    //delete pUkuiMenuInterface;
    if(nullptr != m_todayTimeLabel)
    {
        delete m_todayTimeLabel;
        m_todayTimeLabel = nullptr;
    }
    if(nullptr != m_weekTimeLabel)
    {
        delete m_weekTimeLabel;
        m_weekTimeLabel = nullptr;
    }
    if(nullptr != m_monthTimeLabel)
    {
        delete m_monthTimeLabel;
        m_monthTimeLabel = nullptr;
    }
    if(nullptr != m_progressGridLayout)
    {
        delete m_progressGridLayout;
        m_progressGridLayout = nullptr;
    }
    if(nullptr != m_mainVboxLayout)
    {
        delete m_mainVboxLayout;
        m_mainVboxLayout = nullptr;
    }
}
void StudyStatusWidget::initWidget()
{
    this->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_StyledBackground,true);
    this->setStyleSheet("border:0px;background:transparent;\
                        background-color:rgba(255, 255, 255);\
                        border-radius:15px;");
     //widget->setWindowOpacity(0.9);

    //this->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

    QLabel* titleLabel = new QLabel(this);
    //设置字号
    QFont ft;
    ft.setPointSize(18);
    ft.setWeight(70);
    titleLabel->setFont(ft);

    //设置颜色
    titleLabel->setStyleSheet("color:#9370DB");
    titleLabel->setText(tr("学情中心"));
    titleLabel->setAlignment(Qt::AlignLeft|Qt::AlignTop);

   // QWidget*  userInfoWidget = new QWidget(this);
    QHBoxLayout* userInfoLayout = new QHBoxLayout;

    m_userIconLabel = new QLabel(this);
    m_userNameLabel = new QLabel(this);
    m_userNameLabel->setFont(ft);
    m_userIconLabel->setAlignment(Qt::AlignRight|Qt::AlignTop);
    m_userNameLabel->setAlignment(Qt::AlignRight|Qt::AlignTop);

    userInfoLayout->addWidget(titleLabel);
    userInfoLayout->addStretch();
    userInfoLayout->addWidget(m_userIconLabel);
    userInfoLayout->addWidget(m_userNameLabel);
   // userInfoLayout->addSpacing(10);
   // userInfoLayout->setAlignment(Qt::AlignRight);
    //userInfoWidget->setLayout(userInfoLayout);

    QLabel* todayTitleLabel = new QLabel(this);
    //todayTitleLabel->setFixedSize();

    QLabel* weekTitleLabel = new QLabel(this);
    QLabel* monthTitleLabel = new QLabel(this);
    //设置字号
    ft.setPointSize(12);
    ft.setWeight(50);
    todayTitleLabel->setFont(ft);
    weekTitleLabel->setFont(ft);
    monthTitleLabel->setFont(ft);

    //设置字体颜色
    todayTitleLabel->setStyleSheet("color:#9C9C9C");
    weekTitleLabel->setStyleSheet("color:#9C9C9C");
    monthTitleLabel->setStyleSheet("color:#9C9C9C");

    todayTitleLabel->setText(tr("今日使用时长"));
    weekTitleLabel->setText(tr("本周使用时长"));
    monthTitleLabel->setText(tr("本月使用时长"));

    m_todayTimeLabel = new QLabel(this);
    m_weekTimeLabel = new QLabel(this);
    m_monthTimeLabel = new QLabel(this);

    ft.setPointSize(14);
    m_todayTimeLabel->setFont(ft);
    m_weekTimeLabel->setFont(ft);
    m_monthTimeLabel->setFont(ft);

    m_todayTimeLabel->setStyleSheet("color:#1C1C1C");
    m_weekTimeLabel->setStyleSheet("color:#1C1C1C");
    m_monthTimeLabel->setStyleSheet("color:#1C1C1C");

    QGridLayout* timeGridLayout = new QGridLayout;
   // timeGridLayout->addWidget(titleLabel,0,0,1,1);
   // timeGridLayout->addWidget(userInfoWidget,0,2,1,1);
    timeGridLayout->addWidget(todayTitleLabel,0,0);
    timeGridLayout->addWidget(weekTitleLabel,0,1);
    timeGridLayout->addWidget(monthTitleLabel,0,2);
    timeGridLayout->addWidget(m_todayTimeLabel,1,0);
    timeGridLayout->addWidget(m_weekTimeLabel,1,1);
    timeGridLayout->addWidget(m_monthTimeLabel,1,2);

//    timeGridLayout->setColumnStretch(0,1);
//    timeGridLayout->setColumnStretch(0,1);
//    timeGridLayout->setColumnStretch(1,1);
    timeGridLayout->setRowStretch(0,1);
    timeGridLayout->setRowStretch(1,2);

    m_mainVboxLayout = new QVBoxLayout;
    m_mainVboxLayout->addLayout(userInfoLayout);
    m_mainVboxLayout->addLayout(timeGridLayout);

    m_mainVboxLayout->addStretch();

    QFrame* line=new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setFixedHeight(1);
    line->setStyleSheet("background-color:rgba(0, 0, 0, 0.06)");
    //line->setFixedSize(this->width()-15*2, 1);
    m_mainVboxLayout->addWidget(line);

    QLabel* timeTitleLabel = new QLabel;
   // timeTitleLabel->setFixedSize(this->width()-15*2, 12);
    ft.setPointSize(12);
    timeTitleLabel->setFont(ft);
    timeTitleLabel->setText(tr("最常使用 (本周累计)"));
    timeTitleLabel->setStyleSheet("color:#9C9C9C");
    m_mainVboxLayout->addWidget(timeTitleLabel);

    m_progressGridLayout = new QGridLayout;
    for(int i = 0 ; i < m_appList.size(); ++i)
    {
        ProgressWidget* progress = new ProgressWidget(m_appList[i]);
        //progress->setFixedSize(this->width()-15*2,50);
        m_progressGridLayout->addWidget(progress,i/2 ,i%2);

    }

    m_scrollArea = new QScrollArea;
    QWidget* scrollWid = new QWidget;
    scrollWid->setLayout(m_progressGridLayout);

    m_scrollArea->setWidget(scrollWid);
    m_scrollArea->horizontalScrollBar()->setVisible(false);
    m_scrollArea->verticalScrollBar()->setVisible(true);
    m_scrollArea->verticalScrollBar()->setStyleSheet("QScrollBar{width:3px;padding-top:0px;padding-bottom:0px;background:transparent;border-radius:6px;}"
                                             "QScrollBar::handle{background-color:rgba(190, 190, 190 ,0.5); width:4px;border-radius:1.5px;}"
                                             "QScrollBar::handle:hover{background-color:#BEBEBE;border-radius:1.5px;}"
                                             "QScrollBar::handle:pressed{background-color:#BEBEBE;border-radius:1.5px;}"
                                             "QScrollBar::sub-line{background-color:transparent;height:0px;width:0px;}"
                                             "QScrollBar::add-line{background-color:transparent;height:0px;width:0px;}"
                                             );
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->adjustSize();

    m_progressGridLayout->setSpacing(20);
    m_updateTimeBt = new QPushButton(this);
    m_updateTimeBt->setFont(ft);
    m_updateTimeBt->setStyleSheet("color:#9C9C9C");

    connect(m_updateTimeBt,SIGNAL(clicked()), this,SIGNAL(updateTimeSignal()));
    QDateTime curTime = QDateTime::currentDateTime();//获取当前时间
    m_updateTimeBt->setText(QString("更新于：") + curTime.toString("MM.dd HH:mm"));
    m_updateTimeBt->setIcon(QIcon("/home/user_23799a/yyw/yyw/refresh.svg"));
    m_updateTimeBt->setLayoutDirection(Qt::RightToLeft);
    //m_progressGridLayout->addWidget(m_updateTimeBt,2 ,1);

    m_mainVboxLayout->addWidget(m_scrollArea);

    QSpacerItem *space =new QSpacerItem(this->width()-20,8,QSizePolicy::Expanding,
    QSizePolicy::Ignored);

    QHBoxLayout* updateTimeLayout = new QHBoxLayout;
    updateTimeLayout->addItem(space);
    updateTimeLayout->addWidget(m_updateTimeBt);
    m_mainVboxLayout->addLayout(updateTimeLayout);

    m_mainVboxLayout->setMargin(30);

    this->setLayout(m_mainVboxLayout);

    initUserInfo();
}

void StudyStatusWidget::initUserInfo()
{
    qlonglong uid = getuid();

    QDBusInterface user("org.freedesktop.Accounts",
                        "/org/freedesktop/Accounts",
                        "org.freedesktop.Accounts",
                        QDBusConnection::systemBus());

    QDBusMessage result = user.call("FindUserById", uid);
    QString userpath = result.arguments().value(0).value<QDBusObjectPath>().path();
    qDebug() << userpath;
    m_userInterface = new QDBusInterface ("org.freedesktop.Accounts",
                                           userpath,
                                           "org.freedesktop.Accounts.User",
                                           QDBusConnection::systemBus());
    QString userName = "Hi," + m_userInterface->property("RealName").value<QString>() + "同学";
    QString userIconPath = m_userInterface->property("IconFile").value<QString>();
    m_userInterface->connection().connect("org.freedesktop.Accounts",userpath, "org.freedesktop.DBus.Properties", "PropertiesChanged",
                                    this, SLOT(AccountSlots(QString, QMap<QString, QVariant>, QStringList)));

    m_userNameLabel->setText(userName);

    if (userIconPath != QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/.face")
    {
        m_userIconLabel->setPixmap(PixmapToRound(userIconPath, 20));
    }
    else
    {
        qDebug() << "Connot found avatar image";
    }

}

QPixmap StudyStatusWidget::PixmapToRound(const QString &src, int radius)
{
    if (src == "") {
        return QPixmap();
    }
    QPixmap pixmapa(src);
    QPixmap pixmap(radius*2,radius*2);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    QPainterPath path;
    path.addEllipse(0, 0, radius*2, radius*2);
    painter.setClipPath(path);
    painter.drawPixmap(0, 0, radius*2, radius*2, pixmapa);
    return pixmap;
}
void StudyStatusWidget::paintProgressSlot(QList<TABLETAPP> applist)
{
    qDebug("StudyStatusWidget::paintProgressSlot/n");
    int i = 0;
    for(; i < applist.size(); i++)
    {
        qDebug("StudyStatusWidget::paintProgressSlot :%d:%d/n",i/2,i%2);
        QLayoutItem* item= m_progressGridLayout->itemAtPosition(i/2,i%2);
        if(nullptr == item)
        {
            ProgressWidget* progressWid = new ProgressWidget(applist[i],this);
            m_progressGridLayout->addWidget(progressWid,i/2,i%2);
        }
        else
        {
            ProgressWidget* progressWid = dynamic_cast<ProgressWidget*>(item->widget());
            progressWid->paintSlot(applist[i]);
        }
    }
    for(;i/2 < m_progressGridLayout->rowCount(); i++)
    {
        QLayoutItem* item = m_progressGridLayout->itemAtPosition(i/2,i%2);
        if(nullptr == item)
        {
            break;
        }
        ProgressWidget* progressWid = dynamic_cast<ProgressWidget*>(item->widget());
        delete progressWid;
    }
}
void StudyStatusWidget::timeChangeSlot(QString strMethod ,QString strTime)
{
    qDebug("StudyStatusWidget::timeChangeSlot : method:%s, time:%s/n",strMethod.toLocal8Bit().data(),strTime.toLocal8Bit().data());
    if(strMethod == "GetDayUseTime")
    {
        m_todayTimeLabel->setText(strTime.toLocal8Bit().data());
    }
    else if(strMethod == "GetWeekUseTime")
    {
        m_weekTimeLabel->setText(strTime.toLocal8Bit().data());
    }
    else if(strMethod == "GetMonthUseTime")
    {
        m_monthTimeLabel->setText(strTime.toLocal8Bit().data());
    }
}
void StudyStatusWidget::markTimeSlot()
{
    QDateTime curTime = QDateTime::currentDateTime();//获取当前时间
    m_updateTimeBt->setText(QString("更新于：") + curTime.toString("MM.dd HH:mm"));
}