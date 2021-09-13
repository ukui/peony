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
#include <QMouseEvent>
#include <QDebug>
#include <QDBusReply>
#include "../../tablet/src/Style/style.h"
using namespace Peony;


#define STUDY_CENTER_PRACTICE          "Practice"
#define STUDY_CENTER_STUGUARDS         "Student Guards"
#define STUDY_CENTER_SYN               "Synchronized"
#define STUDY_CENTER_STATISTICS        "Statistics"

StudyCenterMode::StudyCenterMode(QWidget *parent) : DesktopWidgetBase(parent)
{
    m_statusManagerDBus = new QDBusInterface("com.kylin.statusmanager.interface", "/" ,"com.kylin.statusmanager.interface",QDBusConnection::sessionBus(),this);
    this->m_exitAnimationType = AnimationType::RightToLeft;
    this->installEventFilter(this);
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
    updateTimeSlot();
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
    m_tableAppMangager = TabletAppManager::getInstance(this);

    QMap<QString, QList<TabletAppEntity*>> studyCenterDataMap = m_tableAppMangager->getStudyCenterData();
    QStringList strListTitleStyle;
    strListTitleStyle<<"精准练习"<<"color:#009ACD";
    QMap<QString, QList<TabletAppEntity*>> dataMap;

    dataMap.insert(tr("math"),studyCenterDataMap[STUDY_CENTER_MATH]);
    dataMap.insert(tr("english"),studyCenterDataMap[STUDY_CENTER_ENGLISH]);
    dataMap.insert(tr("chinese"),studyCenterDataMap[STUDY_CENTER_CHINESE]);
    dataMap.insert(tr("other"),studyCenterDataMap[STUDY_CENTER_OTHER]);
    practiceWidget = new StudyDirectoryWidget(strListTitleStyle,dataMap, 1);

    dataMap.clear();
    strListTitleStyle.clear();
    strListTitleStyle<<"守护中心"<<"color:#43CD80";
    dataMap.insert(STUDY_CENTER_STUDENT_GUARD,studyCenterDataMap[STUDY_CENTER_STUDENT_GUARD]);
    guradWidget = new StudyDirectoryWidget(strListTitleStyle,dataMap);

    dataMap.clear();
    strListTitleStyle.clear();
    strListTitleStyle<<"同步学习"<<"color:#FF8247";
    dataMap.insert(STUDY_CENTER_SYNCHRONIZED,studyCenterDataMap[STUDY_CENTER_SYNCHRONIZED]);
    synWidget = new StudyDirectoryWidget(strListTitleStyle,dataMap);

    QList<TABLETAPP> appList = getTimeOrder(studyCenterDataMap);
    widget4 = new StudyStatusWidget(appList,this);

    connect(this,SIGNAL(valueChangedSingal(QList<TABLETAPP> &applist)),widget4,SLOT(paintProgressSlot(QList<TABLETAPP> &applist)));
    connect(this,SIGNAL(timeChangedSingal(QString,QString)),widget4,SLOT(timeChangeSlot(QString,QString)));
    connect(widget4,SIGNAL(updateTimeSignal()), this,SLOT(updateTimeSlot()));
    connect(this,SIGNAL(markTimeSingal()),widget4,SLOT(markTimeSlot()));

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

    practiceWidget->installEventFilter(this);
    guradWidget->installEventFilter(this);
    synWidget->installEventFilter(this);
    widget4->installEventFilter(this);
    m_mainGridLayout = new QGridLayout(this);

    screenRotation();
    m_mainGridLayout->setMargin(80);
    m_mainGridLayout->setSpacing(20);
    this->setLayout(m_mainGridLayout);
    //分辨率变化，就重画屏幕
    connect(QApplication::desktop(), &QDesktopWidget::resized, this, [=]() {
//       sleep(500);
        centerToScreen(this);
        screenRotation();
    });

    //主屏变化
    connect(QApplication::desktop(), &QDesktopWidget::primaryScreenChanged, this, [=] {
        centerToScreen(this);
    });
    connect(QApplication::desktop(), &QDesktopWidget::screenCountChanged, this, [=] {
        centerToScreen(this);
    });

    if (m_statusManagerDBus) {
        if (m_statusManagerDBus->isValid()) {
            /**
             * 屏幕旋转
             * @brief normal,upside-down,left,right
             */
            connect(m_statusManagerDBus, SIGNAL(rotations_change_signal(QString)), this, SLOT(updateRotationsValue(QString)));
            //平板模式切换
            connect(m_statusManagerDBus, SIGNAL(mode_change_signal(bool)), this, SLOT(updateTabletModeValue(bool)));
        }
    }

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
    TABLETAPP maxTimeAppfirst;
    maxTimeAppfirst.desktopName = "";
    maxTimeAppfirst.appName = "";
    maxTimeAppfirst.appIcon = "";
    maxTimeAppfirst.execCommand = "";
    maxTimeAppfirst.iTime = -1;
    maxTimeList.push_back(maxTimeAppfirst);

    m_appList.clear();
    for(; ite != studyCenterDataMap.constEnd(); ++ite)
    {
        QList<TabletAppEntity*> tabletAppList = ite.value();
        for(int i = 0; i < tabletAppList.size(); ++i)
        {
            QString strDesktop = tabletAppList[i]->desktopName.left(tabletAppList[i]->desktopName.indexOf(".desktop"));
            m_appList.push_back(strDesktop);
            long int iWeekmm = getStudyTime("GetWeekCumulativeTime", strDesktop);
            qDebug("StudyCenterMode::getTimeOrder appName[%d] :%s,desktop:%s  /n", i, tabletAppList[i]->appName.toLocal8Bit().data(),tabletAppList[i]->desktopName.toLocal8Bit().data());
            qDebug("StudyCenterMode::getTimeOrder:%d,name:%s,path:%s  /n", iWeekmm,tabletAppList[i]->appName.toLocal8Bit().data(),tabletAppList[i]->appIcon.toLocal8Bit().data());
            for(int j = 0; j < maxTimeList.size(); ++j)
            {
                   qDebug("StudyCenterMode::getTimeOrder maxTimeList[%d] :%s,path:%s  /n", i, maxTimeList[i].appName.toLocal8Bit().data(),maxTimeList[i].appIcon.toLocal8Bit().data());
                if(iWeekmm >= maxTimeList[j].iTime)
                {
                    TABLETAPP maxTimeApp;
                    maxTimeApp.desktopName = tabletAppList[i]->desktopName;
                    maxTimeApp.appName = tabletAppList[i]->appName;
                    maxTimeApp.appIcon = tabletAppList[i]->appIcon;
                    maxTimeApp.execCommand = tabletAppList[i]->execCommand;
                    maxTimeApp.iTime = iWeekmm;
                    maxTimeList.insert(j, maxTimeApp);
                    break;
                }
//                else if(iWeekmm == maxTimeList[j].iTime)
//                {
//                    //QCollator
//                }
            }
        }
    }
    maxTimeList.pop_back();
    Q_EMIT valueChangedSingal(maxTimeList);
    return maxTimeList;
}
long int StudyCenterMode::getStudyTime(QString strMethod, QString appName)
{
    QDBusMessage request = QDBusMessage::createMethodCall("com.ukui.app.info","/com/ukui/app/info/time","com.ukui.app.info.time", strMethod.toLocal8Bit().data());
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
    //QList<QVariant> args;
    qDebug("StudyCenterMode::getTime: strMethod[%s],size:[%d]/n", strMethod.toLocal8Bit().data(),appList.size());
    QDBusMessage request = QDBusMessage::createMethodCall("com.ukui.app.info","/com/ukui/app/info/time","com.ukui.app.info.time", strMethod.toLocal8Bit().data());//com.ukui.app.info.time.interfaces
    for(int i=0;i<appList.size();i++)
    {
       // args.append ( QVariant ( appList[i]) );
        qDebug("StudyCenterMode::getTime trMethod[%s],appList[%d]:%s/n",strMethod.toLocal8Bit().data(),i, appList[i].toLocal8Bit().data());
    }
    //request.setArguments(args);
    request << appList;
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
    int iMin = iWeekmm%60;
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
    qDebug("StudyCenterMode::initTime :%s:%s/n", "GetDayUseTime",strTime.toLocal8Bit().data());
    Q_EMIT timeChangedSingal(strMethod, strTime);

    strMethod = "GetWeekUseTime";
    strTime = getTime(strMethod, m_appList);
    qDebug("StudyCenterMode::initTime :%s:%s/n", "GetWeekUseTime",strTime.toLocal8Bit().data());
    Q_EMIT timeChangedSingal(strMethod, strTime);

    strMethod = "GetMonthUseTime";
    strTime = getTime(strMethod, m_appList);
    qDebug("StudyCenterMode::initTime :%s:%s/n", "GetMonthUseTime",strTime.toLocal8Bit().data());
    Q_EMIT timeChangedSingal(strMethod, strTime);
}

