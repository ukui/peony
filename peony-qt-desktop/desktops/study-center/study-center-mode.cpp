#include "study-center-mode.h"
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
#include <QCollator>
#include <QButtonGroup>
#include <QTimer>
using namespace Peony;


#define STUDY_CENTER_PRACTICE          "Practice"
#define STUDY_CENTER_STUGUARDS         "Student Guards"
#define STUDY_CENTER_SYN               "Synchronized"
#define STUDY_CENTER_STATISTICS        "Statistics"

StudyCenterMode::StudyCenterMode(QWidget *parent) : DesktopWidgetBase(parent)
{
    qRegisterMetaType<QList<TABLETAPP>>("QList<TABLETAPP>");
    m_statusManagerDBus = new QDBusInterface("com.kylin.statusmanager.interface", "/" ,"com.kylin.statusmanager.interface",QDBusConnection::sessionBus(),this);
    this->m_exitAnimationType = AnimationType::RightToLeft;
    this->installEventFilter(this);

    connect(QApplication::primaryScreen(), &QScreen::geometryChanged, this, &StudyCenterMode::screenChange);
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
    updatePageButton();
    updateAppData();
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
    strListTitleStyle<<"精准练习"<<"#009ACD";
    strListTitleStyle<< "PRACTICE" << "rgba(0, 154, 205, 0.05)";

    QList<QPair<QString, QList<TabletAppEntity*>>> dataList;

    dataList.append(qMakePair(QString(tr("math")),studyCenterDataMap[STUDY_CENTER_MATH]));
    dataList.append(qMakePair(QString(tr("english")),studyCenterDataMap[STUDY_CENTER_ENGLISH]));
    dataList.append(qMakePair(QString(tr("chinese")),studyCenterDataMap[STUDY_CENTER_CHINESE]));
    dataList.append(qMakePair(QString(tr("other")),studyCenterDataMap[STUDY_CENTER_OTHER]));
    practiceWidget = new StudyDirectoryWidget(strListTitleStyle,dataList, 1,this);

    dataList.clear();
    strListTitleStyle.clear();
    strListTitleStyle<<"守护中心"<<"#43CD80";
    strListTitleStyle << "GUARDS" << "rgba(67, 205, 128, 0.05)";

    dataList.append(qMakePair(QString(STUDY_CENTER_STUDENT_GUARD),studyCenterDataMap[STUDY_CENTER_STUDENT_GUARD]));
    guradWidget = new SynAndDaemon(strListTitleStyle,dataList, this);

    dataList.clear();
    strListTitleStyle.clear();
    strListTitleStyle<<"同步学习"<<"#FF8247";
    strListTitleStyle << "COURSE" << "rgba(255, 130, 71, 0.05)";

    dataList.append(qMakePair(QString(STUDY_CENTER_SYNCHRONIZED),studyCenterDataMap[STUDY_CENTER_SYNCHRONIZED]));
    synWidget = new SynAndDaemon(strListTitleStyle,dataList, this);

    QList<TABLETAPP> appList = getTimeOrder(studyCenterDataMap);
    statusWidget = new StudyStatusWidget(appList,this);
    if(0 < appList.size())
    {
        Q_EMIT statusWidget->setMaximum(appList[0].iTime);
    }
    connect(this,SIGNAL(valueChangedSingal(QList<TABLETAPP>)),statusWidget,SLOT(paintProgressSlot(QList<TABLETAPP>)));
    connect(this,SIGNAL(timeChangedSingal(QString,QString)),statusWidget,SLOT(timeChangeSlot(QString,QString)));
    connect(statusWidget,SIGNAL(updateTimeSignal()), this,SLOT(updateTimeSlot()));
    connect(this,SIGNAL(markTimeSingal()),statusWidget,SLOT(markTimeSlot()));

    initTime();

    practiceWidget->installEventFilter(this);
    guradWidget->installEventFilter(this);
    synWidget->installEventFilter(this);
    statusWidget->installEventFilter(this);
    m_mainGridLayout = new QGridLayout(this);

    connect(m_tableAppMangager, &TabletAppManager::appEntityListChanged, this, &StudyCenterMode::updateAppData);

    changeTheme();
    screenRotation();

    m_mainGridLayout->setSpacing(16);
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
            QDBusReply<bool> message_a = m_statusManagerDBus->call("get_current_tabletmode");
            if (message_a.isValid()) {
                m_isTabletMode = message_a.value();
            }

            QDBusReply<QString> message_b = m_statusManagerDBus->call("get_current_rotation");
            if (message_b.isValid()) {
                m_direction = message_b.value();
            }
            /**
             * 屏幕旋转
             * @brief normal,upside-down,left,right
             */
            connect(m_statusManagerDBus, SIGNAL(rotations_change_signal(QString)), this, SLOT(updateRotationsValue(QString)));
            //平板模式切换
            connect(m_statusManagerDBus, SIGNAL(mode_change_signal(bool)), this, SLOT(updateTabletModeValue(bool)));
        }
    }

    initPageButton();
}

