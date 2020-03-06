#include "navigation-tab-bar.h"
#include "x11-window-manager.h"

#include "file-utils.h"

#include <QToolButton>

NavigationTabBar::NavigationTabBar(QWidget *parent) : QTabBar(parent)
{
    setFixedHeight(36);

    setProperty("useStyleWindowManager", false);
    setMovable(true);
    setExpanding(false);
    setTabsClosable(true);
    X11WindowManager::getInstance()->registerWidget(this);

    connect(this, &QTabBar::currentChanged, this, [=](int index){
        //qDebug()<<"current changed"<<index;
    });

    connect(this, &QTabBar::tabMoved, this, [=](int from, int to){
        //qDebug()<<"move"<<from<<"to"<<to;
    });

    connect(this, &QTabBar::tabBarClicked, this, [=](int index){
        //qDebug()<<"tab bar clicked"<<index;
    });

    connect(this, &QTabBar::tabBarDoubleClicked, this, [=](int index){
        //qDebug()<<"tab bar double clicked"<<index;
    });

    QToolButton *addPageButton = new QToolButton(this);
    addPageButton->setFixedSize(QSize(36, 36));
    addPageButton->setIcon(QIcon::fromTheme("list-add-symbolic"));
    connect(addPageButton, &QToolButton::clicked, this, [=](){
        auto uri = tabData(currentIndex()).toString();
        Q_EMIT addPageRequest(uri, true);
    });

    m_float_button = addPageButton;

//    connect(this, &QTabBar::tabCloseRequested, this, [=](int index){
//        removeTab(index);
//    });

    setDrawBase(false);
}

void NavigationTabBar::addPages(const QStringList &uri)
{

}

void NavigationTabBar::addPage(const QString &uri, bool jumpToNewTab)
{
    if (!uri.isNull()) {
        auto iconName = Peony::FileUtils::getFileIconName(uri);
        auto displayName = Peony::FileUtils::getFileDisplayName(uri);
        addTab(QIcon::fromTheme(iconName), displayName);
        setTabData(count() - 1, uri);
        if (jumpToNewTab)
            setCurrentIndex(count() - 1);
        Q_EMIT this->pageAdded(uri);
    } else {
        if (currentIndex() == -1) {
            addPage("file:///", true);
        } else {
            QString uri = tabData(currentIndex()).toString();
            addPage(uri, jumpToNewTab);
        }
    }
}

void NavigationTabBar::tabRemoved(int index)
{
    //qDebug()<<"tab removed"<<index;
    QTabBar::tabRemoved(index);
    if (count() == 0) {
        Q_EMIT closeWindowRequest();
    }
    relayoutFloatButton(false);
}

void NavigationTabBar::tabInserted(int index)
{
    //qDebug()<<"tab inserted"<<index;
    QTabBar::tabInserted(index);
    relayoutFloatButton(true);
}

void NavigationTabBar::relayoutFloatButton(bool insterted)
{
    if (count() == 0) {
        m_float_button->move(0, 0);
        return;
    }
    //qDebug()<<"relayout";
    auto lastTabRect = tabRect(count() - 1);
    if (insterted) {
        m_float_button->move(lastTabRect.right(), 0);
    } else {
        m_float_button->move(lastTabRect.right(), 0);
    }
}
