#include "tablet-mode.h"

#include <QDebug>
#include <QMouseEvent>
#include <QPropertyAnimation>
#include <QVBoxLayout>
#include <QPushButton>

#include <QDebug>
#include <QDesktopWidget>
#include <QScreen>
#include <QRect>
#include <QDebug>
#include <QDateTime>
#include <QIcon>
#include <QSvgRenderer>
#include <qgsettings.h>
#include <iostream>
#include "src/Style/style.h"
#include "src/UtilityFunction/timewidget.h"

#define TABLED_SCHEMA "org.ukui.SettingsDaemon.plugins.tablet-mode"
#define TABLET_MODE                  "tablet-mode"

#define TABLED_ROTATION "org.ukui.SettingsDaemon.plugins.xrandr"

#include <QCoreApplication>
#include <QDBusConnection>
#include <QtDBus>
#include <QSettings>
#include <QDir>
/**/
#include <QtX11Extras/QX11Info>
#include "src/XEventMonitor/xeventmonitor.h"
#include <QKeyEvent>
#include <QPropertyAnimation>

using namespace Peony;

QT_BEGIN_NAMESPACE
extern void
qt_blurImage(QPainter *p, QImage &blurImage, qreal radius, bool quality, bool alphaOnly, int transposed = 0);

QT_END_NAMESPACE

