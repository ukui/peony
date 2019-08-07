#include "mainwindow.h"
#include <QApplication>

#include <QMetaType>
#include "gerror-wrapper.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qRegisterMetaType<Peony::GErrorWrapperPtr>("Peony::GErrorWrapperPtr");
    qRegisterMetaType<Peony::GErrorWrapperPtr>("Peony::GErrorWrapperPtr&");
    MainWindow w;
    w.show();

    return a.exec();
}
