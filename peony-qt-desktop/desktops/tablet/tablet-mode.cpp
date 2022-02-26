
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
 * Modified By: Wenfei He <hewenfei@kylinos.cn>
 *
 */

#include "tablet-mode.h"
#include "src/Style/style.h"
#include "desktop-global-settings.h"

#include <QMouseEvent>
#include <QPropertyAnimation>
#include <QVBoxLayout>
#include <QPushButton>
#include <QDebug>
#include <QDesktopWidget>
#include <QRect>
#include <QDateTime>
#include <QIcon>
#include <qgsettings.h>
#include <iostream>
#include <QDBusConnection>
#include <QtDBus>
#include <QSettings>
#include <QDir>
#include <QKeyEvent>

using namespace Peony;

QT_BEGIN_NAMESPACE
extern void
qt_blurImage(QPainter *p, QImage &blurImage, qreal radius, bool quality, bool alphaOnly, int transposed = 0);

QT_END_NAMESPACE

TabletMode::TabletMode(QWidget *parent) : DesktopWidgetBase(parent)
{
    this->m_exitAnimationType = AnimationType::LeftToRight;

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
    this->initAllWidget();

//    this->initRightButton();

    this->initGSettings();

    this->updateByDirection();

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
        m_appViewContainer->updateListViewSlot();
    });


    //右侧按钮翻页
    connect(m_buttonGroup, QOverload<QAbstractButton *>::of(&QButtonGroup::buttonClicked), this, &TabletMode::buttonClicked);
    connect(m_appViewContainer, &FullCommonUseWidget::pagenumchanged, this, &TabletMode::pageNumberChanged);
    connect(m_appViewContainer, &FullCommonUseWidget::drawButtonWidgetAgain, this, &TabletMode::updatePageButton);
    connect(m_appViewContainer, &FullCommonUseWidget::moveRequest, this, &TabletMode::moveWindow);

    //页面收纳
//    connect(m_appViewContainer, &FullCommonUseWidget::pageCollapse, this, &TabletMode::collapse);
//    connect(m_appViewContainer, &FullCommonUseWidget::pageSpread, this, &TabletMode::spread);
//    connect(toolBox, &ToolBox::pageSpread, this, &TabletMode::spread);

    /*界面隐藏信号*/
    connect(m_appViewContainer, &FullCommonUseWidget::sendHideMainWindowSignal, this,
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

    //TODO 删除翻页动画，将页面改为两个不同的组件 0812

    if (checkAppList()) {
        directoryChangedSlot();//初始加载时更新应用列表，检查有没有在ukui-menu没启时安装或卸载应用
    }

}

void TabletMode::setActivated(bool activated)
{
    DesktopWidgetBase::setActivated(activated);
    if (!activated) {
        //将内部容器放回原点
        m_container->move(0, 0);
    }
}

void TabletMode::beforeInitDesktop()
{
    updateGSettings();
    if (m_isTabletMode) {
        //在显示桌面前从新布局界面
        updateMainLayout();
        //刷新各个页面的数据，以及从新排列页面
        m_appViewContainer->updateListViewSlot();
    }
}

QPixmap TabletMode::generatePixmap()
{
    return DesktopWidgetBase::generatePixmap();
}

void TabletMode::showDesktop()
{
    DesktopWidgetBase::showDesktop();
    //将在beforeInitDesktop函数中被hide的组件显示出来
    this->showAllWidgets();
}

DesktopWidgetBase *TabletMode::initDesktop(const QRect &rect)
{
//    //继续上一次屏幕变化的更改
    screenRotation();
    return DesktopWidgetBase::initDesktop(rect);
}

void TabletMode::unSetEffect(std::string str)
{
    setToolsOpacityEffect(1);
}

void TabletMode::setEffect(std::string str)
{
    setToolsOpacityEffect(0.7);
}

void TabletMode::client_get(QString str)
{
    Q_EMIT UpdateSignal();
}