TabletMode::TabletMode(QWidget *parent) : DesktopWidgetBase(parent)
{

//    qDebug()<<"1、构造";
    //this->resize(QApplication::primaryScreen()->geometry().width(),QApplication::primaryScreen()->geometry().height());
    this->setAttribute(Qt::WA_TranslucentBackground, false);
    this->setAutoFillBackground(false);
    this->setFocusPolicy(Qt::StrongFocus);
    this->setProperty("useStyleWindowManager", false);

    //获取数据
    UkuiMenuInterface m_ukuiMenuInterface;
    UkuiMenuInterface::appInfoVector = m_ukuiMenuInterface.createAppInfoVector();
    UkuiMenuInterface::allAppVector = m_ukuiMenuInterface.getAllApp();

    //加入左右侧界面,不使用布局
    m_width = QApplication::primaryScreen()->geometry().width() + 3;
    m_height = QApplication::primaryScreen()->geometry().height() + 3;

    //左侧时间控件
    leftWidget = new TimeWidget(this);
    toolBox = new ToolBox(this, 104, m_height);
    toolBox->setGeometry(0, 0, 104, m_height);
    toolBox->hide();
    //最右侧翻页button
    buttonWidget = new QWidget(this);
    buttonGroup = new QButtonGroup;
    vbox = new QVBoxLayout;
    vbox->setAlignment(Qt::AlignHCenter);
    vbox->setSpacing(0);
    buttonWidget->setLayout(vbox);
    vbox->setContentsMargins(0, 0, 0, 0);

    //特效模式,此处Gsetting不明确，需进一步确认
    bg_effect = new QGSettings(QString("org.ukui.control-center.personalise").toLocal8Bit());
    setOpacityEffect(bg_effect->get("transparency").toReal());

    connect(bg_effect, &QGSettings::changed, [this](const QString &key) {
        if (key == "effect") {
            if (bg_effect->get("effect").toBool()) {
                setOpacityEffect(bg_effect->get("transparency").toReal());
            } else {
                setOpacityEffect(bg_effect->get("transparency").toReal());
            }
        }
    });

    //平板切换
    rotation = new QGSettings(TABLED_ROTATION);
    tabletMode = new QGSettings(TABLED_SCHEMA);
    connect(tabletMode, &QGSettings::changed, [this](const QString &key) {

        qDebug() << "tabletMode key" << key;
        if (key == "tabletMode") {
            autoRotation = tabletMode->get("auto-rotation").toBool(); //监测旋转按钮是否开启
            direction = "normal";
            rotation->set("xrandr-rotations", direction);
            m_isTabletMode = tabletMode->get("tablet-mode").toBool();
            hideOrShowMenu(m_isTabletMode);
        }
    });

    //屏幕旋转
    Style::IsWideScreen = false;
    direction = rotation->get("xrandr-rt-rotations").toString();
    //TODO 左侧时间组件的显示，进行修改 0812
    if (direction == "left" || direction == "right") {
        Style::ScreenRotation = true;
        Style::initWidStyle();
        leftWidget->setGeometry(QRect(0, 0, m_width, 300));
        m_CommonUseWidget = new FullCommonUseWidget(this, m_width, m_height - 300);
        m_CommonUseWidget->setGeometry(QRect(0,
                                             leftWidget->y() + leftWidget->height(),
                                             m_width - Style::ButtonWidgetWidth,
                                             m_height - leftWidget->height()));
        //最右侧翻页button
        buttonWidget->setGeometry(QRect(m_CommonUseWidget->x() + m_CommonUseWidget->width(),
                                        0,
                                        Style::ButtonWidgetWidth,
                                        m_height));
        m_CommonUseWidget->fillAppList();
        leftWidget->show();
        m_CommonUseWidget->show();
        leftWidget->focusPlug->hide();
        buttonWidgetShow();
    } else {
        Style::ScreenRotation = false;
        Style::initWidStyle();
//        toolBox->hide();
        leftWidget->setGeometry(QRect(0, 0, 512, m_height));

        m_CommonUseWidget = new FullCommonUseWidget(this, m_width - leftWidget->width() - Style::ButtonWidgetWidth,
                                                    m_height);
        m_CommonUseWidget->setGeometry(QRect(leftWidget->x() + leftWidget->width(),
                                             0,
                                             m_width - leftWidget->width() - Style::ButtonWidgetWidth,
                                             m_height));

        //最右侧翻页button
        buttonWidget->setGeometry(QRect(m_CommonUseWidget->x() + m_CommonUseWidget->width(),
                                        0,
                                        Style::ButtonWidgetWidth,
                                        m_height));
        m_CommonUseWidget->fillAppList();
        leftWidget->show();
        m_CommonUseWidget->show();
        leftWidget->focusPlug->show();
        buttonWidgetShow();
    }

    //pc下鼠标功能
    XEventMonitor::instance()->start();
    connect(XEventMonitor::instance(), SIGNAL(keyRelease(QString)),
            this, SLOT(XkbEventsRelease(QString)));
    connect(XEventMonitor::instance(), SIGNAL(keyPress(QString)),
            this, SLOT(XkbEventsPress(QString)));
    //监控.desktop文件目录
    //监控安装应用与卸载应用列表

    bool isfile = appListFile();//判断监控看、路径是否存在
    m_fileWatcher = new QFileSystemWatcher;

    m_fileWatcher->addPaths(QStringList() << QString("/usr/share/applications")
                                          << QString(QDir::homePath() + "/.local/share/applications/"));
    connect(m_fileWatcher, &QFileSystemWatcher::directoryChanged, this, &TabletMode::directoryChangedSlot);


    m_fileWatcher1 = new QFileSystemWatcher;
    bool ismonitor = m_fileWatcher1->addPath(QDir::homePath() + "/.config/ukui/desktop_applist");
    //m_fileWatcher1->addPaths(QStringList()<<QDir::homePath()+"/.config/ukui/desktop_applist");
    connect(m_fileWatcher1, &QFileSystemWatcher::fileChanged, this, &TabletMode::directoryChangedSlot);
    if (isfile) {
        QString filepath = QDir::homePath() + "/.config/ukui/ukui-menu.ini";
        QSettings *filetsetting = new QSettings(filepath, QSettings::IniFormat);
        filetsetting->beginGroup("ukui-menu-sysapplist");
        filetsetting->setValue("desktop_applist", 1);
        filetsetting->sync();
        filetsetting->endGroup();
        if (ismonitor) {
            filetsetting->beginGroup("ukui-menu-sysapplist");
            filetsetting->setValue("desktop_applist", 0);
            filetsetting->sync();
            filetsetting->endGroup();
        }
        filetsetting->beginGroup("application");
        filetsetting->setValue("kylin-user-guide.desktop", 1000);
        filetsetting->sync();
        filetsetting->endGroup();
    }

    m_directoryChangedThread = new DirectoryChangedThread;
    connect(this, &TabletMode::sendDirectoryPath, m_directoryChangedThread, &DirectoryChangedThread::recvDirectoryPath);
    connect(m_directoryChangedThread, &DirectoryChangedThread::requestUpdateSignal, this,
            &TabletMode::requestUpdateSlot);
    connect(m_directoryChangedThread, &DirectoryChangedThread::deleteAppSignal, this,
            &TabletMode::requestDeleteAppSlot);

    QDBusConnection::systemBus().connect(QString(), QString("/com/ukui/desktop/software"),
                                         "com.ukui.desktop.software", "send_to_client",
                                         this, SLOT(client_get(QString)));

    connect(this, &TabletMode::UpdateSignal, [&]() {
        m_CommonUseWidget->updateListViewSlot();
    });


    //右侧按钮翻页
    connect(buttonGroup, QOverload<QAbstractButton *>::of(&QButtonGroup::buttonClicked), this, &TabletMode::buttonClicked);
    connect(m_CommonUseWidget, &FullCommonUseWidget::pagenumchanged, this, &TabletMode::pageNumberChanged);
    connect(m_CommonUseWidget, &FullCommonUseWidget::drawButtonWidgetAgain, this, &TabletMode::buttonWidgetShow);

    //页面收纳
    connect(m_CommonUseWidget, &FullCommonUseWidget::pageCollapse, this, &TabletMode::collapse);
    connect(m_CommonUseWidget, &FullCommonUseWidget::pageSpread, this, &TabletMode::spread);
    connect(toolBox, &ToolBox::pageSpread, this, &TabletMode::spread);

    /*界面隐藏信号*/
    connect(m_CommonUseWidget, &FullCommonUseWidget::sendHideMainWindowSignal, this,
            &TabletMode::recvHideMainWindowSlot);

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
    QDBusConnection::sessionBus().unregisterService("org.ukui.menu.tablet");
    QDBusConnection::sessionBus().registerService("org.ukui.menu.tablet");
    QDBusConnection::sessionBus().registerObject("/tablet/menu", this, QDBusConnection::ExportAllSlots);

    //初始化收纳状态，切换时更新
    if (tabletMode->get(TABLET_MODE).toBool() && Style::ScreenRotation == false) {
        spread();
    } else if (!tabletMode->get(TABLET_MODE).toBool()) {
        collapse();
    }
    //TODO 将左侧组件设置为一直展开 0812
    //不论何时都展开，
    spread();
    //TODO 删除翻页动画，将页面改为两个不同的组件 0812
    //翻页动画
    m_animation = new QVariantAnimation(m_CommonUseWidget);
    m_animation->setEasingCurve(QEasingCurve::Linear);
    connect(m_animation, &QVariantAnimation::valueChanged, this, [=](const QVariant variant) {
        int value = variant.toInt();
        m_CommonUseWidget->m_listView->verticalScrollBar()->setValue(value);
        if (value < m_CommonUseWidget->m_listView->verticalScrollBar()->minimum() ||
            value > m_CommonUseWidget->m_listView->verticalScrollBar()->maximum())
            m_animation->stop();
    });
    //绘画管理器打开后，win键功能释放
    if (QGSettings::isSchemaInstalled(QString("org.ukui.session").toLocal8Bit())) {
        QGSettings *gsetting = new QGSettings(QString("org.ukui.session").toLocal8Bit());
        connect(gsetting, &QGSettings::changed, this, &TabletMode::winKeyReleaseSlot);
    }

    if (checkapplist()) {
        //qDebug()<<"==================";
        directoryChangedSlot();//初始加载时更新应用列表，检查有没有在ukui-menu没启时安装或卸载应用
    }

}

