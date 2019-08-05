#include "mainwindow.h"
#include <QApplication>
//#include "gobject-template.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    /*
    {
        Peony::GFileWrapper file = Peony::wrapGFile(g_file_new_for_uri("file:///"));
        file.reset();
        qDebug()<<"new value";
        file = Peony::wrapGFile(g_file_new_for_uri("file:///"));
    }
    */

    return a.exec();
}