//改变搜索框及工具栏透明度
void TabletMode::setToolsOpacityEffect(const qreal &num)
{
    m_pluginBoxWidget->setSearchOpacityEffect(num); //全局搜索框透明度
}

void TabletMode::directoryChangedSlot()
{
    Q_EMIT sendDirectoryPath(QString("/usr/share/applications"));
    m_directoryChangedThread->start();
}

void TabletMode::requestUpdateSlot(QString desktopfp)
{
    m_directoryChangedThread->quit();
    m_appViewContainer->updateListView(desktopfp);
    connect(m_fileWatcher1, &QFileSystemWatcher::fileChanged, this, &TabletMode::directoryChangedSlot);
}

void TabletMode::requestDeleteAppSlot()
{
    m_directoryChangedThread->quit();
    m_appViewContainer->updateListViewSlot();
    connect(m_fileWatcher1, &QFileSystemWatcher::fileChanged, this, &TabletMode::directoryChangedSlot);
}

void TabletMode::screenRotation()
{
    updateGSettings();
    if (!m_isTabletMode) {
        return;
    }
    //note 屏幕变化后，负责将app视图和小组件大小进行调整
    //1.隐藏各个组件
    m_container->hide();
    m_pluginBoxWidget->hide();
//    m_pluginBoxWidget->hidePluginWidget(true);
    m_appViewContainer->hide();
    m_pageButtonWidget->hide();
    //2.屏幕变化后回到第一页
    Style::nowpagenum = 1;

    //3.从新获取屏幕大小,设置当前组件几何
    QScreen *primaryScreen = QApplication::primaryScreen();
    m_width = primaryScreen->geometry().width() + 3;
    m_height = primaryScreen->geometry().height() + 3;

    this->setGeometry(primaryScreen->geometry().x() - 1,
                      primaryScreen->geometry().y() - 1,
                      primaryScreen->geometry().width() + 3,
                      primaryScreen->geometry().height() + 3);

    m_container->setGeometry(0, 0, m_width, m_height - Style::ButtonWidgetHeight);
    //4.从新设置组件
    if ((m_direction == "left" || m_direction == "right") &&
               (primaryScreen->geometry().width() <
                primaryScreen->geometry().height())) {
        screenVertical();
    } else {
        screenHorizontal();
    }

    Style::initWidStyle();
    m_pluginBoxWidget->updateMainLayout();

    //5.显示全部组件
    this->showAllWidgets();
}

void TabletMode::updateMainLayout()
{
    //1.隐藏各个组件
    m_container->hide();
    m_pluginBoxWidget->hide();
    m_appViewContainer->hide();
    m_pageButtonWidget->hide();
    //2.回到第一页
    Style::nowpagenum = 1;

    //3.从新获取屏幕大小
    QScreen *primaryScreen = QApplication::primaryScreen();
    m_width = primaryScreen->geometry().width() + 3;
    m_height = primaryScreen->geometry().height() + 3;

    m_container->setGeometry(0, 0, m_width, m_height - Style::ButtonWidgetHeight);
    //4.从新设置组件属性
    if ((m_direction == "left" || m_direction == "right")
        && (primaryScreen->geometry().width() < primaryScreen->geometry().height())) {
        screenVertical();

    } else {
        screenHorizontal();
    }

    Style::initWidStyle();
    m_pluginBoxWidget->updateMainLayout();
}

void TabletMode::showAllWidgets()
{
    m_container->show();
    m_pluginBoxWidget->show();

    if (Style::ScreenRotation) {
        //屏幕垂直
        m_pluginBoxWidget->hidePluginWidget(true);

        m_appViewContainer->repaintWid(1);
    } else {
        //屏幕水平
        m_pluginBoxWidget->hidePluginWidget(false);

        m_appViewContainer->repaintWid(0);
    }

    m_appViewContainer->fillAppList();
    m_appViewContainer->updatePageData();
    m_appViewContainer->show();

    this->updatePageButton();
    m_pageButtonWidget->show();
}