TabletMode::~TabletMode()
{
    XEventMonitor::instance()->quit();
    if (leftWidget)
        delete leftWidget;
    if (toolBox)
        delete toolBox;
    if (buttonWidget)
        delete buttonWidget;
    if (buttonGroup)
        delete buttonGroup;
    if (vbox)
        delete vbox;
    if (tabletMode)
        delete tabletMode;
    if (rotation)
        delete rotation;
    if (m_CommonUseWidget)
        delete m_CommonUseWidget;
    if (m_fileWatcher)
        delete m_fileWatcher;
    if (m_fileWatcher1)
        delete m_fileWatcher1;
    if (m_directoryChangedThread)
        delete m_directoryChangedThread;
    if (m_animation)
        delete m_animation;

    leftWidget = nullptr;
    toolBox = nullptr;
    buttonWidget = nullptr;
    buttonGroup = nullptr;
    vbox = nullptr;
    tabletMode = nullptr;
    rotation = nullptr;
    m_CommonUseWidget = nullptr;
    m_fileWatcher = nullptr;
    m_fileWatcher1 = nullptr;
    m_directoryChangedThread = nullptr;
    m_animation = nullptr;
}

void TabletMode::setActivated(bool activated)
{
    DesktopWidgetBase::setActivated(activated);
    this->setHidden(!activated);
}

DesktopWidgetBase *TabletMode::initDesktop(const QRect &rect)
{
    return DesktopWidgetBase::initDesktop(rect);
}

