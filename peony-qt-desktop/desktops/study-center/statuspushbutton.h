#ifndef STATUSPUSHBUTTON_H
#define STATUSPUSHBUTTON_H
#include <QPushButton>


class StatusPushButton : public QPushButton
{
    Q_OBJECT
public:
    StatusPushButton(QWidget *parent);
    ~StatusPushButton();
    void paintEvent(QPaintEvent *);
    void setValue(double value);
private:
    double m_val = 0.0;
};

#endif // STATUSPUSHBUTTON_H