void TabletMode::screenVertical()
{
    Style::ScreenRotation = true;

    m_pluginBoxWidget->setGeometry(QRect(0, 0, m_width, 300));

    m_appViewContainer->setGeometry(QRect(0,
                                         m_pluginBoxWidget->y() + m_pluginBoxWidget->height(),
                                         m_width,
                                         m_height - m_pluginBoxWidget->height() - Style::ButtonWidgetHeight));
}

void TabletMode::screenHorizontal()
{
    Style::ScreenRotation = false;

    m_pluginBoxWidget->setGeometry(QRect(0, 0, 512, m_height - Style::ButtonWidgetHeight));

    m_appViewContainer->setGeometry(QRect(m_pluginBoxWidget->x() + m_pluginBoxWidget->width(),
                                         0,
                                         m_width - m_pluginBoxWidget->width(),
                                         m_height - Style::ButtonWidgetHeight));
}

void TabletMode::updatePageButton()
{
    //0.更新切换页面按钮
    m_pageButtonWidget->setGeometry(QRect(0,
                                          m_height - Style::ButtonWidgetHeight,
                                          m_width,
                                          Style::ButtonWidgetHeight));

    for (auto button : m_buttonGroup->buttons()) {
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
            button->setStyleSheet("QPushButton{border-image:url(:/img/learning-center-select.svg);}"
                                  "QPushButton:hover{border-image: url(:/img/learning-center-select.svg);}"
                                  "QPushButton:pressed{border-image: url(:/img/learning-center-selected.svg);}");

        } else if (page == 1) {
            button->setStyleSheet("QPushButton{border-image:url(:/img/click.svg);}"
                                  "QPushButton:hover{border-image: url(:/img/click.svg);}"
                                  "QPushButton:pressed{border-image: url(:/img/click.svg);}");

        } else {
            button->setStyleSheet("QPushButton{border-image: url(:/img/default.svg);}"
                                  "QPushButton:hover{border-image: url(:/img/default.svg);}"
                                  "QPushButton:pressed{border-image:url(:/img/click.svg);}");
        }

        m_buttonGroup->addButton(button, page);
        m_buttonLayout->addWidget(button);
    }
    connect(m_buttonGroup, QOverload<QAbstractButton *>::of(&QButtonGroup::buttonClicked), this,
            &TabletMode::buttonClicked);
}

void TabletMode::buttonClicked(QAbstractButton *button)
{
    int id = m_buttonGroup->id(button);
    if (id <= 0) {
        this->updatePageButtonStatus(0);
        //goto study center
        requestMoveToOtherDesktop(DesktopType::StudyCenter, AnimationType::LeftToRight);
        return;
    }

    if (id > Style::appPage || id == Style::nowpagenum) {
        return;
    }

    this->pageNumberChanged(id - Style::nowpagenum);
}

void TabletMode::updatePageButtonStatus(qint32 page)
{
    for (int i = 0; i < m_buttonGroup->buttons().count(); i++) {
        if (i == page) {
            if (i == 0) {
                m_buttonGroup->button(i)->setStyleSheet("QPushButton{border-image:url(:/img/learning-center-selected.svg);}"
                                                           "QPushButton:hover{border-image: url(:/img/learning-center-selected.svg);}"
                                                           "QPushButton:pressed{border-image: url(:/img/learning-center-selected.svg);}");
            } else {
                m_buttonGroup->button(i)->setStyleSheet("QPushButton{border-image:url(:/img/click.svg);}"
                                                           "QPushButton:hover{border-image: url(:/img/click.svg);}"
                                                           "QPushButton:pressed{border-image: url(:/img/click.svg);}");
            }
        } else {
            if (i == 0) {
                m_buttonGroup->button(i)->setStyleSheet("QPushButton{border-image:url(:/img/learning-center-select.svg);}"
                                                        "QPushButton:hover{border-image: url(:/img/learning-center-select.svg);}"
                                                        "QPushButton:pressed{border-image: url(:/img/learning-center-selected.svg);}");
            } else {
                m_buttonGroup->button(i)->setStyleSheet("QPushButton{border-image:url(:/img/default.svg);}"
                                                        "QPushButton:hover{border-image: url(:/img/default.svg);}"
                                                        "QPushButton:pressed{border-image: url(:/img/click.svg);}");
            }
        }
    }
}