void TabletMode::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_leftKeyPressed = true;
        m_startPoint = event->globalPos();
        m_lastEndPoint = m_startPoint;
        qDebug() << "鼠标按下事件：" << m_startPoint << event->pos() << geometry();
    }
    QWidget::mousePressEvent(event);
}

void TabletMode::mouseReleaseEvent(QMouseEvent *event)
{
    if (m_animation->state() == QVariantAnimation::Running)
        return;

    if (event->button() == Qt::LeftButton) {
        m_leftKeyPressed = false;
        m_endPoint = event->globalPos();
        qint64 moveWidth = m_endPoint.x() - m_startPoint.x();
        quint64 minWidth = geometry().width() * 0.3;
        qDebug() << "鼠标抬起事件：" << m_endPoint << "滑动距离：" << moveWidth << "minWidth:" << minWidth << "geometry:"
                 << geometry();

        if (moveWidth > minWidth) {
            m_exitAnimationType = AnimationType::OpacityLess;
            Q_EMIT moveToOtherDesktop(DesktopType::Desktop, AnimationType::OpacityFull);
        } else if (moveWidth >= 0) {
            QPropertyAnimation *returnAnimation = new QPropertyAnimation(this, "pos");

            returnAnimation->setStartValue(QPoint(geometry().x(), geometry().y()));
            returnAnimation->setEndValue(QPoint(0, 0));
            returnAnimation->setDuration(300);

            returnAnimation->start();

        } else {
            //上划的处理
        }
    }
    QWidget::mouseReleaseEvent(event);
}

void TabletMode::mouseMoveEvent(QMouseEvent *event)
{
    //判断鼠标左键是否按下
    if (m_leftKeyPressed) {
        QPoint currentPoint = event->globalPos();
        //当前减去开始点，如果moveLength大于0那么是向下拉
        qint64 moveLength = currentPoint.x() - m_lastEndPoint.x();
        qint64 currentX = moveLength + geometry().x();
        qDebug() << "移动的长度：" << moveLength << "新的Y值:" << currentX;
        if (currentX > 0) {
            setGeometry(currentX, 0, geometry().width(), geometry().height());
        } else {
            setGeometry(0, 0, geometry().width(), geometry().height());
        }
        qDebug() << "===鼠标移动后 geometry：" << geometry();
        m_lastEndPoint = currentPoint;
    }
    QWidget::mouseMoveEvent(event);
}

void TabletMode::unSetEffect(std::string str)
{
    setOpacityEffect(1);
}

void TabletMode::setEffect(std::string str)
{
    setOpacityEffect(0.7);
}

void TabletMode::client_get(QString str)
{
    Q_EMIT UpdateSignal();
}

//改变搜索框及工具栏透明度
void TabletMode::setOpacityEffect(const qreal &num)
{
    leftWidget->setDownOpacityEffect(num); //全局搜索框透明度
    toolBox->setToolOpacityEffect(num);//工具栏透明度
}

//打开应用，开始菜单隐藏，背景改为黑色纯色；关闭应用，开始菜单显示，背景为壁纸
void TabletMode::desktopSwitch(int res)
{
    if (appRun == res)
        return;
    else if (res == 1) {
        appRun = res;
        hideBackground = false;
        screenRotation();
    } else if (res == 0) {
        appRun = res;
        hideBackground = true;
        screenRotation();
    }
}

void TabletMode::hideOrShowMenu(bool res)
{
    //qDebug()<<"2、隐藏或显示";
    if (res) {
        this->setAttribute(Qt::WA_TranslucentBackground, false);
        this->setAttribute(Qt::WA_X11NetWmWindowTypeDesktop, true);
        this->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
//        direction=rotation->get("xrandr-rotations").toString();
        //    Style::IsWideScreen = false;
        if ((direction == "left" || direction == "right") &&
            (QApplication::primaryScreen()->geometry().width() < QApplication::primaryScreen()->geometry().height()))
            screenVertical();
        else
            screenHorizontal();
        this->show();
        this->raise();
        this->activateWindow();
    } else {
        this->setAttribute(Qt::WA_TranslucentBackground, true);
        this->setAttribute(Qt::WA_X11NetWmWindowTypeDesktop, false);
        this->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint | Qt::X11BypassWindowManagerHint);
        this->hide();
//        this->raise();
//        this->activateWindow();
    }
}

