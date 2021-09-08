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
#include <QTime>
#include <QPushButton>
//#include <QStandardItemModel>
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
//    QMap<QString, ProgressWidget*>::iterator ite = m_progressMap.begin();
//    for(;ite != m_progressMap.end(); ite++)
//    {
//        if(nullptr != *ite)
//        {
//            delete *ite;
//        }
//    }
}
void StudyStatusWidget::initWidget()
{
//    int iWidth = 1520-400;
//    int iHeight = 634-100;
//    this->setFixedSize(iWidth+200, iHeight+40);
//    this->setFixedSize(iWidth+200, iHeight+40);
    this->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_StyledBackground,true);
    this->setStyleSheet("border:0px;background:transparent;\
                        background-color:rgba(255, 255, 255);\
                        border-radius:15px;");
     //widget->setWindowOpacity(0.9);

    //this->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

    QLabel* titleLabel = new QLabel;
    //设置字号
    QFont ft;
    ft.setPointSize(18);
    titleLabel->setFont(ft);

    //设置颜色
//    QPalette pa;
//    pa.setColor(QPalette::WindowText,Qt::red);
//    titleLabel->setPalette(pa);
    titleLabel->setStyleSheet("color:#9370DB");
    titleLabel->setText("学情中心");

    QWidget*  userInfoWidget = new QWidget(this);
    QHBoxLayout* userInfoLayout = new QHBoxLayout(userInfoWidget);

    m_userIconLabel = new QLabel(this);
   // m_userIconLabel->setFixedSize(40, 40);

    m_userNameLabel = new QLabel();
    m_userNameLabel->setFont(ft);
    userInfoLayout->addWidget(m_userIconLabel);
    userInfoLayout->addWidget(m_userNameLabel);
   // userInfoLayout->addSpacing(10);
    userInfoLayout->setAlignment(Qt::AlignRight);
    userInfoWidget->setLayout(userInfoLayout);

    QLabel* todayTitleLabel = new QLabel;
    //todayTitleLabel->setFixedSize();

    QLabel* weekTitleLabel = new QLabel;
    QLabel* monthTitleLabel = new QLabel;
    //设置字号
    ft.setPointSize(12);
    todayTitleLabel->setFont(ft);
    weekTitleLabel->setFont(ft);
    monthTitleLabel->setFont(ft);

    //设置字体颜色
    todayTitleLabel->setStyleSheet("color:#9C9C9C");
    weekTitleLabel->setStyleSheet("color:#9C9C9C");
    monthTitleLabel->setStyleSheet("color:#9C9C9C");

    todayTitleLabel->setText("今日使用时长");
    weekTitleLabel->setText("本周使用时长");
    monthTitleLabel->setText("本月使用时长");

    m_todayTimeLabel = new QLabel;
    m_weekTimeLabel = new QLabel;
    m_monthTimeLabel = new QLabel;

    ft.setPointSize(14);
    m_todayTimeLabel->setFont(ft);
    m_weekTimeLabel->setFont(ft);
    m_monthTimeLabel->setFont(ft);

    m_todayTimeLabel->setStyleSheet("color:#1C1C1C");
    m_weekTimeLabel->setStyleSheet("color:#1C1C1C");
    m_monthTimeLabel->setStyleSheet("color:#1C1C1C");

    QGridLayout* timeGridLayout = new QGridLayout;
    timeGridLayout->addWidget(titleLabel,0,0,1,1);
    timeGridLayout->addWidget(userInfoWidget,0,2,1,1);
    timeGridLayout->addWidget(todayTitleLabel,1,0);
    timeGridLayout->addWidget(weekTitleLabel,1,1);
    timeGridLayout->addWidget(monthTitleLabel,1,2);
    timeGridLayout->addWidget(m_todayTimeLabel,2,0);
    timeGridLayout->addWidget(m_weekTimeLabel,2,1);
    timeGridLayout->addWidget(m_monthTimeLabel,2,2);

//    timeGridLayout->setColumnStretch(0,1);
//    timeGridLayout->setColumnStretch(0,1);
//    timeGridLayout->setColumnStretch(1,1);
    timeGridLayout->setRowStretch(0,1);
    timeGridLayout->setRowStretch(1,2);

    m_mainVboxLayout = new QVBoxLayout;
    m_mainVboxLayout->addWidget(titleLabel);
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
    timeTitleLabel->setText("最常使用 (本周累计)");
    timeTitleLabel->setStyleSheet("color:#9C9C9C");
    m_mainVboxLayout->addWidget(timeTitleLabel);

    m_progressGridLayout = new QGridLayout;
    for(int i = 0 ; i < m_appList.size(); ++i)
    {
        ProgressWidget* progress = new ProgressWidget(m_appList[i]);
        //progress->setFixedSize(this->width()-15*2,50);
        m_progressGridLayout->addWidget(progress,i/2 ,i%2);

    }
    // gridLayout->setMargin(80);
    //m_progressGridLayout->setSpacing(20);
    QTime curTime=QTime::currentTime(); //获取当前时间

    m_updateTimeBt = new QPushButton(this);
    m_updateTimeBt->setFont(ft);
    m_updateTimeBt->setStyleSheet("color:#9C9C9C");

    connect(m_updateTimeBt,SIGNAL(clicked()), this,SIGNAL(updateTimeSignal()));
    m_updateTimeBt->setText(QString("更新于：今天%1:%2").arg(curTime.hour()).arg(curTime.minute()));
    m_progressGridLayout->addWidget(m_updateTimeBt,2 ,1);

    m_mainVboxLayout->addLayout(m_progressGridLayout);
    m_mainVboxLayout->setMargin(30);
