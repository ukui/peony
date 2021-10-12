#ifndef STATUSPUSHBUTTON_H
#define STATUSPUSHBUTTON_H
#include <QPushButton>
#include <QPixmap>

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
    QPixmap disc;

public Q_SLOTS:
    void changeTheme(QString);
};

#endif // STATUSPUSHBUTTON_H
