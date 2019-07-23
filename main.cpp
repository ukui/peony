#include <QApplication>

#include "testwidget.h"
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TestWidget w;
    w.show();

    //a.setQuitOnLastWindowClosed(false);

    return a.exec();
}
