#include "tablet-mode.h"

#include <QDebug>
#include <QMouseEvent>
#include <QPropertyAnimation>
#include <QVBoxLayout>
#include <QPushButton>

using namespace Peony;

TabletMode::TabletMode(QWidget *parent) : DesktopWidgetBase(parent)
{
    QVBoxLayout *boxLayout = new QVBoxLayout(this);

    this->setLayout(boxLayout);

    boxLayout->setAlignment(Qt::AlignCenter);

    QPushButton *button = new QPushButton("button", this);
    QPushButton *button1 = new QPushButton("button1", this);
    QPushButton *button2 = new QPushButton("button2", this);
    QPushButton *button3 = new QPushButton("button3", this);
    QPushButton *button4 = new QPushButton("button4", this);

    boxLayout->addWidget(button);
    boxLayout->addWidget(button1);
    boxLayout->addWidget(button2);
    boxLayout->addWidget(button3);
    boxLayout->addWidget(button4);

}

TabletMode::~TabletMode()
{

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
    if (event->button() == Qt::LeftButton) {
        m_leftKeyPressed = false;
        m_endPoint = event->globalPos();
        qint64 moveWidth = m_endPoint.x() - m_startPoint.x();
        quint64 minWidth = geometry().width() * 0.3;
        qDebug() << "鼠标抬起事件：" << m_endPoint << "滑动距离：" << moveWidth << "minWidth:" << minWidth << "geometry:" << geometry();

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
