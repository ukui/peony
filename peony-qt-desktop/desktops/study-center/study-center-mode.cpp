#include "study-center-mode.h"
#include "study-status-widget.h"
#include "study-directory-widget.h"
#include <QStandardItemModel>
#include <QListView>
#include <QStandardItem>
#include <QGridLayout>
#include <QString>
#include <QApplication>
#include <QLabel>
#include <QScreen>
#include <QDBusInterface>
#include <QDBusMessage>
#include <QDBusConnection>

using namespace Peony;


#define STUDY_CENTER_PRACTICE          "Practice"
#define STUDY_CENTER_STUGUARDS         "Student Guards"
#define STUDY_CENTER_SYN               "Synchronized"
#define STUDY_CENTER_STATISTICS        "Statistics"

StudyCenterMode::StudyCenterMode(QWidget *parent) : DesktopWidgetBase(parent)
{
    initUi();
}

StudyCenterMode::~StudyCenterMode()
{

}

void StudyCenterMode::setActivated(bool activated)
{
    DesktopWidgetBase::setActivated(activated);
}

DesktopWidgetBase *StudyCenterMode::initDesktop(const QRect &rect)
{
    return DesktopWidgetBase::initDesktop(rect);
}
void StudyCenterMode::initUi()
{
    this->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    //this->setAttribute(Qt::WA_TranslucentBackground,true);
    this->setAttribute(Qt::WA_StyledBackground,true);
    this->setStyleSheet("border:0px;background:transparent;");
    this->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    int ScreenWidth=QApplication::primaryScreen()->geometry().width();
    int ScreenHeight=QApplication::primaryScreen()->geometry().height();
    this->setFixedSize(ScreenWidth,ScreenHeight);
    m_tableAppMangager = TabletAppManager::getInstance();

    QMap<QString, QList<TabletAppEntity*>> studyCenterDataMap = m_tableAppMangager->getStudyCenterData();
    QStringList strListTitleStyle;
    strListTitleStyle<<"精准练习"<<"color:#009ACD";
    QMap<QString, QList<TabletAppEntity*>> dataMap;

    dataMap.insert(STUDY_CENTER_MATH,studyCenterDataMap[STUDY_CENTER_MATH]);
    dataMap.insert(STUDY_CENTER_ENGLISH,studyCenterDataMap[STUDY_CENTER_ENGLISH]);
    dataMap.insert(STUDY_CENTER_CHINESE,studyCenterDataMap[STUDY_CENTER_CHINESE]);
    dataMap.insert(STUDY_CENTER_OTHER,studyCenterDataMap[STUDY_CENTER_OTHER]);
    StudyDirectoryWidget* practiceWidget = new StudyDirectoryWidget(strListTitleStyle,dataMap, 1);

    dataMap.clear();
    strListTitleStyle.clear();
    strListTitleStyle<<"守护中心"<<"color:#43CD80";
    dataMap.insert(STUDY_CENTER_SYNCHRONIZED,studyCenterDataMap[STUDY_CENTER_SYNCHRONIZED]);
    StudyDirectoryWidget* guradWidget = new StudyDirectoryWidget(strListTitleStyle,dataMap);

    dataMap.clear();
    strListTitleStyle.clear();
    strListTitleStyle<<"同步学习"<<"color:#FF8247";
    dataMap.insert(STUDY_CENTER_STUDENT_GUARD,studyCenterDataMap[STUDY_CENTER_STUDENT_GUARD]);
    StudyDirectoryWidget* synWidget = new StudyDirectoryWidget(strListTitleStyle,dataMap);

    QList<TABLETAPP> appList = getTimeOrder(studyCenterDataMap);
    StudyStatusWidget* widget4 = new StudyStatusWidget(appList,this);

    connect(this,SIGNAL(valueChangedSingal(QList<TABLETAPP> &applist)),widget4,SLOT(paintProgressSlot(QList<TABLETAPP> &applist)));
    connect(this,SIGNAL(timeChangedSingal(QString,QString)),widget4,SLOT(timeChangeSlot(QString,QString)));

    initTime();
//    widget1->setFixedSize(iWidth/2, iHeight);
//    widget2->setFixedSize(iWidth/4, iHeight/3);
//    widget3->setFixedSize(iWidth/4, iHeight/3);
//    widget4->setFixedSize(iWidth/4, iHeight/3*2);
//    QHBoxLayout hboxLayout;
//    hboxLayout->addWidget(widget2);
//    hboxLayout->addWidget(widget3);
//    QVBoxLayout vboxLayout;
//    vboxLayout->addLayout(hboxLayout);
//    vboxLayout->addWidget(widget4);

    QGridLayout* gridLayout = new QGridLayout;

    gridLayout->addWidget(practiceWidget,0,0,3,2);
    gridLayout->addWidget(guradWidget,0,2,1,1);
    gridLayout->addWidget(synWidget,0,3,1,1);
    gridLayout->addWidget(widget4,1,2,2,2);

//    gridLayout->setColumnStretch(0,1);
//   // gridLayout->setColumnStretch(3,1);
//    gridLayout->setRowStretch(2,1);
//    gridLayout->setRowStretch(3,1);
    //gridLayout->setRowStretch(4,1);
    gridLayout->setMargin(80);
    gridLayout->setSpacing(20);
    this->setLayout(gridLayout);


//    QVBoxLayout* m_mainLayout = new QVBoxLayout(this);
//    //m_mainLayout->setContentsMargins(1,0,0,0);
//    QListView* m_listview = new QListView;

//    QStringList m_strListData ={"1","2","3","4"};
//    QStandardItemModel* listmodel=new QStandardItemModel(this);
//    m_listview->setModel(listmodel);
//    m_listview->setFixedSize(200,200);
//    for(int i = 0;i< m_strListData.size();i++)
//    {
//        QString str= static_cast<QString>(m_strListData.at(i));
//        QStandardItem* item=new QStandardItem(str);
//        listmodel->appendRow(item);
//    }
//    //this->addWidget(m_listview);
//    m_mainLayout->addWidget(m_listview);
    //this->setLayout(m_mainLayout);
}

