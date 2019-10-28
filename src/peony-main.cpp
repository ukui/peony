#include "peony-application.h"

int main(int argc, char *argv[]) {
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    PeonyApplication app(argc, argv, "peony-qt-single-app-test");
    if (app.isSecondary())
        return 0;

    return app.exec();
}