//打开PC模式下的开始菜单
void TabletMode::showPCMenu()
{
//    if(!Style::IsWideScreen)
//    {
//        Style::IsWideScreen=true;
    screenHorizontal();
//    }
    this->show();
    this->raise();
    this->activateWindow();
}

void TabletMode::directoryChangedSlot()
{
    Q_EMIT sendDirectoryPath(QString("/usr/share/applications"));
    m_directoryChangedThread->start();
}

void TabletMode::requestUpdateSlot(QString desktopfp)
{
    m_directoryChangedThread->quit();
    m_CommonUseWidget->updateListView(desktopfp);
    connect(m_fileWatcher1, &QFileSystemWatcher::fileChanged, this, &TabletMode::directoryChangedSlot);
}

void TabletMode::requestDeleteAppSlot()
{
    m_directoryChangedThread->quit();
    m_CommonUseWidget->updateListViewSlot();
    connect(m_fileWatcher1, &QFileSystemWatcher::fileChanged, this, &TabletMode::directoryChangedSlot);
}

void TabletMode::screenRotation()
{
    //qDebug()<<"3、屏幕旋转";
    leftWidget->hide();
    m_CommonUseWidget->hide();
    buttonWidget->hide();
    toolBox->hide();
    m_width = QApplication::primaryScreen()->geometry().width() + 3;
    m_height = QApplication::primaryScreen()->geometry().height() + 3;
    int x = QApplication::primaryScreen()->geometry().x();
    int y = QApplication::primaryScreen()->geometry().y();
    this->setGeometry(x - 1, y - 1,
                      QApplication::primaryScreen()->geometry().width() + 3,
                      QApplication::primaryScreen()->geometry().height() + 3);

    direction = rotation->get("xrandr-rotations").toString();
    if (hideBackground) {
        return;
    } else if ((direction == "left" || direction == "right") &&
               (QApplication::primaryScreen()->geometry().width() <
                QApplication::primaryScreen()->geometry().height())) {
        screenVertical();
    } else {
        screenHorizontal();
    }

}

void TabletMode::screenVertical()
{
    Style::ScreenRotation = true;
//    Style::IsWideScreen=false;
    Style::nowpagenum = 1;
    Style::initWidStyle();
    toolBox->hide();
    leftWidget->setGeometry(QRect(0,
                                  0,
                                  m_width,
                                  300));

    m_CommonUseWidget->setGeometry(QRect(0,
                                         leftWidget->y() + leftWidget->height(),
                                         m_width - Style::ButtonWidgetWidth,
                                         m_height - leftWidget->height()));
    m_CommonUseWidget->repaintWid(1);
    buttonWidget->setGeometry(QRect(m_CommonUseWidget->x() + m_CommonUseWidget->width(),
                                    0,
                                    Style::ButtonWidgetWidth,
                                    m_height));
    leftWidget->show();
    leftWidget->focusPlug->hide();
    m_CommonUseWidget->fillAppList();
    m_CommonUseWidget->show();
    buttonWidgetShow();
    buttonWidget->show();

}

void TabletMode::screenHorizontal()
{
    Style::ScreenRotation = false;
    Style::nowpagenum = 1;
    Style::initWidStyle();
    if (!Style::IsWideScreen) {
        toolBox->hide();
        leftWidget->setGeometry(QRect(0,
                                      0,
                                      512,
                                      m_height));
        m_CommonUseWidget->setGeometry(QRect(leftWidget->x() + leftWidget->width(),
                                             0,
                                             m_width - leftWidget->width() - Style::ButtonWidgetWidth,
                                             m_height));

        m_CommonUseWidget->repaintWid(0);
        leftWidget->show();
        leftWidget->focusPlug->show();

    } else if (Style::IsWideScreen) {
        leftWidget->hide();
        m_CommonUseWidget->setGeometry(QRect(toolBox->x() + toolBox->width(),
                                             0,
                                             m_width - toolBox->width() - toolBox->x() - Style::ButtonWidgetWidth,
                                             m_height));
        m_CommonUseWidget->repaintWid(0);
        leftWidget->hide();
        leftWidget->focusPlug->hide();
        toolBox->show();

    }
    m_CommonUseWidget->fillAppList();
    m_CommonUseWidget->show();
    buttonWidget->setGeometry(QRect(m_CommonUseWidget->x() + m_CommonUseWidget->width(),
                                    0,
                                    Style::ButtonWidgetWidth,
                                    m_height));
    buttonWidgetShow();
    buttonWidget->show();

}

