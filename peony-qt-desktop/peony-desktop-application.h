#ifndef PEONYDESKTOPAPPLICATION_H
#define PEONYDESKTOPAPPLICATION_H


#include "singleapplication.h"

class PeonyDesktopApplication : public SingleApplication
{
    Q_OBJECT
public:
    explicit PeonyDesktopApplication(int &argc, char *argv[], const char *applicationName = "peony-qt-desktop");

protected Q_SLOTS:
    void parseCmd(quint32 id, QByteArray msg);
};

#endif // PEONYDESKTOPAPPLICATION_H
