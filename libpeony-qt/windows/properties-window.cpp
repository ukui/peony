/*
 * Peony-Qt's Library
 *
 * Copyright (C) 2020, KylinSoft Co., Ltd.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Authors: Yue Lan <lanyue@kylinos.cn>
 *
 */

#include "properties-window.h"

#include "properties-window-tab-page-plugin-iface.h"

#include "basic-properties-page-factory.h"
#include "permissions-properties-page-factory.h"
#include "computer-properties-page-factory.h"
#include "recent-and-trash-properties-page-factory.h"

#include <QToolBar>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QProcess>
#include <QDebug>
#include <QStatusBar>

#include <QStringList>
#include <QString>
#include <QHBoxLayout>
#include <QDialogButtonBox>
#include <QSpacerItem>
#include <QTimer>
#include <QPainter>
#include <QStyleOptionTab>

#include "file-info.h"
#include "file-info-job.h"

using namespace Peony;

//single properties-window
//static QHash<QString,PropertiesWindow> openPropertiesWindow = nullptr;
static QList<PropertiesWindow *> *openPropertiesWindows = nullptr;

//plugin manager

static PropertiesWindowPluginManager *global_instance = nullptr;

PropertiesWindowPluginManager *PropertiesWindowPluginManager::getInstance()
{
    if (!global_instance)
        global_instance = new PropertiesWindowPluginManager;
    return global_instance;
}

void PropertiesWindowPluginManager::release()
{
    deleteLater();
}

PropertiesWindowPluginManager::PropertiesWindowPluginManager(QObject *parent) : QObject (parent)
{
    //register internal factories.
    registerFactory(BasicPropertiesPageFactory::getInstance());
    registerFactory(PermissionsPropertiesPageFactory::getInstance());
    registerFactory(ComputerPropertiesPageFactory::getInstance());
    registerFactory(RecentAndTrashPropertiesPageFactory::getInstance());
}

PropertiesWindowPluginManager::~PropertiesWindowPluginManager()
{
    for (auto factory : m_factory_hash) {
        factory->closeFactory();
    }
    m_factory_hash.clear();
}


bool PropertiesWindowPluginManager::registerFactory(PropertiesWindowTabPagePluginIface *factory)
{
    m_mutex.lock();
    auto id = factory->name();
    if (m_factory_hash.value(id)) {
        m_mutex.unlock();
        return false;
    }

    m_factory_hash.insert(id, factory);
    m_sorted_factory_map.insert(-factory->tabOrder(), id);
    m_mutex.unlock();
    return true;
}

const QStringList PropertiesWindowPluginManager::getFactoryNames()
{
    QStringList l;
    for (auto factoryId : m_sorted_factory_map) {
        l<<factoryId;
    }
    return l;
}

PropertiesWindowTabPagePluginIface *PropertiesWindowPluginManager::getFactory(const QString &id)
{
    return m_factory_hash.value(id);
}

/*!
 * init PropertiesWindows`s static member
 * \brief PropertiesWindow::s_windowWidth
 */
const qint32 PropertiesWindow::s_windowWidth        = 460;
const qint32 PropertiesWindow::s_windowHeightFolder = 600;
const qint32 PropertiesWindow::s_windowHeightOther  = 652;
const QSize  PropertiesWindow::s_bottomButtonSize   = QSize(100,32);
const QSize  PropertiesWindow::s_topButtonSize      = QSize(65,30);

PropertiesWindow::PropertiesWindow(const QStringList &uris, QWidget *parent) : QMainWindow (parent)
{
    m_uris = uris;

    if(!PropertiesWindow::checkUriIsOpen(m_uris,this)) {

        this->setContextMenuPolicy(Qt::CustomContextMenu);

        if (uris.contains("computer:///"))
            gotoAboutComputer();
        else {

            this->setWindowTitleTextAndIcon();

            this->setAttribute(Qt::WA_DeleteOnClose);
            this->setContentsMargins(0, 18, 0, 0);
            //only show closs button
            this->setWindowFlags(this->windowFlags() &~ Qt::WindowMinMaxButtonsHint &~ Qt::WindowSystemMenuHint);
            qDebug() << "PropertiesWindow::PropertiesWindow" << "运行追踪 1111";

            if(this->notDir())
                //如果含有文件夹，那么高度是600，如果是其他文件，那么高度是652
                this->setFixedSize(PropertiesWindow::s_windowWidth,PropertiesWindow::s_windowHeightOther);
            else
                this->setFixedSize(PropertiesWindow::s_windowWidth,PropertiesWindow::s_windowHeightFolder);

            this->initTabPage(uris);
            this->initStatusBar();
        }
    }else {
        this->m_destroyThis = true;
    }
}

/*!
 * recent:///  : 最近
 * trash:///   : 回收站
 *
 * \brief PropertiesWindow::setWindowTitleText
 */
