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

    QToolButton *addPageButton = new QToolButton(this);
    addPageButton->setFixedSize(QSize(36, 36));
    addPageButton->setIcon(QIcon::fromTheme("list-add-symbolic"));
    connect(addPageButton, &QToolButton::clicked, this, [=](){
        addPage(nullptr, true);
    });

    m_float_button = addPageButton;

    connect(this, &QTabBar::tabCloseRequested, this, [=](int index){
        removeTab(index);
    });

    addPage("file:///");
    addPage("file:///etc");

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
        QString uri = tabData(count() - 1).toString();
        addPage(uri, jumpToNewTab);
    }
}

void NavigationTabBar::tabRemoved(int index)
{
    QTabBar::tabRemoved(index);
    if (count() == 0) {
        Q_EMIT closeWindowRequest();
    }
    relayoutFloatButton(false);
}

void NavigationTabBar::tabInserted(int index)
{
    QTabBar::tabInserted(index);
    relayoutFloatButton(true);
}

void NavigationTabBar::relayoutFloatButton(bool insterted)
{
    if (count() == 0)
        return;
    qDebug()<<"relayout";
    auto lastTabRect = tabRect(count() - 1);
    if (insterted) {
        m_float_button->move(lastTabRect.right(), 0);
    } else {
        m_float_button->move(lastTabRect.right(), 0);
    }
}
