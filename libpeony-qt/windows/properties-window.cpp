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
#include "mark-properties-page-factory.h"
#include "open-with-properties-page-factory.h"
#include "details-properties-page-factory.h"

#include <QToolBar>
#include <QPushButton>
#include <QProcess>
#include <QDebug>
#include <QStatusBar>

#include <QList>
#include <QStringList>
#include <QString>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QTimer>
#include <QPainter>
#include <QStyleOptionTab>

#include "file-info-job.h"

using namespace Peony;

#define WINDOW_NOT_OPEN -1
//single properties-window
//static QHash<QString,PropertiesWindow> openPropertiesWindow = nullptr;
static QList<PropertiesWindow *> *openedPropertiesWindows = nullptr;

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

PropertiesWindowPluginManager::PropertiesWindowPluginManager(QObject *parent) : QObject(parent)
{
    //register internal factories.
    registerFactory(BasicPropertiesPageFactory::getInstance());
    registerFactory(PermissionsPropertiesPageFactory::getInstance());
    registerFactory(ComputerPropertiesPageFactory::getInstance());
    registerFactory(RecentAndTrashPropertiesPageFactory::getInstance());
    registerFactory(MarkPropertiesPageFactory::getInstance());
    registerFactory(OpenWithPropertiesPageFactory::getInstance());
    registerFactory(DetailsPropertiesPageFactory::getInstance());
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
    QStringList list;
    for (auto factoryId : m_sorted_factory_map) {
        list << factoryId;
    }
    return list;
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
const QSize  PropertiesWindow::s_bottomButtonSize   = QSize(100, 32);

PropertiesWindow::PropertiesWindow(const QStringList &uris, QWidget *parent) : QMainWindow(parent)
{
    m_uris = uris;
    m_uris.removeDuplicates();
    qDebug() << __FUNCTION__ << m_uris.count() << m_uris;

    //FIX:BUG #31635
    if (m_uris.contains("computer:///")) {
        QtConcurrent::run([=]() {
            gotoAboutComputer();
        });
        m_uris.removeAt(m_uris.indexOf("computer:///"));
    }

    if (m_uris.count() == 0) {
        m_destroyThis = true;
        return;
    }

    if (!PropertiesWindow::checkUriIsOpen(m_uris, this)) {
        this->init();
    } else {
        m_destroyThis = true;
    }
}

void PropertiesWindow::init()
{
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->setContentsMargins(0, 18, 0, 0);
    //only show close button
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowMinMaxButtonsHint & ~Qt::WindowSystemMenuHint);

    //不能更改执行顺序 - Cannot change execution order
    this->notDir();

    this->setWindowTitleTextAndIcon();

    if (m_notDir) {
        //如果含有文件夹，那么高度是600，如果是其他文件，那么高度是652
        //If there are folders, the height is 600, if it is other files, the height is 652
        this->setFixedSize(PropertiesWindow::s_windowWidth, PropertiesWindow::s_windowHeightOther);
    } else {
        this->setFixedSize(PropertiesWindow::s_windowWidth, PropertiesWindow::s_windowHeightFolder);
    }
    this->initStatusBar();

    this->initTabPage(m_uris);
}

/*!
 * recent:///  : 最近
 * trash:///   : 回收站
 *
 * \brief PropertiesWindow::setWindowTitleText
 */
void PropertiesWindow::setWindowTitleTextAndIcon()
{
    QString windowTitle = "";
    QString iconName = "system-file-manager";

    /**
     * \brief Trash 和 Recent 情况下，uris中只有一个uri - In the case of Trash and Recent, there is only one uri in the uris
     */
    if (m_uris.contains("trash:///")) {
        windowTitle = tr("Trash");
        iconName = m_fileInfo.get()->iconName();

    } else if (m_uris.contains("recent:///")) {
        windowTitle = tr("Recent");
        iconName = m_fileInfo.get()->iconName();

    } else {
        qint32 fileNum = m_uris.count();

        if (fileNum > 1) {
            //use default icon
            windowTitle = tr("Selected") + QString(tr(" %1 Files")).arg(fileNum);
        } else {
            qDebug() << __FILE__ << __FUNCTION__ << "fileInfo is null :" << (m_fileInfo.get() == nullptr);
            if (m_fileInfo) {
                windowTitle = m_fileInfo.get()->displayName();
                iconName = m_fileInfo.get()->iconName();
            }
        }
    }

    windowTitle += " " + tr("Properties");
    this->setWindowIcon(QIcon::fromTheme(iconName));
    this->setWindowTitle(windowTitle);
}