void StudyCenterMode::updateAppData()
{
    if (!practiceWidget || !guradWidget || !synWidget || !statusWidget) {
        return;
    }

    QMap<QString, QList<TabletAppEntity*>> studyCenterDataMap = m_tableAppMangager->getStudyCenterData();
    QList<QPair<QString, QList<TabletAppEntity*>>> dataList;

    dataList.append(qMakePair(QString(tr("math")),studyCenterDataMap[STUDY_CENTER_MATH]));
    dataList.append(qMakePair(QString(tr("english")),studyCenterDataMap[STUDY_CENTER_ENGLISH]));
    dataList.append(qMakePair(QString(tr("chinese")),studyCenterDataMap[STUDY_CENTER_CHINESE]));
    dataList.append(qMakePair(QString(tr("other")),studyCenterDataMap[STUDY_CENTER_OTHER]));
    practiceWidget->updateAppData(dataList);

    dataList.clear();

    dataList.append(qMakePair(QString(STUDY_CENTER_STUDENT_GUARD),studyCenterDataMap[STUDY_CENTER_STUDENT_GUARD]));
    guradWidget->updateAppData(dataList);

    dataList.clear();

    dataList.append(qMakePair(QString(STUDY_CENTER_SYNCHRONIZED),studyCenterDataMap[STUDY_CENTER_SYNCHRONIZED]));
    synWidget->updateAppData(dataList);

    dataList.clear();

    updateTimeSlot();
}

