#include "header-bar.h"
#include "main-window.h"

#include <QHBoxLayout>
#include <advanced-location-bar.h>

HeaderBar::HeaderBar(MainWindow *parent) : QToolBar(parent)
{
    m_window = parent;
    setAttribute(Qt::WA_OpaquePaintEvent);
    setStyleSheet(".HeaderBar{"
                  "background-color: transparent;"
                  "border: 0px solid transparent;"
                  "margin: 4, 5, 4, 5;"
                  "};");
    setMovable(false);

    auto createFolder = new HeaderBarToolButton(this);
    createFolder->setToolTip(tr("Create Folder"));
    createFolder->setIcon(QIcon::fromTheme("folder-new-symbolic"));
    createFolder->setFixedSize(QSize(40, 40));
    addWidget(createFolder);

    auto openTerminal = new HeaderBarToolButton(this);
    openTerminal->setToolTip(tr("Open Terminal"));
    openTerminal->setIcon(QIcon::fromTheme("terminal-app-symbolic"));
    openTerminal->setFixedSize(QSize(40, 40));
    addWidget(openTerminal);

    addSeparator();

    auto goBack = new HeadBarPushButton(this);
    goBack->setToolTip(tr("Go Back"));
    goBack->setFixedSize(QSize(36, 28));
    goBack->setIcon(QIcon::fromTheme("go-previous-symbolic"));
    addWidget(goBack);

    auto goForward = new HeadBarPushButton(this);
    goForward->setToolTip(tr("Go Forward"));
    goForward->setFixedSize(QSize(36, 28));
    goForward->setIcon(QIcon::fromTheme("go-next-symbolic"));
    addWidget(goForward);

    addSeparator();

    auto locationBar = new Peony::AdvancedLocationBar(this);
    addWidget(locationBar);

    auto serach = new HeaderBarToolButton(this);
    serach->setToolTip(tr("Search"));
    serach->setIcon(QIcon::fromTheme("edit-find-symbolic"));
    serach->setFixedSize(QSize(40, 40));
    addSeparator();
    addWidget(serach);

    addSeparator();

    auto viewType = new HeaderBarToolButton(this);
    viewType->setToolTip(tr("View Type"));
    viewType->setIcon(QIcon::fromTheme("view-grid-symbolic"));
    viewType->setFixedSize(QSize(57, 40));
    viewType->setPopupMode(QToolButton::MenuButtonPopup);
    addWidget(viewType);

    auto sortType = new HeaderBarToolButton(this);
    sortType->setToolTip(tr("Sort Type"));
    sortType->setIcon(QIcon::fromTheme("view-sort-descending-symbolic"));
    sortType->setFixedSize(QSize(57, 40));
    sortType->setPopupMode(QToolButton::MenuButtonPopup);
    addWidget(sortType);

    auto popMenu = new HeaderBarToolButton(this);
    popMenu->setToolTip(tr("Option"));
    popMenu->setIcon(QIcon::fromTheme("open-menu-symbolic"));
    popMenu->setFixedSize(QSize(40, 40));
    addWidget(popMenu);

    //minimize, maximize and close
    auto close = new HeaderBarToolButton(this);
    close->setToolTip(tr("Close"));
    close->setIcon(QIcon::fromTheme("window-close-symbolic"));
    close->setFixedSize(QSize(40, 40));

    //window-maximize-symbolic
    //window-restore-symbolic
    auto maximizeAndRestore = new HeaderBarToolButton(this);
    maximizeAndRestore->setToolTip(tr("Maximize"));
    maximizeAndRestore->setIcon(QIcon::fromTheme("window-maximize-symbolic"));
    maximizeAndRestore->setFixedSize(QSize(40, 40));

    auto minimize = new HeaderBarToolButton(this);
    minimize->setToolTip(tr("Minimize"));
    minimize->setIcon(QIcon::fromTheme("window-minimize-symbolic"));
    minimize->setFixedSize(QSize(40, 40));

    QWidget *container = new QWidget(this);
    container->setContentsMargins(0, 0, 0, 0);
    QHBoxLayout *hbox = new QHBoxLayout(container);
    hbox->setContentsMargins(0, 0, 0, 0);
    hbox->setSpacing(5);
    hbox->addWidget(minimize);
    hbox->addWidget(maximizeAndRestore);
    hbox->addWidget(close);
    container->setLayout(hbox);

    addWidget(container);
}

void HeaderBar::setLocation(const QString &uri)
{
    //FIXME:
}

//HeaderBarToolButton
HeaderBarToolButton::HeaderBarToolButton(QWidget *parent) : QToolButton(parent)
{
    setIconSize(QSize(16, 16));
}

//HeadBarPushButton
HeadBarPushButton::HeadBarPushButton(QWidget *parent) : QPushButton(parent)
{
    setIconSize(QSize(16, 16));
}
