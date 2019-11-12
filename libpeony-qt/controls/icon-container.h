#ifndef ICONCONTAINER_H
#define ICONCONTAINER_H

#include <QPushButton>

namespace Peony {

class IconContainer : public QPushButton
{
    Q_OBJECT
public:
    explicit IconContainer(QWidget *parent = nullptr);

protected:
    void mouseMoveEvent(QMouseEvent *e) {}
    void mousePressEvent(QMouseEvent *e) {}
    void paintEvent(QPaintEvent *e);
};

#endif // ICONCONTAINER_H

}
