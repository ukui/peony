#ifndef DECORATOR_H
#define DECORATOR_H

#include <QObject>

class Decorator : public QObject
{
    Q_OBJECT
public:
    static void wrapWindow(QWidget *w);

    explicit Decorator(QObject *parent = nullptr);
};

#endif // DECORATOR_H
