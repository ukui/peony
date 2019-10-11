#include "peony-application.h"

int main(int argc, char *argv[]) {
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    PeonyApplication app(argc, argv);

    return app.exec();
}