void TabletMode::buttonWidgetShow()
{
    //qDebug()<<"4、buttonWidgetShow";
    delete vbox;
    vbox = new QVBoxLayout;
    buttonWidget->setLayout(vbox);
    vbox->setAlignment(Qt::AlignHCenter);
    vbox->setSpacing(16);
    vbox->setContentsMargins(0, 0, 0, 0);

    for (auto button : buttonGroup->buttons()) {
        buttonGroup->removeButton(button);
        button->deleteLater();
    }
    m_isTabletMode = tabletMode->get("tablet-mode").toBool();
    for (int page = 1; page <= Style::appPage; page++) {

        button = new QPushButton;
        button->setFocusPolicy(Qt::NoFocus);
        button->setFixedSize(24, 24);
        if (m_isTabletMode)//平板模式禁用悬停图标
        {
            button->setStyleSheet("QPushButton{border-image: url(:/img/default.svg);}"
                                  "QPushButton:hover{border-image: url(:/img/default.svg);}"
                                  "QPushButton:pressed{border-image:url(:/img/click.svg);}");
        } else {
            button->setStyleSheet("QPushButton{border-image: url(:/img/default.svg);}"
                                  "QPushButton:hover{border-image: url(:/img/hover.svg);}"
                                  "QPushButton:pressed{border-image:url(:/img/click.svg);}");
        }

        if (page == 1) {
            button->setStyleSheet("QPushButton{border-image:url(:/img/click.svg);}"
                                  "QPushButton:hover{border-image: url(:/img/click.svg);}"
                                  "QPushButton:pressed{border-image: url(:/img/click.svg);}");
        }
        vbox->addWidget(button);
        buttonGroup->addButton(button, page);
    }
    connect(buttonGroup, QOverload<QAbstractButton *>::of(&QButtonGroup::buttonClicked), this,
            &TabletMode::buttonClicked);
}

void TabletMode::buttonClicked(QAbstractButton *button)
{
    if (m_animation->state() == QVariantAnimation::Running)
        return;
    int idd = buttonGroup->id(button);
    Style::nowpagenum = idd;
    m_isTabletMode = tabletMode->get("tablet-mode").toBool();
    for (int page = 1; page <= Style::appPage; page++) {
        if (idd == page) {
            buttonGroup->button(page)->setStyleSheet("QPushButton{border-image:url(:/img/click.svg);}"
                                                     "QPushButton:hover{border-image: url(:/img/click.svg);}"
                                                     "QPushButton:pressed{border-image: url(:/img/click.svg);}");
            int idd1 = buttonGroup->id(buttonGroup->button(page));
        } else {
            if (m_isTabletMode) {
                buttonGroup->button(page)->setStyleSheet("QPushButton{border-image:url(:/img/default.svg);}"
                                                         "QPushButton:hover{border-image: url(:/img/default.svg);}"
                                                         "QPushButton:pressed{border-image: url(:/img/click.svg);}");
            } else {
                buttonGroup->button(page)->setStyleSheet("QPushButton{border-image:url(:/img/default.svg);}"
                                                         "QPushButton:hover{border-image: url(:/img/hover.svg);}"
                                                         "QPushButton:pressed{border-image: url(:/img/click.svg);}");
            }

            int idd2 = buttonGroup->id(buttonGroup->button(page));
        }
    }
    m_animation->stop();
    int dis = Style::appLine * (Style::AppListItemSizeHeight);
    m_animation->setStartValue(m_CommonUseWidget->m_listView->verticalScrollBar()->value());
    m_animation->setEndValue((idd - 1) * dis);
    m_animation->setDuration(350);
    m_animation->start();

}

void TabletMode::pageNumberChanged()
{
    m_isTabletMode = tabletMode->get("tablet-mode").toBool();
    if (Style::appPage != 1) {
        for (int page = 1; page <= Style::appPage; page++) {
            if (Style::nowpagenum == page) {
                buttonGroup->button(page)->setStyleSheet("QPushButton{border-image:url(:/img/click.svg);}"
                                                         "QPushButton:hover{border-image: url(:/img/click.svg);}"
                                                         "QPushButton:pressed{border-image: url(:/img/click.svg);}");
            } else {
                if (!m_isTabletMode) {
                    buttonGroup->button(page)->setStyleSheet("QPushButton{border-image:url(:/img/default.svg);}"
                                                             "QPushButton:hover{border-image: url(:/img/hover.svg);}"
                                                             "QPushButton:pressed{border-image: url(:/img/click.svg);}");
                } else //平板模式禁用悬停图标
                {
                    buttonGroup->button(page)->setStyleSheet("QPushButton{border-image:url(:/img/default.svg);}"
                                                             "QPushButton:hover{border-image: url(:/img/default.svg);}"
                                                             "QPushButton:pressed{border-image: url(:/img/click.svg);}");
                }

            }
        }
    }
}