void PropertiesWindow::setWindowTitleTextAndIcon()
{
    QString l_windowTitle = "";
    QString l_iconName = "system-file-manager";

    auto l_fineInfo = FileInfo::fromUri(m_uris.at(0));
    FileInfoJob *j = new FileInfoJob(l_fineInfo);
    j->setAutoDelete();
    j->querySync();

    if(m_uris.contains("trash:///")) {
        l_windowTitle = tr("Trash");
        l_iconName = l_fineInfo.get()->iconName();
        //新版本似乎没有最近？
    } else if(m_uris.contains("recent:///")) {
        l_windowTitle = tr("Recent");
        l_iconName = l_fineInfo.get()->iconName();

    } else {
        qint32 l_fileNum = this->m_uris.count();

        if(l_fileNum > 1) {
            //use default icon
            l_windowTitle = tr("Selected") + QString(tr(" %1 Files")).arg(l_fileNum);
        } else {
            qDebug() << "PropertiesWindow::setWindowTitleTextAndIcon():文件信息为空?" << (l_fineInfo.get() == nullptr);
            if(l_fineInfo) {
                l_windowTitle = l_fineInfo.get()->displayName();
                l_iconName = l_fineInfo.get()->iconName();
            }
        }
    }

    l_windowTitle += " " + tr("Properties");
    this->setWindowIcon(QIcon::fromTheme(l_iconName));
    this->setWindowTitle(l_windowTitle);
}

bool PropertiesWindow::notDir()
{
    for(QString uri : m_uris) {
        auto l_fineInfo = FileInfo::fromUri(uri);
        FileInfoJob *j = new FileInfoJob(l_fineInfo);
        j->setAutoDelete();
        j->querySync();
        if(l_fineInfo.get()->isDir())
            return false;
    }
    return true;
}

void PropertiesWindow::show()
{   
    if(m_destroyThis) {
        this->close();
        return;
    }

    //跳转到关于电脑
    if (m_uris.contains("computer:///"))
        this->close();
    else
        return QWidget::show();
}

void PropertiesWindow::gotoAboutComputer()
{
    QProcess p;
    p.setProgram("ukui-control-center");
    //-a para to show about computer infos
    p.setArguments(QStringList()<<"-a");
#if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
    p.startDetached();
#else
    p.startDetached("ukui-control-center", QStringList()<<"-a");
    p.waitForFinished(-1);
#endif
}

/*!
 *
 * \brief PropertiesWindow::initStatusBar
 */
void PropertiesWindow::initStatusBar(){
    QStatusBar *statusBar = new QStatusBar(this);

    //    statusBar->setFixedSize(PropertiesWindow::s_windowWidth,64);
    statusBar->setMinimumSize(PropertiesWindow::s_windowWidth,64);

    // use button-box  暂时不能使用button box实现底部按钮
    //    QDialogButtonBox *buttonBox = new QDialogButtonBox(Qt::Horizontal,statusBar);
    //    buttonBox->setMinimumSize(PropertiesWindow::s_windowWidth,64);
    //    buttonBox->setContentsMargins(0,0,16,0);

    //    QPushButton *cancelButton = buttonBox->addButton(tr("Cancel"),QDialogButtonBox::RejectRole);
    //    QPushButton *okButton = buttonBox->addButton(tr("Ok"),QDialogButtonBox::AcceptRole);

    //    okButton->setMinimumSize(PropertiesWindow::s_bottomButtonSize);
    //    cancelButton->setMinimumSize(PropertiesWindow::s_bottomButtonSize);

    //    statusBar->addWidget(buttonBox);

    //use HBox-layout
    QHBoxLayout *bottomToolLayout = new QHBoxLayout(statusBar);

    QPushButton *okButton = new QPushButton(tr("Ok"),statusBar);
    QPushButton *cancelButton = new QPushButton(tr("Cancel"),statusBar);

    okButton->setMinimumSize(PropertiesWindow::s_bottomButtonSize);
    cancelButton->setMinimumSize(PropertiesWindow::s_bottomButtonSize);

    bottomToolLayout->addWidget(okButton);
    bottomToolLayout->addWidget(cancelButton);

    okButton->move(344,16);
    cancelButton->move(236,16);

    statusBar->setLayout(bottomToolLayout);

    this->setStatusBar(statusBar);

    //set cancelButton event process
    connect(cancelButton,&QPushButton::clicked,this,&QMainWindow::close);
    connect(okButton,&QPushButton::clicked,this,&PropertiesWindow::saveAllChanged);
}

void PropertiesWindow::initTabPage(const QStringList &uris)
{
    auto w = new PropertiesWindowPrivate(uris, this);
    w->tabBar()->setStyle(new tabStyle);
    w->tabBar()->setMinimumHeight(72);

    //    w->tabBar()->setMinimumSize(PropertiesWindow::s_windowWidth,72);
    this->setCentralWidget(w);
}