QList<TABLETAPP>  StudyCenterMode::getTimeOrder(QMap<QString, QList<TabletAppEntity*>> studyCenterDataMap )
{
    QMap<QString, QList<TabletAppEntity*>>::const_iterator ite = studyCenterDataMap.begin();
    QList<TABLETAPP> maxTimeList;
    m_appList.clear();
    for(; ite != studyCenterDataMap.constEnd(); ++ite)
    {
        QList<TabletAppEntity*> tabletAppList = ite.value();
        for(int i = 0; i < tabletAppList.size(); ++i)
        {
            QString strDesktop = tabletAppList[i]->desktopName.left(tabletAppList[i]->desktopName.indexOf(".desktop"));
            m_appList.push_back(strDesktop);
            long int iWeekmm = getStudyTime("GetWeekCumulativeTime", strDesktop);
            TABLETAPP maxTimeApp;
            maxTimeApp.desktopName = tabletAppList[i]->desktopName;
            maxTimeApp.appName = tabletAppList[i]->appName;
            maxTimeApp.appIcon = tabletAppList[i]->appIcon;
            maxTimeApp.execCommand = tabletAppList[i]->execCommand;
            maxTimeApp.iTime = iWeekmm;
            maxTimeList.insert(i, maxTimeApp);
            qDebug("StudyCenterMode::getTimeOrder appName[%d] :%s,desktop:%s  /n", i, tabletAppList[i]->appName.toLocal8Bit().data(),tabletAppList[i]->desktopName.toLocal8Bit().data());
            qDebug("StudyCenterMode::getTimeOrder:%d,name:%s,path:%s  /n", iWeekmm,tabletAppList[i]->appName.toLocal8Bit().data(),tabletAppList[i]->appIcon.toLocal8Bit().data());
        }
    }
    qSort(maxTimeList.begin(), maxTimeList.end(), [=](TABLETAPP p1, TABLETAPP p2){
        QLocale local(QLocale::Chinese);
        QCollator collator(local);
        int nResult = false;
        if((p1.iTime > p2.iTime) || (p1.iTime == p2.iTime && collator.compare(p1.appName, p2.appName) < 0))
        {
            nResult = true;
        }
        qDebug()<<"StudyCenterMode::qsort p1.appName:"<<p1.appName<<"  p2.appName:"<< p2.appName <<" p1.iTime:"<<p1.iTime <<" p1.iTime:" <<p2.iTime <<" result:"<<nResult;
        return nResult;
    });
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
    //暂停响应更新请求，节省资源
    if (isPause()) return;

    QMap<QString, QList<TabletAppEntity*>> studyCenterDataMap = m_tableAppMangager->getStudyCenterData();
    qDebug()<<"StudyCenterMode::updateTimeSlot begin,size:"<<studyCenterDataMap.size();
    QList<TABLETAPP> appList = getTimeOrder(studyCenterDataMap);
    if(0 < appList.size())
    {
        Q_EMIT statusWidget->setMaximum(appList[0].iTime);
    }
    Q_EMIT valueChangedSingal(appList);
    qDebug()<<"StudyCenterMode::valueChangedSingal end,size:"<<appList.size();
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
        //fix bug 83738
        if (!m_isTabletMode || isPause()) {
            return false;
        }
        if (mouseEvent->button() == Qt::LeftButton) {
            if (m_leftButtonPressed) {
                m_releasePoint = mouseEvent->globalPos();
                if (qAbs(m_releasePoint.y() - m_pressPoint.y()) < 300) {
                    qint32 moveLength = m_releasePoint.x() - m_pressPoint.x();
                    qDebug() << "[StudyCenterMode::mouseReleaseEvent] " << m_releasePoint << m_pressPoint << moveLength;
                    if (moveLength < -200) {
                        //下一页
                        requestMoveToOtherDesktop(DesktopType::Tablet, AnimationType::RightToLeft);
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

    updatePageButton();
}

void StudyCenterMode::updateRotationsValue(QString rotation)
{
    qDebug() << "StudyCenterMode::updateRotationsValue rotation:" << rotation <<"  m_isTabletMode:"<<m_isTabletMode << "  ScreenRotation:"<< Style::ScreenRotation;
    m_direction = rotation;
    //NOTE 2021-10-12 因为statusManager 的dbus信号到达时，屏幕可能会没有及时更改宽高为旋转后的状态，所以使用Qt的方式监听 见: screenChange()
//    m_pageButtonWidget->hide();
    screenRotation();

//    updatePageButton();
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
    for(int i=0;i < m_mainGridLayout->rowCount();i++ )
    {
         m_mainGridLayout->setRowStretch(i,0);
    }
    for(int i=0;i < m_mainGridLayout->columnCount();i++ )
    {
         m_mainGridLayout->setColumnStretch(i,0);
    }
    if(m_mainGridLayout->count())
    {
        m_mainGridLayout->removeWidget(practiceWidget);
        m_mainGridLayout->removeWidget(guradWidget);
        m_mainGridLayout->removeWidget(synWidget);
        m_mainGridLayout->removeWidget(statusWidget);
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
    practiceWidget->setSize();

    //5.显示全部组件
    if(Style::ScreenRotation)
    {
        m_mainGridLayout->addWidget(practiceWidget,0,0,4,2);
        m_mainGridLayout->addWidget(guradWidget,4,0,2,1);
        m_mainGridLayout->addWidget(synWidget,4,1,2,1);
        m_mainGridLayout->addWidget(statusWidget,6,0,3,2);
        m_mainGridLayout->setRowStretch(3,13);
        m_mainGridLayout->setRowStretch(4,5);
        m_mainGridLayout->setRowStretch(6,8);
        m_mainGridLayout->setColumnStretch(0,1);
        m_mainGridLayout->setColumnStretch(1,1);
        m_mainGridLayout->setContentsMargins(72,72,72,120);
    }
    else
    {
        m_mainGridLayout->addWidget(practiceWidget,0,0,3,2);
        m_mainGridLayout->addWidget(guradWidget,0,2,1,1);
        m_mainGridLayout->addWidget(synWidget,0,3,1,1);
        m_mainGridLayout->addWidget(statusWidget,1,2,2,2);

        m_mainGridLayout->setRowStretch(0,8);
        m_mainGridLayout->setRowStretch(1,13);
        m_mainGridLayout->setColumnStretch(0,2);
        m_mainGridLayout->setColumnStretch(2,1);
        m_mainGridLayout->setColumnStretch(3,1);
        m_mainGridLayout->setContentsMargins(80,86,80,138);
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

void StudyCenterMode::initPageButton()
{
    m_pageButtonWidget = new QWidget(this);
    m_buttonLayout     = new QHBoxLayout(m_pageButtonWidget);

    m_buttonLayout->setAlignment(Qt::AlignCenter);
    m_buttonLayout->setSpacing(10);

    m_pageButtonWidget->setLayout(m_buttonLayout);

    m_buttonGroup = new QButtonGroup(m_pageButtonWidget);
    connect(m_buttonGroup, QOverload<QAbstractButton *>::of(&QButtonGroup::buttonClicked), this,
            &StudyCenterMode::pageButtonClicked);

    updatePageButton();
}

void StudyCenterMode::updatePageButton()
{
    if (!m_isTabletMode) {
        m_pageButtonWidget->hide();
        return;
    };

    QScreen *screen = QApplication::primaryScreen();
    if (Style::ScreenRotation) {
        Style::appLine = 7;
        m_buttonLayout->setContentsMargins(0, 0, 0, 54);
    } else {
        Style::appLine = 4;
        m_buttonLayout->setContentsMargins(0, 0, 0, 56);
    }

    Style::appNum = TabletAppManager::getInstance()->getTabletAppEntityList().count();
    if (Style::appNum % (Style::appColumn * Style::appLine) == 0) {
        Style::appPage = Style::appNum / (Style::appColumn * Style::appLine);
    } else {
        Style::appPage = Style::appNum / (Style::appColumn * Style::appLine) + 1;
    }

    //0.更新切换页面按钮
    m_pageButtonWidget->setGeometry(QRect(0,
                                          screen->geometry().height() - Style::ButtonWidgetHeight,
                                          screen->geometry().width(),
                                          Style::ButtonWidgetHeight));

    for (auto button: m_buttonGroup->buttons()) {
        m_buttonGroup->removeButton(button);
        button->hide();
        delete button;
    }

    for (int page = 0; page <= Style::appPage; page++) {
        QPushButton *button = new QPushButton(m_pageButtonWidget);
        button->setFocusPolicy(Qt::NoFocus);
        button->setFixedSize(24, 24);

        if (page == 0) {
            //学习中心特别图标
            button->setStyleSheet("QPushButton{border-image:url(:/img/learning-center-selected.svg);}"
                                  "QPushButton:hover{border-image: url(:/img/learning-center-selected.svg);}"
                                  "QPushButton:pressed{border-image: url(:/img/learning-center-selected.svg);}");

        } else {
            button->setStyleSheet("QPushButton{border-image: url(:/img/default.svg);}"
                                  "QPushButton:hover{border-image: url(:/img/default.svg);}"
                                  "QPushButton:pressed{border-image:url(:/img/click.svg);}");
        }

        m_buttonGroup->addButton(button, page);
        m_buttonLayout->addWidget(button);
    }

    m_pageButtonWidget->show();
}

void StudyCenterMode::pageButtonClicked(QAbstractButton *button)
{
    int id = m_buttonGroup->id(button);

    if (id <= 0) return;
    m_pageButtonWidget->hide();

    requestMoveToOtherDesktop(DesktopType::Tablet, AnimationType::RightToLeft);
}
void StudyCenterMode::changeTheme()
{
    qDebug()<<"StudyCenterMode::changeTheme";
    if (QGSettings::isSchemaInstalled("org.ukui.style"))
    {
        QGSettings *settings = new QGSettings("org.ukui.style");

        connect(settings, &QGSettings::changed, this, [=](const QString &key)
        {
            if (key == "styleName")
            {
                QString strTheme = settings->get(key).toString();

                Q_EMIT practiceWidget->changeTheme(strTheme);
                Q_EMIT guradWidget->changeTheme(strTheme);
                Q_EMIT synWidget->changeTheme(strTheme);
                Q_EMIT statusWidget->changeTheme(strTheme);
            }

        });
        Q_EMIT practiceWidget->changeTheme(settings->get("styleName").toString());
        Q_EMIT guradWidget->changeTheme(settings->get("styleName").toString());
        Q_EMIT synWidget->changeTheme(settings->get("styleName").toString());
        Q_EMIT statusWidget->changeTheme(settings->get("styleName").toString());
    }
}

void StudyCenterMode::screenChange()
{
    m_pageButtonWidget->hide();
    screenRotation();
    updatePageButton();
}