void TabletMode::pageNumberChanged(qint32 signal, bool hide)
{
    //fix bug #82786
    if (!m_isActivated | isPause()) return;

    if (m_exitAnimation && m_showAnimation) {
        if ((m_exitAnimation->state() != QPropertyAnimation::Stopped)
            || (m_showAnimation->state() != QPropertyAnimation::Stopped))
            return;
    }
    if (signal == 0) {
        returnRawPoint();
        return;
    }

    qint32 lastPage = Style::nowpagenum;
    Style::nowpagenum += signal;

    if (Style::nowpagenum < 1) {
        Style::nowpagenum = 1;
        this->updatePageButtonStatus(0);
        requestMoveToOtherDesktop(DesktopType::StudyCenter, AnimationType::LeftToRight);
        return;
    }

    if (Style::nowpagenum > Style::appPage) {
        Style::nowpagenum = Style::appPage;
    }

    if (lastPage == Style::nowpagenum) {
        returnRawPoint();
        return;
    }

    if (Style::appPage != 1) {
        m_pageButtonWidget->setDisabled(true);
        this->changePage(signal, hide);
        this->updatePageButtonStatus(Style::nowpagenum);
    }
}

/**
 * 隐藏窗口
 */
void TabletMode::recvHideMainWindowSlot()
{
}

void TabletMode::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);
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