void StudyCenterMode::updateTimeSlot()
{
    QMap<QString, QList<TabletAppEntity*>> studyCenterDataMap = m_tableAppMangager->getStudyCenterData();
    QList<TABLETAPP> appList = getTimeOrder(studyCenterDataMap);
    initTime();
    Q_EMIT markTimeSingal();
}

bool StudyCenterMode::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            qDebug() << "[StudyCenterMode::mousePressEvent] " << mouseEvent->globalPos();
            m_pressPoint = mouseEvent->globalPos();
            m_lastPressPoint = mouseEvent->globalPos();
            m_leftButtonPressed = true;
        }
    } else if (event->type() == QEvent::MouseButtonRelease) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            if (m_leftButtonPressed) {
                m_releasePoint = mouseEvent->globalPos();
                if (qAbs(m_releasePoint.y() - m_pressPoint.y()) < 50) {
                    qint32 moveLength = m_releasePoint.x() - m_pressPoint.x();
                    qDebug() << "[StudyCenterMode::mouseReleaseEvent] " << m_releasePoint << m_pressPoint << moveLength;
                    if (moveLength < -200) {
                        //下一页
                        QDBusReply<bool> message_a = m_statusManagerDBus->call("get_current_tabletmode");
                        if (message_a.isValid()) {
                            if ((bool) message_a.value()) {
                                Q_EMIT moveToOtherDesktop(DesktopType::Tablet, AnimationType::RightToLeft);
                            } else {
                                Q_EMIT moveToOtherDesktop(DesktopType::Desktop, AnimationType::RightToLeft);
                            }
                        } else {
                            Q_EMIT moveToOtherDesktop(DesktopType::Desktop, AnimationType::RightToLeft);
                        }
                    } else {
                        Q_EMIT desktopReboundRequest();
                    }
                    m_pressPoint = QPoint(-1, -1);
                    m_lastPressPoint = QPoint(-1, -1);
                    m_releasePoint = QPoint(-1, -1);

                    return true;
                }
                m_leftButtonPressed = false;
            }
        }
    }