void PropertiesWindow::notDir()
{
    //FIXME:请尝试使用非阻塞方式获取 FIleInfo - Please try to obtain FIleInfo in a non-blocking way
    quint64 index = 0;
    for (QString uri : m_uris) {
        auto fileInfo = FileInfo::fromUri(uri);
        FileInfoJob *fileInfoJob = new FileInfoJob(fileInfo);
        fileInfoJob->setAutoDelete();
        fileInfoJob->querySync();

        if (index == 0) {
            m_fileInfo = fileInfo;
            index ++;
        }

        if (fileInfo.get()->isDir()) {
            m_notDir = false;
            break;
        }
    }
}

void PropertiesWindow::show()
{
    if (m_destroyThis) {
        this->close();
        return;
    }

    return QWidget::show();
}

void PropertiesWindow::gotoAboutComputer()
{
    QProcess p;
    p.setProgram("ukui-control-center");
    //-a para to show about computer infos
    p.setArguments(QStringList() << "-a");
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
void PropertiesWindow::initStatusBar()
{
    QStatusBar *statusBar = new QStatusBar(this);

    //    statusBar->setFixedSize(PropertiesWindow::s_windowWidth,64);
    statusBar->setMinimumSize(PropertiesWindow::s_windowWidth, 64);

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

    QPushButton *okButton = new QPushButton(tr("Ok"), statusBar);
    QPushButton *cancelButton = new QPushButton(tr("Cancel"), statusBar);

    okButton->setMinimumSize(PropertiesWindow::s_bottomButtonSize);
    cancelButton->setMinimumSize(PropertiesWindow::s_bottomButtonSize);

    bottomToolLayout->addWidget(okButton);
    bottomToolLayout->addWidget(cancelButton);

    okButton->move(344, 16);
    cancelButton->move(236, 16);

    statusBar->setLayout(bottomToolLayout);

    this->setStatusBar(statusBar);

    //set cancelButton event process
    connect(cancelButton, &QPushButton::clicked, this, &QMainWindow::close);
    connect(okButton, &QPushButton::clicked, this, &PropertiesWindow::saveAllChanged);
}

void PropertiesWindow::initTabPage(const QStringList &uris)
{
    auto window = new PropertiesWindowPrivate(uris, this);
    window->tabBar()->setStyle(new tabStyle);
    //Warning: 不要设置tab高度，否则会导致tab页切换上下跳动
    //Do not set the tab height, otherwise it will cause the tab page to switch up and down
    //window->tabBar()->setMinimumHeight(72);

    window->tabBar()->setContentsMargins(0,0,0,0);
    this->setCentralWidget(window);
}

bool PropertiesWindow::checkUriIsOpen(QStringList &uris, PropertiesWindow *newWindow)
{
    if (!openedPropertiesWindows) {
        openedPropertiesWindows = new QList<PropertiesWindow *>();
        qDebug() << __FILE__ << __FUNCTION__ << "new->openedPropertiesWindows";
    }
    //1.对uris进行排序 - Sort uris
    std::sort(uris.begin(), uris.end(), [](QString a, QString b) {
        return a < b;
    });

    //2.检查是否已经打开 - Check if it is open
    qint64 index = PropertiesWindow::getOpenUriIndex(uris);
    if (index != WINDOW_NOT_OPEN) {
        openedPropertiesWindows->at(index)->raise();
        return true;
    }

    openedPropertiesWindows->append(newWindow);

    return false;
}

qint64 PropertiesWindow::getOpenUriIndex(QStringList &uris)
{
    //strong !
    if (!openedPropertiesWindows)
        return WINDOW_NOT_OPEN;

    quint64 index = 0;
    for (PropertiesWindow *window : *openedPropertiesWindows) {
        if (window->getUris() == uris) {
            //当前的uris已经存在打开的窗口 - The current uris already exists in the open window
            return index;
        }
        index++;
    }

    return WINDOW_NOT_OPEN;
}

void PropertiesWindow::removeThisWindow(qint64 index)
{
    if (index == WINDOW_NOT_OPEN)
        return;

    if (!openedPropertiesWindows)
        return;

    openedPropertiesWindows->removeAt(index);

    if (openedPropertiesWindows->count() == 0) {
        delete openedPropertiesWindows;
        openedPropertiesWindows = nullptr;
        qDebug() << __FILE__ << __FUNCTION__ << "delete->openedPropertiesWindows";
    }

}

void PropertiesWindow::closeEvent(QCloseEvent *event)
{
    /**
     * \brief 如果当前uris窗口已经打开，那么不能移除全局的记录，只需要关闭当前窗口即可
     * If the current uris window is already open, then the global record cannot be removed, just close the current window
     */
    if (m_destroyThis)
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
    if (!m_openTabPage.count() == 0) {
        for (auto tabPage : m_openTabPage) {
            tabPage->saveAllChange();
        }
    }

    this->close();
}

//properties window
PropertiesWindowPrivate::PropertiesWindowPrivate(const QStringList &uris, QWidget *parent) : QTabWidget(parent)
{
    setTabsClosable(false);
    setMovable(false);
    setContentsMargins(0, 0, 0, 0);

    auto manager = PropertiesWindowPluginManager::getInstance();
    auto names = manager->getFactoryNames();
    for (auto name : names) {
        auto factory = manager->getFactory(name);
        if (factory->supportUris(uris)) {
            auto tabPage = factory->createTabPage(uris);
            tabPage->setParent(this);
            addTab(tabPage, factory->name());

            (qobject_cast<PropertiesWindow *>(parent))->addTabPage(tabPage);
        }
    }
}

void tabStyle::drawControl(QStyle::ControlElement element, const QStyleOption *option, QPainter *painter,
                           const QWidget *widget) const
{
    /**
     * FIX:需要修复颜色不能跟随主题的问题
     * \brief
     */
    if (element == CE_TabBarTab) {
        if (const QStyleOptionTab *tab = qstyleoption_cast<const QStyleOptionTab *>(option)) {
            //设置按钮的左右上下偏移
            QRect rect = (tab->rect).adjusted(4, 0, 1, -12);

            const QPalette &palette = widget->palette();

            if (tab->state & QStyle::State_Selected) {
                painter->save();
                painter->setPen(palette.color(QPalette::Highlight));
                painter->setBrush(palette.brush(QPalette::Highlight));

                painter->drawRect(rect);
                //FIX:圆角矩形绘制问题
                //painter->drawRoundRect(rect,10,17);
                painter->restore();
            }

            painter->setPen(palette.color(QPalette::ButtonText));

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

QSize tabStyle::sizeFromContents(QStyle::ContentsType ct, const QStyleOption *opt, const QSize &contentsSize,
                                 const QWidget *widget) const
{
    QSize barSize = QProxyStyle::sizeFromContents(ct, opt, contentsSize, widget);

    if (ct == QStyle::CT_TabBarTab) {
        barSize.transpose();
        const QStyleOptionTab *tab = qstyleoption_cast<const QStyleOptionTab *>(opt);
        //解决按钮不能自适应的问题
        int fontWidth = tab->fontMetrics.width(tab->text);
        //宽度统一加上30px
        barSize.setWidth(fontWidth + 30);

        int fontHeight = tab->fontMetrics.height();
        if (fontHeight <= 30) {
            //数值大于设计稿的30是因为在下方偏移了12px
            barSize.setHeight(42);
        } else {
            //同上所述
            barSize.setHeight(fontHeight + 12);
        }
//        qDebug() << "tabStyle::sizeFromContents font width:" << fontWidth << "height:" << fontHeight << "text:"
//                 << tab->text;
    }

    return barSize;
}
