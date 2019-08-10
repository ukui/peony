#include "mainwindow.h"
#include <QApplication>

#include <QMetaType>
#include "gerror-wrapper.h"

#include <QIcon>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QIcon::setThemeName("ukui-icon-theme");
    qRegisterMetaType<Peony::GErrorWrapperPtr>("Peony::GErrorWrapperPtr");
    qRegisterMetaType<Peony::GErrorWrapperPtr>("Peony::GErrorWrapperPtr&");
    MainWindow w;
    w.show();

    return a.exec();
}