//    else if (event->type() == QEvent::MouseMove) {
//        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
//    if (m_leftButtonPressed) {
//        qDebug() << "[StudyCenterMode::mouseMoveEvent] " << event->globalPos();
//
//        if (m_lastPressPoint.x() == -1) {
//            return;
//        }
//        QPoint currentPoint = event->globalPos();
//
//        if (qAbs(currentPoint.x() - m_pressPoint.x()) < 50) {
//            return;
//        }
//
//        qint32 length = currentPoint.x() - m_lastPressPoint.x();
//
//        if (length <= 0) {
//            Q_EMIT desktopMoveRequest(AnimationType::RightToLeft, qAbs(length), 0);
//        } else {
//            Q_EMIT desktopMoveRequest(AnimationType::LeftToRight, qAbs(length), 0);
//        }
//
//        m_lastPressPoint = currentPoint;
//    } else {
//        QWidget::mouseMoveEvent(event);
//    }
//    }
    return false;
}

void StudyCenterMode::updateTabletModeValue(bool mode)
{
    m_isTabletMode = mode;
}

void StudyCenterMode::updateRotationsValue(QString rotation)
{
    qDebug() << "StudyCenterMode::updateRotationsValue rotation:" << rotation <<"  m_isTabletMode:"<<m_isTabletMode << "  ScreenRotation:"<< Style::ScreenRotation;
    m_direction = rotation;
    screenRotation();
}
void StudyCenterMode::screenRotation()
{
    qDebug() << "StudyCenterMode::screenRotation   m_isTabletMode:"<<m_isTabletMode << "  ScreenRotation:"<< Style::ScreenRotation;

//    if (!m_isTabletMode)
//    {
//        return;
//    }
    //note 屏幕变化后，负责将app视图和小组件大小进行调整
    //1.隐藏各个组件
    if(m_mainGridLayout->count())
    {
        m_mainGridLayout->removeWidget(practiceWidget);
        m_mainGridLayout->removeWidget(guradWidget);
        m_mainGridLayout->removeWidget(synWidget);
        m_mainGridLayout->removeWidget(widget4);
    }

    //3.从新获取屏幕大小,设置当前组件几何
    QScreen *primaryScreen = QApplication::primaryScreen();

    //4.从新设置组件
    if ((m_direction == "left" || m_direction == "right") &&
               (primaryScreen->geometry().width() <
                primaryScreen->geometry().height()))
    {
         Style::ScreenRotation = true; //竖屏

    }
    else
    {
         Style::ScreenRotation = false; //横屏
    }
    qDebug() << "StudyCenterMode::screenRotation width:" << primaryScreen->geometry().width()  <<"  height:"<<primaryScreen->geometry().height() << "Style::ScreenRotation :"<<Style::ScreenRotation  ;

    this->setFixedSize(primaryScreen->geometry().width(),primaryScreen->geometry().height());
    qDebug() << "StudyCenterMode::screenRotation widget width:" << this->width()  <<"  height:"<<this->height();

    Style::initWidStyle();

    //5.显示全部组件
    if(Style::ScreenRotation)
    {
        m_mainGridLayout->addWidget(practiceWidget,0,0,4,2);
        m_mainGridLayout->addWidget(guradWidget,4,0,2,1);
        m_mainGridLayout->addWidget(synWidget,4,1,2,1);
        m_mainGridLayout->addWidget(widget4,6,0,3,2);
    }
    else
    {
        m_mainGridLayout->addWidget(practiceWidget,0,0,3,2);
        m_mainGridLayout->addWidget(guradWidget,0,2,1,1);
        m_mainGridLayout->addWidget(synWidget,0,3,1,1);
        m_mainGridLayout->addWidget(widget4,1,2,2,2);
    }

}


void StudyCenterMode::centerToScreen(QWidget *widget)
{
    qDebug() << "StudyCenterMode::centerToScreen" ;
    if (!widget)
        return;
    QDesktopWidget *m = QApplication::desktop();
    QRect desk_rect = m->screenGeometry(m->screenNumber(QCursor::pos()));
    int desk_x = desk_rect.width();
    int desk_y = desk_rect.height();
    int x = QApplication::primaryScreen()->geometry().width();
    int y = QApplication::primaryScreen()->geometry().height();
    widget->move(desk_x / 2 - x / 2 + desk_rect.left(), desk_y / 2 - y / 2 + desk_rect.top());
}