bool TabletMode::checkAppList()
{
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

void TabletMode::initGSettings()
{
    //搜索框的特效,默认为不透明
    setToolsOpacityEffect(1);
    if (QGSettings::isSchemaInstalled(QString("org.ukui.control-center.personalise").toLocal8Bit())) {
        bg_effect = new QGSettings(QString("org.ukui.control-center.personalise").toLocal8Bit());
        setToolsOpacityEffect(bg_effect->get("transparency").toReal());

        connect(bg_effect, &QGSettings::changed, [this](const QString &key) {
            if (key == "effect") {
                if (bg_effect->get("effect").toBool()) {
                    setToolsOpacityEffect(bg_effect->get("transparency").toReal());
                }
            }
        });
    }

//    if (QGSettings::isSchemaInstalled(TABLED_ROTATION_SCHEMA)) {
//        m_rotationGSettings = new QGSettings(TABLED_ROTATION_SCHEMA);
//        m_direction = m_rotationGSettings->get("xrandrRotations").toString();
//
//        connect(m_rotationGSettings, &QGSettings::changed, [this](const QString &key) {
//            if (key == "xrandrRotations") {
//                m_direction = m_rotationGSettings->get(key).toString();
//            }
//        });
//    }
//
//    if (QGSettings::isSchemaInstalled(TABLET_SCHEMA)) {
//        m_tabletModeGSettings = new QGSettings(TABLET_SCHEMA);
//        m_isTabletMode = m_tabletModeGSettings->get(TABLET_MODE).toBool();
//
//        connect(m_tabletModeGSettings, &QGSettings::changed, [this](const QString &key) {
//            if (key == TABLET_MODE) {
//                m_isTabletMode = m_tabletModeGSettings->get(key).toBool();
//                m_autoRotation = m_tabletModeGSettings->get("autoRotation").toBool(); //监测旋转按钮是否开启
//                m_direction = "normal";
//
//                if (m_rotationGSettings) {
//                    m_rotationGSettings->set("xrandrRotations", m_direction);
//                }
//            }
//        });
//    }

    //dbus
    m_statusManagerDBus = new QDBusInterface(DBUS_STATUS_MANAGER_IF, "/" ,DBUS_STATUS_MANAGER_IF,QDBusConnection::sessionBus(),this);
    qDebug() << "[TabletMode::initGSettings] init statusManagerDBus:" << m_statusManagerDBus->isValid();
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
}

void TabletMode::updateGSettings()
{
//    if (m_rotationGSettings) {
//        m_direction = m_rotationGSettings->get("xrandrRotations").toString();
//    }
//
//    if (m_tabletModeGSettings) {
//        m_isTabletMode = m_tabletModeGSettings->get(TABLET_MODE).toBool();
//    }

    if (m_statusManagerDBus) {
        QDBusReply<bool> message_a = m_statusManagerDBus->call("get_current_tabletmode");
        if (message_a.isValid()) {
            m_isTabletMode = message_a.value();
        }

        QDBusReply<QString> message_b = m_statusManagerDBus->call("get_current_rotation");
        if (message_b.isValid()) {
            m_direction = message_b.value();
        }
    }
}
void TabletMode::initAllWidget()
{
    m_container = new QWidget(this);
    m_container->setAttribute(Qt::WA_TranslucentBackground);
    m_container->setGeometry(0, 0, m_width, m_height - Style::ButtonWidgetHeight);
    m_pluginBoxWidget = new TabletPluginWidget(m_container);
    m_appViewContainer = new FullCommonUseWidget(m_container, m_width, m_height);
    initRightButton();
}

void TabletMode::initRightButton()
{
    //最右侧翻页button
    m_pageButtonWidget = new QWidget(this);

    m_buttonLayout = new QHBoxLayout(m_pageButtonWidget);
    m_buttonLayout->setAlignment(Qt::AlignCenter);
    m_buttonLayout->setSpacing(10);
    m_buttonLayout->setContentsMargins(0, 0, 0, 56);

    m_pageButtonWidget->setLayout(m_buttonLayout);

    m_buttonGroup = new QButtonGroup(m_pageButtonWidget);
}

void TabletMode::updateByDirection()
{
    this->screenRotation();
}

TabletMode::~TabletMode()
{
    if (m_pluginBoxWidget)
        delete m_pluginBoxWidget;
    if (toolBox)
        delete toolBox;
    if (m_pageButtonWidget)
        delete m_pageButtonWidget;
    if (m_buttonGroup)
        delete m_buttonGroup;
    if (m_buttonLayout)
        delete m_buttonLayout;
    if (m_tabletModeGSettings)
        delete m_tabletModeGSettings;
    if (m_rotationGSettings)
        delete m_rotationGSettings;
    if (m_appViewContainer)
        delete m_appViewContainer;
    if (m_fileWatcher)
        delete m_fileWatcher;
    if (m_fileWatcher1)
        delete m_fileWatcher1;
    if (m_directoryChangedThread)
        delete m_directoryChangedThread;
    if (m_exitAnimation)
        delete m_exitAnimation;

    m_pluginBoxWidget = nullptr;
    toolBox = nullptr;
    m_pageButtonWidget = nullptr;
    m_buttonGroup = nullptr;
    m_buttonLayout = nullptr;
    m_tabletModeGSettings = nullptr;
    m_rotationGSettings = nullptr;
    m_appViewContainer = nullptr;
    m_fileWatcher = nullptr;
    m_fileWatcher1 = nullptr;
    m_directoryChangedThread = nullptr;
    m_exitAnimation = nullptr;
}

void TabletMode::changePage(qint32 signal, bool hide)
{
    if (!m_exitAnimation) {
        m_exitAnimation = new QPropertyAnimation(m_container, "pos");
    }

    if (!m_showAnimation) {
        m_showAnimation = new QPropertyAnimation(m_container, "pos");
    }

    if ((m_exitAnimation->state() != QPropertyAnimation::Stopped) || (m_showAnimation->state() != QPropertyAnimation::Stopped)) return;
    QRect saveRect = m_container->geometry();
    //当app页面数量发生变化时，会跳转到第一页，添加该处的目的是可以选择不显示动画，在显示完毕后直接显示。
    this->setHidden(hide);
    //显示动画
    m_exitAnimation->setEasingCurve(QEasingCurve::Linear);
    m_exitAnimation->setStartValue(saveRect.topLeft());
    m_exitAnimation->setDuration(500);

    if (signal < 0) {
        //上一页（左边）
        m_exitAnimation->setEndValue(QPoint(saveRect.width(), 0));

    } else {
        //下一页（右边）
        m_exitAnimation->setEndValue(QPoint(-saveRect.width(), 0));
    }

    connect(m_exitAnimation, &QPropertyAnimation::finished, this, [=] {
        m_exitAnimation->stop();
        exitAnimationFinished(signal, hide);
    });

    m_exitAnimation->start();
}

void TabletMode::exitAnimationFinished(qint32 signal, bool hide)
{
    //fix bug #82786
    if (!m_isActivated) return;

    QRect screenRect = QApplication::primaryScreen()->geometry();

    m_container->hide();
    QRect endRect;
    if (signal < 0) {
        endRect = QRect(-screenRect.width(), screenRect.y(), screenRect.width(), screenRect.height());
    } else {
        endRect = QRect(screenRect.width(), screenRect.y(), screenRect.width(), screenRect.height());
    }
    m_container->setGeometry(endRect);
    m_container->setWindowOpacity(1);

    if (Style::nowpagenum == 1) {
        //只在第一页显示小组件
        m_pluginBoxWidget->show();
        if (Style::ScreenRotation) {
            m_pluginBoxWidget->hidePluginWidget(true);
            screenVertical();
        } else {
            m_pluginBoxWidget->hidePluginWidget(false);
            screenHorizontal();
        }
    } else {
        //不显示小组件
        m_pluginBoxWidget->hide();
        m_pluginBoxWidget->hidePluginWidget(true);

        m_appViewContainer->setGeometry(QRect(0, 0, m_width, m_height - Style::ButtonWidgetHeight));
    }
    //不是页面数量变化就不用隐藏
    m_container->setHidden(hide);
    //更新大小，设置数据
    m_appViewContainer->repaintWid(Style::ScreenRotation);
    m_appViewContainer->updatePageData();

    //显示动画
    m_showAnimation->setEasingCurve(QEasingCurve::InOutQuad);
    m_showAnimation->setStartValue(endRect.topLeft());
    m_showAnimation->setEndValue(screenRect.topLeft());
    //hide 相当于不显示动画
    m_showAnimation->setDuration(hide ? 50 : 500);

    connect(m_showAnimation, &QVariantAnimation::finished, this, [=] {
        //显示按钮
        m_showAnimation->stop();
        show();
        m_container->show();
        m_pageButtonWidget->show();
        m_pageButtonWidget->setDisabled(false);
    });

    m_showAnimation->start();
}

void TabletMode::moveWindow(qint32 length)
{
    if (m_isPause) {
        return;
    }
    QPoint newPoint = {(m_container->geometry().x() + length), m_container->geometry().y()};
    m_container->move(newPoint);
}

void TabletMode::returnRawPoint()
{
    //归位
    if (!m_returnAnimation) {
        m_returnAnimation = new QPropertyAnimation(m_container, "pos");
        m_returnAnimation->setEasingCurve(QEasingCurve::OutCubic);
        m_returnAnimation->setEndValue(QPoint{0, 0});
        m_returnAnimation->setDuration(300);
    }

    if (m_returnAnimation->state() != QVariantAnimation::Stopped) {
        return;
    }

    m_returnAnimation->setStartValue(m_container->geometry().topLeft());
    m_returnAnimation->start();
}

void TabletMode::updateTabletModeValue(bool mode)
{
    m_isTabletMode = mode;
}

void TabletMode::updateRotationsValue(QString rotation)
{
    qDebug() << "[TabletMode::updateRotationsValue] rotation:" << rotation;
    m_direction = rotation;

    m_appViewContainer->updateStyleValue();
    screenRotation();
}