//    m_showProgressView = new QListView ;
//    QStandardItemModel* listmodel=new QStandardItemModel;
//    QStringList m_strListData;
//    QString desktop=QString("/usr/share/applications/xdiagnose.desktop");
//    m_strListData.append(desktop);
//    desktop=QString("/usr/share/applications/yelp.desktop");
//    m_strListData.append(desktop);
//    m_showProgressView->setModel(listmodel);
//    Q_FOREACH(QString desktopfp,m_strListData)
//    {
//        QStandardItem* item=new QStandardItem;
//        item->setData(QVariant::fromValue<QString>(desktopfp),Qt::DisplayRole);
//        listmodel->appendRow(item);
//    }
//    FullItemDelegate* delegate= new FullItemDelegate(m_showProgressView,0);
//    m_showProgressView->setItemDelegate(delegate);
    //m_mainVboxLayout->addWidget(m_showProgressView);
    this->setLayout(m_mainVboxLayout);

    initUserInfo();
    /*BUS类型	SESSION BUS
    DBUS名称	com.ukui.app.info
    OBJECT路径	/com/ukui/app/info/time
    INTERFACES名称	com.ukui.app.info.time.interface
    */
    //bool QDBusConnection::connect(const QString &service, const QString &path, const QString &interface, const QString &name, QObject *receiver, const char *slot)
//    QDBusConnection::sessionBus().connect("com.ukui.app.info","/com/ukui/app/info/time","com.ukui.app.info.time.interface","GetDayUseTime",this,SLOT(setAppTimeSlot(QStringList)));
//    QDBusConnection::sessionBus().connect("com.ukui.app.info","/com/ukui/app/info/time","com.ukui.app.info.time.interface","GetWeekUseTime",this,SLOT(setAppTimeSlot(QStringList)));
//    QDBusConnection::sessionBus().connect("com.ukui.app.info","/com/ukui/app/info/time","com.ukui.app.info.time.interface","GetMonthUseTime",this,SLOT(setAppTimeSlot(QStringList)));
//    QDBusConnection::sessionBus().connect("com.ukui.app.info","/com/ukui/app/info/time","com.ukui.app.info.time.interface","GetWeekCumulativeTime",this,SLOT(setAppTimeSlot(QStringList)));


    // m_mainVboxLayout->addWidget(new QWidget());
//    m_gridLayout = new QGridLayout;
//    m_gridLayout->addLayout()
//    m_timeView = new QTableView
//    QStandardItemModel* timeTitleModel = new QStandardItemModel();
//    timeTitleModel->setHorizontalHeaderItem(0,new QStandardItem(QObject::tr("今日使用时长")));
//    timeTitleModel->setHorizontalHeaderItem(1,new QStandardItem(QObject::tr("本周使用时长")));
//    timeTitleModel->setHorizontalHeaderItem(2,new QStandardItem(QObject::tr("本月使用时长")));
//    timeTitleModel->item(0,0)->setFont(QFont("Times",10,QFont::Black));
//    timeTitleModel->item(0,1)->setFont(QFont("Times",10,QFont::Black));
//    timeTitleModel->item(0,2)->setFont(QFont("Times",10,QFont::Black));
//    timeTitleModel->setItem(0,1,new QStandardItem(QObject::tr("0小时0分钟") );
//    timeTitleModel->setItem(0,1,new QStandardItem(QObject::tr("0小时0分钟") );
//    timeTitleModel->setItem(0,1,new QStandardItem(QObject::tr("0小时0分钟") );

//    m_timeView->setModeal(timeTitleModel);
//    m_timeView->horizontalHeader->setResizeMode(0,QHeaderView::Fixed);
//    m_timeView->horizontalHeader->setResizeMode(1,QHeaderView::Fixed);
//    m_timeView->horizontalHeader->setResizeMode(2,QHeaderView::Fixed);
//    m_timeView->verticalHeader()->hide();
//    m_timeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
//    m_timeView->setShowGrid(false);

//    m_gridLayout->addWidget(m_timeView);
//    m_showProgressView = new QlistView ;
//    QStandardItemModelabl* listmodel=new QStandardItemModel(this);
//    m_listview->setModel(listmodel);
//    Q_FOREACH(QString desktopfp,m_strListData)
//    {
//        QStandardItem* item=new QStandardItem;
//        item->setData(QVariant::fromValue<QString>(desktopfp),Qt::DisplayRole);
//        listmodel->appendRow(item);
//    }
//    m_timeView;
//    QlistView* m_showProgressView=nullptr;
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
    QString userName = m_userInterface->property("RealName").value<QString>();
    QString userIconPath = m_userInterface->property("IconFile").value<QString>();
    m_userInterface->connection().connect("org.freedesktop.Accounts",userpath, "org.freedesktop.DBus.Properties", "PropertiesChanged",
                                    this, SLOT(AccountSlots(QString, QMap<QString, QVariant>, QStringList)));

    m_userNameLabel->setText(userName);

    if (userIconPath != QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/.face")
    {
        m_userIconLabel->setPixmap(PixmapToRound(userIconPath, 10));
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
    for(int i = 0; i < applist.size(); i++)
    {
        qDebug("StudyStatusWidget::paintProgressSlot :%d:%d/n",i/2,i%2);
        QLayoutItem* item= m_progressGridLayout->itemAtPosition(i/2,i%2);
        ProgressWidget* progressWid = dynamic_cast<ProgressWidget*>(item->widget());
        progressWid->paintSlot(applist[i]);
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
    QTime curTime=QTime::currentTime(); //获取当前时间
    m_updateTimeBt->setText(QString("更新于：今天%1:%2").arg(curTime.hour()).arg(curTime.minute()));
}
