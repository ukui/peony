#include "mainwindow.h"
#include <QApplication>

#include "decorator.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    Decorator::wrapWindow(&w);
    w.resize(800, 600);
    //w.setMask(w.geometry().adjusted(100, 100, -100, -100));
    w.show();

    return a.exec();
}
