#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QToolButton>
#include <QIcon>

#include <QPushButton>
#include <QHBoxLayout>
#include <QWidget>

#include <QPainter>
#include <QPainterPath>

#include <QMoveEvent>

#include "controls/navigation-bar/advanced-location-bar.h"

#include <QtWidgets/private/qwidgetresizehandler_p.h>

#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    installEventFilter(this);

    /*!
      \todo
      make window be recognized as csd window in all window managers.
      .
      In kwin, it also detect _GTK_FRAME_EXTENT atom, and asume it as csd window.
      However it seems that there is no effect for qt window, and maybe
      I have to try implementing a KDecorator for client side decorations in kwin.
      */
    //Even though I have used Decorator tell window manager decorate border only,
    //some window manager, such as kwin still add titlebar to the widget. For now
    //I have to add FramelessWindowHint first to tell it realy doesn't need titlebar.
    //Then i will manually set it border with Decorator.
    //This hint is useless for window manager know how to response my request, such
    //as ukwm.
    setWindowFlag(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_Hover);
    setAttribute(Qt::WA_TranslucentBackground);
    //setMouseTracking(true);
    //setAutoFillBackground(true);
    //setForegroundRole(QPalette::Window);

    setContentsMargins(10, 10, 10, 10);

    //setContentsMargins(4, 4, 4, 4);
    //use qt private api resize the widget.
    auto handler = new QWidgetResizeHandler(this);
    Q_UNUSED(handler)
    //handler->setActive(QWidgetResizeHandler::Resize, false);

    setMinimumSize(600, 480);

    this->setAutoFillBackground(false);

    //title bar
    ui->setupUi(this);
    ui->mainToolBar->setFixedHeight(50);
    ui->mainToolBar->setAttribute(Qt::WA_TranslucentBackground);
    ui->mainToolBar->setStyleSheet("QToolBar#mainToolBar{background-color: transparent;}");
    auto a = ui->mainToolBar->addAction(QIcon::fromTheme("folder-new-symbolic"), tr("Create New Folder"));
    auto b = qobject_cast<QToolButton*>(ui->mainToolBar->widgetForAction(a));
    b->setAutoRaise(false);
    b->setFixedSize(40, 40);

    a = ui->mainToolBar->addAction(QIcon::fromTheme("terminal"), tr("Open in Terminal"));
    b = qobject_cast<QToolButton*>(ui->mainToolBar->widgetForAction(a));
    b->setAutoRaise(false);
    b->setFixedSize(40, 40);

    ui->mainToolBar->addSeparator();

    auto hlayout = new QHBoxLayout();
    auto tmp = new QWidget(this);
    tmp->setFocusPolicy(Qt::NoFocus);
    tmp->setLayout(hlayout);
    hlayout->setMargin(0);
    hlayout->setSpacing(0);

    auto p = new QToolButton(this);
    p->setAutoRaise(true);
    p->setIcon(QIcon::fromTheme("go-previous"));
    p->setFixedSize(QSize(36, 28));
    p->setToolTip(tr("Go Back"));
    p->setIconSize(QSize(16, 16));
    hlayout->addWidget(p);

    p = new QToolButton(this);
    p->setAutoRaise(true);
    p->setIcon(QIcon::fromTheme("go-next"));
    p->setFixedSize(QSize(36, 28));
    p->setToolTip(tr("Go Forward"));
    p->setIconSize(QSize(16, 16));
    hlayout->addWidget(p);

    ui->mainToolBar->addWidget(tmp);

    ui->mainToolBar->addSeparator();
    auto bar = new Peony::AdvancedLocationBar(this);
    bar->updateLocation("file:///etc");
    ui->mainToolBar->addWidget(bar);
    ui->mainToolBar->addSeparator();

    a = ui->mainToolBar->addAction(QIcon::fromTheme("search-symbolic"), tr("Find"));
    b = qobject_cast<QToolButton*>(ui->mainToolBar->widgetForAction(a));
    b->setAutoRaise(false);
    b->setFixedSize(40, 40);

    ui->mainToolBar->addSeparator();

    a = ui->mainToolBar->addAction(QIcon::fromTheme("view-grid-symbolic"), tr("View"));
    b = qobject_cast<QToolButton*>(ui->mainToolBar->widgetForAction(a));
    b->setAutoRaise(false);
    b->setFixedWidth(57);
    b->setFixedHeight(40);
    b->setPopupMode(QToolButton::MenuButtonPopup);

    a = ui->mainToolBar->addAction(QIcon::fromTheme("view-sort-ascending-symbolic"), tr("View"));
    b = qobject_cast<QToolButton*>(ui->mainToolBar->widgetForAction(a));
    b->setAutoRaise(false);
    b->setFixedWidth(57);
    b->setFixedHeight(40);
    b->setPopupMode(QToolButton::MenuButtonPopup);

    a = ui->mainToolBar->addAction(QIcon::fromTheme("open-menu-symbolic"), tr("Menu"));
    b = qobject_cast<QToolButton*>(ui->mainToolBar->widgetForAction(a));
    b->setAutoRaise(false);
    b->setFixedSize(40, 40);

    a = ui->mainToolBar->addAction(QIcon::fromTheme("window-minimize-symbolic"), tr("Menu"));
    b = qobject_cast<QToolButton*>(ui->mainToolBar->widgetForAction(a));
    b->setAutoRaise(false);
    b->setFixedSize(40, 40);

    a = ui->mainToolBar->addAction(QIcon::fromTheme("window-maximize-symbolic"), tr("Menu"));
    b = qobject_cast<QToolButton*>(ui->mainToolBar->widgetForAction(a));
    b->setAutoRaise(false);
    b->setFixedSize(40, 40);

    a = ui->mainToolBar->addAction(QIcon::fromTheme("window-close-symbolic"), tr("Menu"));
    b = qobject_cast<QToolButton*>(ui->mainToolBar->widgetForAction(a));
    b->setAutoRaise(false);
    b->setFixedSize(40, 40);
    //title bar

    //ui->centralWidget->setAutoFillBackground(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::eventFilter(QObject *obj, QEvent *e)
{
    //qDebug()<<e->type();
    if (e->type() == QEvent::LayoutRequest) {
        QPainterPath path = QPainterPath();
        auto rect = this->rect();
        if (!this->isMaximized()) {
            rect.adjust(10, 10, -10, -10);
        }
        if (!this->isMaximized()) {
            path.addRoundedRect(rect, 6, 6);
        } else {
            path.addRect(rect);
        }
        this->setProperty("blurRegion", QRegion(path.toFillPolygon().toPolygon()));
    }
    if (e->type() == QEvent::Resize) {
        qDebug()<<"resize";
    }
    if (e->type() == QEvent::WindowStateChange) {
        /*!
          \bug
          this event doesn't contain the half screen maximum situation.
          */
        qDebug()<<"window state change"<<e;
        bool ret = QMainWindow::eventFilter(obj, e);
        if (this->isMaximized()) {
            qDebug()<<"maximized";
            setContentsMargins(0, 0, 0, 0);
        } else {
            setContentsMargins(10, 10, 10, 10);
        }
        return ret;
    }
    if (e->type() == QEvent::Resize) {
        //qDebug()<<"resize";
    }

    return QMainWindow::eventFilter(obj, e);
}

void MainWindow::paintEvent(QPaintEvent *e)
{
    auto rect = this->rect();
    if (!this->isMaximized()) {
        rect.adjust(10, 10, -10, -10);
    }
    QPainter p(this);
    p.setRenderHints(QPainter::Antialiasing);
    p.setPen(Qt::NoPen);
    QPainterPath path = QPainterPath();

    if (!this->isMaximized()) {
        path.addRoundedRect(rect, 6, 6);
    } else {
        path.addRect(rect);
    }

    auto centerRect = this->ui->centralWidget->rect();
    centerRect.moveTo(this->ui->centralWidget->mapToParent(QPoint()));
    auto otherPath = QPainterPath();
    otherPath.addRect(centerRect.adjusted(1, -1, -1, -1));
    path -= otherPath;
    p.setClipPath(path);
    p.fillPath(path, this->palette().window());

    QMainWindow::paintEvent(e);
}
