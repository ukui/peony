#include "study-status-widget.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QStringList>
#include <iterator>
//#include <QListView>
//#include <QStandardItemModel>
#include "progress-widget.h"
//#include "progress-item-delegate.h"
//#include <QDBusMessage>
#include "../../tablet/data/tablet-app-manager.h"

StudyStatusWidget::StudyStatusWidget(QWidget *parent) :
    QWidget(parent)
{
    initWidget();
}

StudyStatusWidget::~StudyStatusWidget()
{
    //delete pUkuiMenuInterface;
    if(nullptr != m_todayTimeLabel)
    {
        delete m_todayTimeLabel;
    }
    if(nullptr != m_weekTimeLabel)
    {
        delete m_weekTimeLabel;
    }
    if(nullptr != m_monthTimeLabel)
    {
        delete m_monthTimeLabel;
    }
    if(nullptr != m_timeGridLayout)
    {
        delete m_timeGridLayout;
    }
    if(nullptr != m_mainVboxLayout)
    {
        delete m_mainVboxLayout;
    }
    QMap<QString, ProgressWidget*>::iterator ite = m_progressMap.begin();
    for(;ite != m_progressMap.end(); ite++)
    {
        if(nullptr != *ite)
        {
            delete *ite;
        }
    }
}
void StudyStatusWidget::initWidget()
{
//    int iWidth = 1520-400;
//    int iHeight = 634-100;
//    this->setFixedSize(iWidth+200, iHeight+40);
//    this->setFixedSize(iWidth+200, iHeight+40);
    this->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_StyledBackground,true);
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

    QStringList strAppList;
    strAppList<<"课程中心"<<"英语精准练习"<<"语文精准练习"<<"数学精准练习";

    m_timeGridLayout = new QGridLayout;
    long int iTime = 0;
    for(int i = 0 ; i < strAppList.size(); ++i)
    {
        ProgressWidget* progress = new ProgressWidget(strAppList.at(i),iTime);
        //progress->setFixedSize(this->width()-15*2,50);
        m_progressMap.insert(strAppList.at(i), progress);
        m_timeGridLayout->addWidget(progress,i/2 ,i%2);
    }
    // gridLayout->setMargin(80);
    //m_timeGridLayout->setSpacing(20);
    m_mainVboxLayout->addLayout(m_timeGridLayout);
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
    /*BUS类型	SESSION BUS
    DBUS名称	com.ukui.app.info
    OBJECT路径	/com/ukui/app/info/time
    INTERFACES名称	com.ukui.app.info.time.interface
    */
    //bool QDBusConnection::connect(const QString &service, const QString &path, const QString &interface, const QString &name, QObject *receiver, const char *slot)
//    QDBusConnection::sessionBus().connect("com.ukui.app.info","/com/ukui/app/info/time","com.ukui.app.info.time.interface","GetDayUseTime",this,SLOT(setAppTimeSlot(QStringList)));
//    QDBusConnection::sessionBus().connect("com.ukui.app.info","/com/ukui/app/info/time","com.ukui.app.info.time.interface","GetWeekUseTime",this,SLOT(setAppTimeSlot(QStringList)));
//    QDBusConnection::sessionBus().connect("com.ukui.app.info","/com/ukui/app/info/time","com.ukui.app.info.time.interface","GetMonthUseTime",this,SLOT(setAppTimeSlot(QStringList)));
//   a QDBusConnection::sessionBus().connect("com.ukui.app.info","/com/ukui/app/info/time","com.ukui.app.info.time.interface","GetWeekCumulativeTime",this,SLOT(setAppTimeSlot(QStringList)));
    QString strTime;
    QString strName;
    strName = "GetDayUseTime";
    strTime =getStudyTime(strName, strAppList);
    m_todayTimeLabel->setText(tr(strTime.toLocal8Bit().data()));

    strName = "GetWeekUseTime";
    strTime =getStudyTime(strName, strAppList);
    m_weekTimeLabel->setText(tr(strTime.toLocal8Bit().data()));

    strName = "GetMonthUseTime";
    strTime =getStudyTime(strName, strAppList);
    m_monthTimeLabel->setText(tr(strTime.toLocal8Bit().data()));

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

QString  StudyStatusWidget::getStudyTime(QString &strMethod, QStringList &appList)
{

//    QDBusMessage request = QDBusMessage::createMethodCall("com.ukui.app.info","/com/ukui/app/info/time","com.ukui.app.info.time.interface", strMethod.toLocal8Bit().data());
//    request<<appList;
//    QDBusMessage response = QDBusConnection::sessionBus().call(request);
   long int iStudyTime = 0;
//    if (response.type() == QDBusMessage::ReplyMessage)
 //   {
//        iWeekmm = response.arguments().takeFirst().toInt();
//        qDebug("study time:%d/n", iWeekmm);
//    }
//    else
//    {
//        qDebug( "get study time fail!/n");
//    }
    int iHour = iStudyTime/60;
    int iMin = iStudyTime/60;
    if(iHour > 0)
    {
       return QString("%1小时％2分钟").arg(iHour).arg(iMin);
    }
    else
    {
        return QString("％1分钟").arg(iMin);
    }

}
