#include "mainwindow.h"
#include <QApplication>

#include <QGraphicsDropShadowEffect>

#include <QPushButton>

#include "decorator.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    //Decorator::wrapWindow(&w);
    auto shadowEffects = new QGraphicsDropShadowEffect(&w);
    shadowEffects->setBlurRadius(20);
    shadowEffects->setOffset(0, 0);
    /*!
      \bug
      ukui-style has a bug that make control paint incorrectly
      when a QGraphicsDropShadowEffects is setted.

      to resolve the problem, this bug should be fixed in ukui-style.
      */
    w.setGraphicsEffect(shadowEffects);

    w.resize(800, 600);
    //w.setMask(w.geometry().adjusted(100, 100, -100, -100));
    w.show();

    return a.exec();
}