QList<TABLETAPP>  StudyCenterMode::getTimeOrder(QMap<QString, QList<TabletAppEntity*>> studyCenterDataMap )
{
    QMap<QString, QList<TabletAppEntity*>>::const_iterator ite = studyCenterDataMap.begin();
    QList<TABLETAPP> maxTimeList;
    TABLETAPP maxTimeApp;
    maxTimeApp.desktopName = "";
    maxTimeApp.appName = "";
    maxTimeApp.appIcon = "";
    maxTimeApp.execCommand = "";
    maxTimeApp.iTime = -1;

    m_appList.clear();
    for(; ite != studyCenterDataMap.constEnd(); ++ite)
    {
        QList<TabletAppEntity*> tabletAppList = ite.value();
        for(int i = 0; i < tabletAppList.size(); ++i)
        {

            long int iWeekmm = getStudyTime("GetWeekCumulativeTime", tabletAppList[i]->appName);
            qDebug("StudyCenterMode::getTimeOrder:%d,name:%s/n", iWeekmm,tabletAppList[i]->appName.toLocal8Bit().data());
            for(int j = 0; j < maxTimeList.size(); ++j)
            {
                m_appList.push_back(tabletAppList[i]->appName);

                qDebug("StudyCenterMode::getTimeOrder tabletAppList[%d] :%s/n", i, tabletAppList[i]->appName.toLocal8Bit().data());
                if(iWeekmm > maxTimeList[j].iTime)
                {
                    qDebug("StudyCenterMode::getTimeOrder  maxTimeList.insert tabletAppList[%d] :%s/n", i, tabletAppList[i]->appName.toLocal8Bit().data());

                    TABLETAPP maxTimeApp;
                    maxTimeApp.desktopName = tabletAppList[i]->desktopName;
                    maxTimeApp.appName = tabletAppList[i]->appName;
                    maxTimeApp.appIcon = tabletAppList[i]->appIcon;
                    maxTimeApp.execCommand = tabletAppList[i]->execCommand;
                    maxTimeList.insert(j, maxTimeApp);
                    if(maxTimeList.size() > 4)
                    {
                        maxTimeList.pop_back();
                    }
                }
            }
        }
    }
    Q_EMIT valueChangedSingal(maxTimeList);
    return maxTimeList;
}
long int StudyCenterMode::getStudyTime(QString strMethod, QString appName)
{
    QDBusMessage request = QDBusMessage::createMethodCall("com.ukui.app.info","/com/ukui/app/info/time","com.ukui.app.info.time.interface", strMethod.toLocal8Bit().data());
    request<<appName;
    QDBusMessage response = QDBusConnection::sessionBus().call(request);
    long int iWeekmm = 0;
    if (response.type() == QDBusMessage::ReplyMessage)
    {
        iWeekmm = response.arguments().takeFirst().toInt();
        int ll = response.type();
        QString errMeg = response.errorMessage();
        QString errName = response.errorName();
        qDebug( "get study time sucess!, errtype:[%d], errmsg:[%s], errname:[%s],strMethod:[%s],appname:[%s]/n",ll, errMeg.toLocal8Bit().data(),errName.toLocal8Bit().data(),strMethod.toLocal8Bit().data(), appName.toLocal8Bit().data());

        qDebug("study time:%d/n", iWeekmm);
    }
    else
    {
        int ll = response.type();
        iWeekmm = response.arguments().takeFirst().toInt();
        QString errMeg = response.errorMessage();
        QString errName = response.errorName();
        qDebug( "get study time fail!, errtype:[%d], errmsg:[%s], errname:[%s],strMethod:[%s],appname:[%s]/n",ll, errMeg.toLocal8Bit().data(),errName.toLocal8Bit().data(),strMethod.toLocal8Bit().data(), appName.toLocal8Bit().data());
        qDebug("study time:%d/n", iWeekmm);
    }
    return iWeekmm;
}
QString  StudyCenterMode::getTime(QString strMethod, QStringList appList)
{
    qDebug("StudyCenterMode::getTime: strMethod[%s],size:[%d]/n", strMethod.toLocal8Bit().data(),appList.size());

    QDBusMessage request = QDBusMessage::createMethodCall("com.ukui.app.info","/com/ukui/app/info/time","com.ukui.app.info.time.interface", strMethod.toLocal8Bit().data());
    request<<appList;
    for(int i=0;i<appList.size();i++)
    {
        request<<appList[i];
        qDebug("StudyCenterMode::getTime trMethod[%s],appList[%d]:%s/n",strMethod.toLocal8Bit().data(),i, appList[i].toLocal8Bit().data());
    }

    QDBusMessage response = QDBusConnection::sessionBus().call(request);
    long int iWeekmm = 0;
    if (response.type() == QDBusMessage::ReplyMessage)
    {
        iWeekmm = response.arguments().takeFirst().toInt();
        int ll = response.type();
        QString errMeg = response.errorMessage();
        QString errName = response.errorName();
        qDebug( "get study time sucess!, errtype:[%d], errmsg:[%s], errname:[%s]/n",ll, errMeg.toLocal8Bit().data(),errName.toLocal8Bit().data());

        qDebug("study time:%d/n", iWeekmm);
    }
    else
    {
        int ll = response.type();
        iWeekmm = response.arguments().takeFirst().toInt();
        QString errMeg = response.errorMessage();
        QString errName = response.errorName();
        qDebug( "get study time fail!, errtype:[%d], errmsg:[%s], errname:[%s]/n",ll, errMeg.toLocal8Bit().data(),errName.toLocal8Bit().data());

    }
    int iHour = iWeekmm/60;
    int iMin = iWeekmm/60;
    if(iHour > 0)
    {
       return QString("%1小时%2分钟").arg(iHour).arg(iMin);
    }
    else
    {
        return QString("%1分钟").arg(iMin);
    }

}
void  StudyCenterMode::initTime()
{
    QString strTime ;
    QString strMethod ;
    strMethod = "GetDayUseTime";
    strTime = getTime(strMethod, m_appList);
    qDebug("StudyCenterMode::initTime :%s:%s/n", "GetDayUseTime",strTime.data());
    Q_EMIT timeChangedSingal(strMethod, strTime);

    strMethod = "GetWeekUseTime";
    strTime = getTime(strMethod, m_appList);
    qDebug("StudyCenterMode::initTime :%s:%s/n", "GetWeekUseTime",strTime.data());
    Q_EMIT timeChangedSingal(strMethod, strTime);

    strMethod = "GetMonthUseTime";
    strTime = getTime(strMethod, m_appList);
    qDebug("StudyCenterMode::initTime :%s:%s/n", "GetMonthUseTime",strTime.data());
    Q_EMIT timeChangedSingal(strMethod, strTime);
}