bool PropertiesWindow::checkUriIsOpen(QStringList &uris, PropertiesWindow *newWindow)
{
    if(!openPropertiesWindows)
        openPropertiesWindows = new QList<PropertiesWindow*>();

    //1.对uris进行排序
    std::sort(uris.begin(), uris.end(), [](QString a, QString b) {
        return a < b;
    });
        for(QString uri : uris) {
            qDebug() << "PropertiesWindow::checkUriIsOpen排序结果" << uri;
        }
    //2.检查是否已经打开
    if(PropertiesWindow::getOpenUriIndex(uris) != -1)
        return true;

    qDebug() << "m_uris" << newWindow->getUris();
    openPropertiesWindows->append(newWindow);

    return false;
}

qint64 PropertiesWindow::getOpenUriIndex(QStringList &uris)
{
    //strong !
    if(!openPropertiesWindows)
        return -1;

    quint64 index = 0;
    for(PropertiesWindow *window : *openPropertiesWindows) {
        if(window->getUris() == uris) {
            //当前的uris已经存在打开的窗口
            return index;
        }
        index ++;
    }

    return -1;
}

void PropertiesWindow::removeThisWindow(qint64 index)
{
    if(index == -1)
        return;

    if(!openPropertiesWindows)
        return;

    openPropertiesWindows->removeAt(index);

}

void PropertiesWindow::closeEvent(QCloseEvent *event)
{
    //如果该窗口已经打开，那么不能移除
    if(this->m_destroyThis)
        return;

    PropertiesWindow::removeThisWindow(PropertiesWindow::getOpenUriIndex(this->getUris()));
}

/*!
 * save all changed settings when 'ok' is clicked
 * \brief PropertiesWindow::saveAllChanged
 */
void PropertiesWindow::saveAllChanged()
{
    qDebug() << "PropertiesWindow::saveAllChanged()" << "count" << m_openTabPage.count();
    if(m_openTabPage.count() == 0)
        return;

    for(auto tabPage : m_openTabPage) {
        tabPage->saveAllChange();
    }

    this->close();
}

//properties window
PropertiesWindowPrivate::PropertiesWindowPrivate(const QStringList &uris, QWidget *parent) : QTabWidget(parent)
{
    setTabsClosable(false);
    setMovable(false);
    setContentsMargins(0,0,0,0);

    auto manager = PropertiesWindowPluginManager::getInstance();
    auto names = manager->getFactoryNames();
    for (auto name : names) {
        auto factory = manager->getFactory(name);
        if (factory->supportUris(uris)) {
            auto tabPage = factory->createTabPage(uris);
            tabPage->setParent(this);
            addTab(tabPage, factory->name());

            (qobject_cast<PropertiesWindow*>(parent))->addTabPage(tabPage);
        }
    }
}

void tabStyle::drawControl(QStyle::ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    if (element == CE_TabBarTab) {
        if (const QStyleOptionTab *tab = qstyleoption_cast<const QStyleOptionTab *>(option)) {
            //设置按钮的左右上下偏移
            QRect rect = (tab->rect).adjusted(4,0,1,-12);

            if (tab->state & QStyle::State_Selected) {
                painter->save();
                painter->setPen(0x3D6BE5);
                painter->setBrush(QBrush(0x3D6BE5));

                painter->drawRect(rect);
                //FIX:圆角矩形绘制问题
                //                painter->drawRoundRect(rect,10,17);
                painter->restore();
            }

            if (tab->state & QStyle::State_Selected) {
                painter->setPen(0xffffff);
            } else {
                QColor color(0,0,0);
                painter->setPen(color);
            }

            QTextOption alignCenter;
            alignCenter.setAlignment(Qt::AlignCenter);
            painter->drawText(rect, tab->text, alignCenter);

            return;
        }
    }
    if (element == CE_TabBarTabLabel) {
        QProxyStyle::drawControl(element, option, painter, widget);
    }
}

QSize tabStyle::sizeFromContents(QStyle::ContentsType ct, const QStyleOption *opt, const QSize &contentsSize, const QWidget *w) const
{
    QSize barSize = QProxyStyle::sizeFromContents(ct,opt,contentsSize,w);

    if(ct == QStyle::CT_TabBarTab) {
        barSize.transpose();
        const QStyleOptionTab *tab = qstyleoption_cast<const QStyleOptionTab *>(opt);
        //解决按钮不能自适应的问题
        int fontWidth = tab->fontMetrics.width(tab->text);
        if(fontWidth <= 65)
            //数值大于设计稿的65是因为在左侧偏移了4px
            barSize.setWidth(70);
        else
            //同上所述
            barSize.setWidth(fontWidth + 10);

        //保证底部距离为设计稿上的8px

        int fontHeight = tab->fontMetrics.height();
        if(fontHeight <= 30)
            //数值大于设计稿的30是因为在下方偏移了12px
            barSize.setHeight(42);
        else
            //同上所述
            barSize.setHeight(fontHeight + 12);

        qDebug() << "tabStyle::sizeFromContents font width:" << fontWidth << "height:" << fontHeight << "text:" << tab->text;
    }

    return barSize;
}
