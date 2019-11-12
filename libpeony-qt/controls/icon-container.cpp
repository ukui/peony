#include "icon-container.h"
#include <QPaintEvent>
#include <QPainter>

using namespace Peony;

IconContainer::IconContainer(QWidget *parent) : QPushButton(parent)
{
    setEnabled(true);
    setCheckable(false);
    setDefault(true);
    setFlat(true);
}

void IconContainer::paintEvent(QPaintEvent *e)
{
    QPainter p(this);
    p.fillRect(this->rect(), this->palette().base());
    p.setPen(this->palette().dark().color());
    p.drawRect(this->rect().adjusted(0, 0, -1, -1));
    QPushButton::paintEvent(e);
}
