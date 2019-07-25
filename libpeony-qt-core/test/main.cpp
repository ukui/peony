#include "testwidget.h"
#include <QApplication>

#include <gio/gio.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TestWidget w;
    w.show();

    return a.exec();
}