/*pc 功能*/
bool TabletMode::event(QEvent *event)
{
    if (event->type() == QEvent::ActivationChange) {
        if (QApplication::activeWindow() != this) {
            if (!tabletMode->get(TABLET_MODE).toBool()) {
                //平板模式 下禁止win隐藏菜单
                this->hide();
            }
        }
    }

    return QWidget::event(event);
}

void TabletMode::XkbEventsPress(const QString &keycode)
{
    QString KeyName;
    if (keycode.length() >= 8) {
        KeyName = keycode.left(8);
    }
    if (KeyName.compare("Super_L+") == 0) {
        m_winFlag = true;
    }
    if (m_winFlag && keycode == "Super_L") {
        m_winFlag = false;
        return;
    }
}

void TabletMode::XkbEventsRelease(const QString &keycode)
{
    QString KeyName;
    static bool winFlag = false;
    if (keycode.length() >= 8) {
        KeyName = keycode.left(8);
    }
    if (KeyName.compare("Super_L+") == 0) {
        winFlag = true;
    }
    if (winFlag && keycode == "Super_L") {
        winFlag = false;
        return;
    } else if (m_winFlag && keycode == "Super_L")
        return;

    if (tabletMode && tabletMode->get(TABLET_MODE).toBool()) {
        qWarning() << QTime::currentTime()
                   << " Now is tablet mode, and it is forbidden to hide or show the menu after 'win'.'Esc'";
        return;
    }

    /**以下代码是非平板模式需要处理的键盘按键**/
    if ((keycode == "Super_L") || (keycode == "Super_R")) {
        qDebug() << "(ActiveWindow, SelfWindow):(" << QApplication::activeWindow() << ", " << this << ")";
        if (QApplication::activeWindow() == this) {
            if (m_CommonUseWidget->m_listView->isDraging()) {
                qWarning() << "Icon is been draging";
            }
            this->hide();
        } else {
            this->showPCMenu();
        }
    }

    if (keycode == "Escape") {
        this->hide();
    }
}

void TabletMode::winKeyReleaseSlot(const QString &key)
{
    if (key == "winKeyRelease" || key == "win-key-release") {
        QGSettings gsetting(QString("org.ukui.session").toLocal8Bit());
        if (gsetting.get(QString("win-key-release")).toBool()) {
            disconnect(XEventMonitor::instance(), SIGNAL(keyRelease(QString)),
                       this, SLOT(XkbEventsRelease(QString)));

            disconnect(XEventMonitor::instance(), SIGNAL(keyPress(QString)),
                       this, SLOT(XkbEventsPress(QString)));

        } else {
            connect(XEventMonitor::instance(), SIGNAL(keyRelease(QString)),
                    this, SLOT(XkbEventsRelease(QString)));

            connect(XEventMonitor::instance(), SIGNAL(keyPress(QString)),
                    this, SLOT(XkbEventsPress(QString)));
        }
    }
}

/**
 * 隐藏窗口
 */
void TabletMode::recvHideMainWindowSlot()
{
    if (!tabletMode->get(TABLET_MODE).toBool()) {
        //平板模式 下禁止win隐藏菜单
        this->hide();
    }
}

void TabletMode::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);
}

QImage applyEffectToImage(QImage src, QGraphicsEffect *effect, int extent = 0)
{
    //qDebug()<<"6、绘制特效";
    if (src.isNull()) return QImage();   //No need to do anything else!
    if (!effect) return src;             //No need to do anything else!
    QGraphicsScene scene;
    QGraphicsPixmapItem item;
    item.setPixmap(QPixmap::fromImage(src));
    item.setGraphicsEffect(effect);
    scene.addItem(&item);
    QImage res(src.size() + QSize(-extent * 2, -extent * 2), QImage::Format_ARGB32);
    res.fill(Qt::transparent);//transparent
    QPainter ptr(&res);
    scene.render(&ptr, QRectF(), QRectF(-extent, -extent, src.width() + extent * 2, src.height() + extent * 2));
    return res;
}

//收纳模式
void TabletMode::collapse()
{
    //不在支持收起
    return;
}

