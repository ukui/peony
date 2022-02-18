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
#include <QPaintEvent>
#include "progress-widget.h"
#include "desktop-background-manager.h"
//#include "progress-item-delegate.h"

#include "../../tablet/data/tablet-app-manager.h"
//qt's global function
extern void qt_blurImage(QImage &blurImage, qreal radius, bool quality, int transposed);

StudyStatusWidget::StudyStatusWidget(QList<TABLETAPP> appList, QWidget *parent) :
    QWidget(parent)
{
    m_appList= appList;
    m_animation = new QVariantAnimation;
    m_animation->setDuration(500);
    m_animation->setStartValue(0.0);
    m_animation->setEndValue(360.0);
    connect(m_animation,&QVariantAnimation::valueChanged,[=](){
        m_updateIconBt->update();
        m_updateIconBt->setValue(m_animation->currentValue().toDouble());
    });
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
    this->setStyleSheet("background-color:rgba(255, 255, 255,0.85);border-radius:24px;");
     //widget->setWindowOpacity(0.9);

    //this->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

    m_titleLabel = new QLabel(this);
    m_titleLabel->setAttribute(Qt::WA_TranslucentBackground);
    //设置字号
    QFont ft;
    ft.setBold(true);
    m_titleLabel->setFont(ft);

    //设置颜色
    m_titleLabel->setStyleSheet("QLabel{background-color: transparent;color:#9370DB; font-size:32px;}");
    m_titleLabel->setText(tr("学情中心"));
    m_titleLabel->setAlignment(Qt::AlignLeft|Qt::AlignTop);

   // QWidget*  userInfoWidget = new QWidget(this);
    QHBoxLayout* userInfoLayout = new QHBoxLayout;

    m_userIconLabel = new QLabel(this);
    m_userNameLabel = new QLabel(this);

    m_userIconLabel->setAttribute(Qt::WA_TranslucentBackground);
    m_userNameLabel->setAttribute(Qt::WA_TranslucentBackground);

    m_userNameLabel->setFont(ft);
    m_userNameLabel->setStyleSheet("QLabel{color: palette(text);font-size:24px}");
    m_userIconLabel->setAlignment(Qt::AlignRight|Qt::AlignTop);
    m_userNameLabel->setAlignment(Qt::AlignRight|Qt::AlignTop);

    //设置学情中心背景字母
    QLabel* backGroundLabel = new QLabel(this);

    QFont bft;
    bft.setBold(true);
    bft.setWeight(QFont::Bold);
    backGroundLabel->setFont(bft);

    backGroundLabel->setStyleSheet("QLabel{background-color: transparent;color:rgba(147, 112, 219, 0.05);font-size:64px;}");
    backGroundLabel->setText(tr("STATISTICS"));
    backGroundLabel->setGeometry(45,0,500,110);
    backGroundLabel->lower();

    userInfoLayout->addWidget(m_titleLabel);
    userInfoLayout->addStretch();
    userInfoLayout->addWidget(m_userIconLabel);
    userInfoLayout->addSpacing(10);
    userInfoLayout->addWidget(m_userNameLabel);
    userInfoLayout->setContentsMargins(0, 0, 40, 0);
   // userInfoLayout->addSpacing(10);
   // userInfoLayout->setAlignment(Qt::AlignRight);
    //userInfoWidget->setLayout(userInfoLayout);

    m_scrollArea = new QScrollArea;
    QWidget* scrollWid = new QWidget;
    scrollWid->setStyleSheet("background:transparent;");
    m_scrollArea->setStyleSheet("background:transparent;");
    m_scrollArea->setWidget(scrollWid);
    m_scrollArea->horizontalScrollBar()->setVisible(false);
    m_scrollArea->verticalScrollBar()->setVisible(true);

    m_scrollArea->verticalScrollBar()->setStyleSheet("QScrollBar{width:12px;height:172px;padding-top:0px;padding-bottom:0px;border-radius:2px;border:2px solid transparent;}"
                                             "QScrollBar::handle{background-color:#B6BDC6; width:12px;height:172px;border-radius:2px;border:2px solid transparent;}"
                                             "QScrollBar::handle:hover{background-color:#CDD2D8;border-radius:2px;height:172px;border:none;}"
                                             "QScrollBar::handle:pressed{background-color:#9CA6B2;border-radius:2px;}"
                                             "QScrollBar::sub-line{background-color:transparent;height:0px;width:0px;}"
                                             "QScrollBar::add-line{background-color:transparent;height:0px;width:0px;}"
                                             );
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->adjustSize();

    QLabel* todayTitleLabel = new QLabel(this);
    //todayTitleLabel->setFixedSize();

    QLabel* weekTitleLabel = new QLabel(this);
    QLabel* monthTitleLabel = new QLabel(this);

    todayTitleLabel->setAttribute(Qt::WA_TranslucentBackground);
    weekTitleLabel->setAttribute(Qt::WA_TranslucentBackground);
    monthTitleLabel->setAttribute(Qt::WA_TranslucentBackground);

    todayTitleLabel->setText(tr("今日使用时长"));
    weekTitleLabel->setText(tr("本周使用时长"));
    monthTitleLabel->setText(tr("本月使用时长"));

    m_todayTimeLabel = new QLabel(this);
    m_weekTimeLabel = new QLabel(this);
    m_monthTimeLabel = new QLabel(this);

    m_todayTimeLabel->setAttribute(Qt::WA_TranslucentBackground);
    m_weekTimeLabel->setAttribute(Qt::WA_TranslucentBackground);
    m_monthTimeLabel->setAttribute(Qt::WA_TranslucentBackground);

    m_todayTimeLabel->setStyleSheet("QLabel{color: palette(text);font-size:20px}");
    m_weekTimeLabel->setStyleSheet("QLabel{color: palette(text);font-size:20px}");
    m_monthTimeLabel->setStyleSheet("QLabel{color: palette(text);font-size:20px}");

    QGridLayout* timeGridLayout = new QGridLayout;
    timeGridLayout->setSpacing(8);
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
    m_mainVboxLayout->addSpacing(45);

    QFrame* line=new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setFixedHeight(1);

    QVBoxLayout* ScrollVboxLayout = new QVBoxLayout;
    ScrollVboxLayout->addLayout(timeGridLayout);
    ScrollVboxLayout->addSpacing(27);
    ScrollVboxLayout->addWidget(line);
    ScrollVboxLayout->addSpacing(27);

    QLabel* timeTitleLabel = new QLabel;
    timeTitleLabel->setAttribute(Qt::WA_TranslucentBackground);
    timeTitleLabel->setText(tr("最常使用 (本周累计)"));

    ScrollVboxLayout->addWidget(timeTitleLabel);
    ScrollVboxLayout->addSpacing(10);

    m_progressGridLayout = new QGridLayout;
    for(int i = 0 ; i < m_appList.size(); ++i)
    {
        ProgressWidget* progress = new ProgressWidget(m_appList[i]);
        m_progressGridLayout->addWidget(progress,i/2 ,i%2);

        connect(this, SIGNAL(setMaximum(int)), progress, SLOT(setMaximum(int)));
        connect(this, &StudyStatusWidget::changeTheme, progress, &ProgressWidget::changeThemeSlot);
    }

    m_progressGridLayout->setVerticalSpacing(25);
    m_progressGridLayout->setHorizontalSpacing(48);
    m_progressGridLayout->setContentsMargins(0, 0, 32, 0);
    ScrollVboxLayout->addLayout(m_progressGridLayout);
    ScrollVboxLayout->setContentsMargins(0, 0, 0, 0);
    scrollWid->setLayout(ScrollVboxLayout);

    m_updateTimeBt = new QPushButton(this);
    //m_updateTimeBt->setFont(ft);

    connect(m_updateTimeBt,SIGNAL(clicked()), this,SIGNAL(updateTimeSignal()));
    QDateTime curTime = QDateTime::currentDateTime();//获取当前时间
    m_updateTimeBt->setText(QString("更新于：") + curTime.toString("MM.dd HH:mm"));
    //m_updateTimeBt->setIcon(QIcon("/home/user_23799a/yyw/yyw/refresh.svg"));
    m_updateTimeBt->setLayoutDirection(Qt::RightToLeft);
    m_updateTimeBt->setAttribute(Qt::WA_TranslucentBackground);
    //m_progressGridLayout->addWidget(m_updateTimeBt,2 ,1);

    m_updateIconBt = new StatusPushButton(this);
    m_updateIconBt->setStyleSheet("background:transparent;");

    connect(this, &StudyStatusWidget::changeTheme, m_updateIconBt, &StatusPushButton::changeTheme);

    connect(m_updateTimeBt,&QPushButton::clicked,[=](){
        m_animation->start();
    });

    connect(m_updateIconBt,&StatusPushButton::clicked,[=](){
        m_animation->start();
    });

    connect(m_updateIconBt,&StatusPushButton::clicked,this,&StudyStatusWidget::updateTimeSignal);

    m_mainVboxLayout->addWidget(m_scrollArea);

    QSpacerItem *space =new QSpacerItem(this->width()-20,8,QSizePolicy::Expanding,
    QSizePolicy::Ignored);

    QHBoxLayout* updateTimeLayout = new QHBoxLayout;
    updateTimeLayout->addItem(space);
    updateTimeLayout->addWidget(m_updateTimeBt);
    updateTimeLayout->addWidget(m_updateIconBt);
    updateTimeLayout->setContentsMargins(0, 0, 32, 0);

    m_mainVboxLayout->addSpacing(15);
    m_mainVboxLayout->addLayout(updateTimeLayout);

    m_mainVboxLayout->setContentsMargins(48, 33, 8, 16);

    this->setLayout(m_mainVboxLayout);

    connect(this, &StudyStatusWidget::changeTheme, [=](QString strTheme)
    {
        QString styleSheetDark = QString("QWidget{border-radius:24px;background-color:rgba(38, 38, 40,0.85)}"
                                         "QLabel,QListView,QPushButton,QScrollArea{background:transparent}");

        QString styleSheetLight = QString("QWidget{border-radius:24px;background:rgba(255, 255, 255,0.85)}"
                                          "QLabel,QListView,QPushButton,QScrollArea{background:transparent");

        if (strTheme == "ukui-dark")
        {
            //深色主题
           this->setStyleSheet(styleSheetDark);
           line->setStyleSheet("background-color:rgba(255, 255, 255, 0.1)");
           //设置字体颜色
           todayTitleLabel->setStyleSheet("QLabel{color:rgba(255,255,255,0.45);font-size:16px}");
           weekTitleLabel->setStyleSheet("QLabel{color:rgba(255,255,255,0.45);font-size:16px}");
           monthTitleLabel->setStyleSheet("QLabel{color:rgba(255,255,255,0.45);font-size:16px}");
           timeTitleLabel->setStyleSheet("QLabel{color:rgba(255,255,255,0.45);font-size:16px;background:transparent;}");
           m_updateTimeBt->setStyleSheet("QPushButton{color:rgba(255,255,255,0.45);font-size:16px;background:transparent;}");
           m_colorMask = QColor(38,38,40);
        }
        else
        {
            //浅色主题
           this->setStyleSheet(styleSheetLight);
           line->setStyleSheet("background-color:rgba(38, 38, 40, 0.1)");
           //设置字体颜色
           todayTitleLabel->setStyleSheet("QLabel{color:rgba(38,38,38,0.45);font-size:16px}");
           weekTitleLabel->setStyleSheet("QLabel{color:rgba(38,38,38,0.45);font-size:16px}");
           monthTitleLabel->setStyleSheet("QLabel{color:rgba(38,38,38,0.45);font-size:16px}");
           timeTitleLabel->setStyleSheet("QLabel{color:rgba(38,38,38,0.45);font-size:16px;background:transparent;}");
           m_updateTimeBt->setStyleSheet("QPushButton{color:rgba(38,38,38,0.45);font-size:16px;background:transparent;}");
           m_colorMask = QColor(255,255,255);
        }
    });
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
    QString userName = "Hi, " + m_userInterface->property("RealName").value<QString>() + "同学";
    QString userIconPath = m_userInterface->property("IconFile").value<QString>();
    m_userInterface->connection().connect("org.freedesktop.Accounts",userpath, "org.freedesktop.DBus.Properties", "PropertiesChanged",
                                    this, SLOT(accountSlots(QString, QMap<QString, QVariant>, QStringList)));

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
            connect(this, SIGNAL(setMaximum(int)), progressWid, SLOT(setMaximum(int)));
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
void StudyStatusWidget::accountSlots(QString property, QMap<QString, QVariant> propertyMap, QStringList propertyList)
{
    Q_UNUSED(property);
    Q_UNUSED(propertyList);

    if (propertyMap.keys().contains("IconFile"))
    {
        m_userIconLabel->setPixmap(PixmapToRound(propertyMap.value("IconFile").toString(), 20));
    }
    if (propertyMap.keys().contains("RealName"))
    {
        QString userName = "Hi, " + m_userInterface->property("RealName").value<QString>() + "同学";
        m_userNameLabel->setText(userName);
    }
}
void StudyStatusWidget::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.save();
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    auto manager = DesktopBackgroundManager::globalInstance();
    QSize size = this->size();
    QPoint point = this->pos();
    QRect widgetRect = this->rect();

    //把需要模糊的图片区域放大，再模糊处理
    QImage img = manager->getBlurImage();
    //img = img.copy(point.x(),point.y(),size.width(),size.height());
    QRect source(point.x(),point.y(),size.width(),size.height());

    QPainterPath roundPath;
    roundPath.addRoundedRect(widgetRect,24,24);
    painter.setClipPath(roundPath);
    //painter.drawImage(0,0,img);
    painter.drawImage(widgetRect,img,source);

    //auto colorMask = QColor(255,255,255);
    m_colorMask.setAlphaF(0.85);
    painter.fillRect(widgetRect, m_colorMask);
    painter.restore();

    painter.save();
    painter.setRenderHint(QPainter::Antialiasing,true);
    //QPainterPath画圆角矩形
    const qreal radius = 8;
    QPainterPath path;
    QRect rect(0,m_titleLabel->pos().y()+7,8,32);
    path.moveTo(rect.topRight() - QPointF(radius, 0));
    path.lineTo(rect.topLeft());
    path.lineTo(rect.bottomLeft());
    path.lineTo(rect.bottomRight() - QPointF(radius, 0));
    path.quadTo(rect.bottomRight(), rect.bottomRight() + QPointF(0, -radius));
    path.lineTo(rect.topRight() + QPointF(0, radius));
    path.quadTo(rect.topRight(), rect.topRight() + QPointF(-radius, -0));

    painter.setPen(QPen(Qt::NoPen));
    QColor color("#8869D5");
    painter.setBrush(QBrush(color));

    painter.drawPath(path);

    painter.restore();
}