void TabletMode::spread()
{
    Style::IsWideScreen = false;
    Style::AppListItemSizeWidth = 216;
    Style::AppListItemSizeHeight = 270;
    Style::AppListViewLeftMargin = 52;
    Style::AppLeftSpace = 60;

    if (toolBox->type == 1) {
        leftWidget->focusPlug->button_1_click();
        toolBox->type = -1;

    } else if (toolBox->type == 2) {
        leftWidget->focusPlug->button_2_click();
        toolBox->type = -1;
    }

    leftWidget->show();
    leftWidget->focusPlug->show();
    m_CommonUseWidget->show();
    toolBox->hide();

    leftWidget->setGeometry(QRect(0, 0, 512, m_height));

    m_CommonUseWidget->setGeometry(QRect(leftWidget->x() + leftWidget->width(),
                                         0,
                                         m_width - leftWidget->width() - leftWidget->x() - Style::ButtonWidgetWidth,
                                         m_height));

    m_CommonUseWidget->m_listView->setFixedSize(
            m_CommonUseWidget->width() - Style::AppListViewLeftMargin - Style::AppListViewRightMargin,
            m_CommonUseWidget->height());
    m_CommonUseWidget->m_listView->setGridSize(QSize(Style::AppListItemSizeWidth, Style::AppListItemSizeHeight));
    m_CommonUseWidget->m_listView->update(this->rect());
    m_CommonUseWidget->show();
    m_CommonUseWidget->m_listView->verticalScrollBar()->setValue(
            (Style::nowpagenum - 1) * Style::appLine * Style::AppListItemSizeHeight);

}

//接收快捷键，屏幕收纳功能
void TabletMode::keyPressEvent(QKeyEvent *event)
{
    if (hideBackground) {
        return;

    } else if ((event->key() == Qt::Key_Left) && ((event->modifiers() & Qt::AltModifier) != 0) &&
               Style::ScreenRotation == false) {
        collapse();

    } else if ((event->key() == Qt::Key_Right) && ((event->modifiers() & Qt::AltModifier) != 0) &&
               Style::ScreenRotation == false) {
        spread();

    }
    QWidget::keyPressEvent(event);
}


bool TabletMode::appListFile()
{
//    qDebug()<<"应用列表文件==============================是否文件存在";
    QFile fp;//要包含必要的头文件，这里省略
    QString path = QDir::homePath() + "/.config/ukui/desktop_applist";
    fp.setFileName(path);                      //为fp指定包含路径的文件名
    if (fp.exists()) {
        //若存在，读取
        //QString(text);
        qDebug() << "文件存在";
/*        fp.open(QIODevice::ReadOnly); */                  //打开 和 关闭 要紧密相关
        return 1;
    } else {
        //若不存在，则通过open操作新建文件
//        qDebug()<<"文件不存在";
        fp.open(QIODevice::ReadWrite | QIODevice::Text);  //不存在的情况下，打开包含了新建文件的操作
        //fp.write("I am writing file");
//        qDebug()<<"正在写文件";
        fp.close();
        return 1;
    }
    return 0;

}

void TabletMode::centerToScreen(QWidget *widget)
{
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

bool TabletMode::checkapplist()
{
    qDebug() << "TabletMode   checkapplist";
    QString path = QDir::homePath() + "/.config/ukui/ukui-menu.ini";
    QSettings *setting = new QSettings(path, QSettings::IniFormat);
    setting->beginGroup("application");
    QStringList keyList = setting->allKeys();

    setting->sync();
    setting->endGroup();
    delete setting;
    if (keyList.count() == UkuiMenuInterface::desktopfpVector.count()) {
        return false;
    } else {
        UkuiMenuInterface::desktopfpVector.clear();
        for (int i = 0; i < keyList.count(); i++) {
            QString tmp = QString("%1%2").arg("/usr/share/applications/").arg(keyList.at(i));
            UkuiMenuInterface::desktopfpVector.append(tmp);
        }

        return true;
    }

}

void TabletMode::recvStartMenuSlot()
{
    if(this->isVisible())//wgx
    {
        if (!tabletMode->get(TABLET_MODE).toBool())//平板模式 下禁止win隐藏菜单
        {

            this->hide();
        }
    }
    else{
        if (!tabletMode->get(TABLET_MODE).toBool())//平板模式 下禁止win隐藏菜单
        {
            this->showPCMenu();
//            this->raise();
//            this->activateWindow();
        }

    }
}
