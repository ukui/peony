#ifndef PEONYDESKTOPAPPLICATION_H
#define PEONYDESKTOPAPPLICATION_H


#include "singleapplication.h"
#include "desktop-window.h"

#include <QScreen>
#include <QWindow>

class PeonyDesktopApplication : public SingleApplication
{
    Q_OBJECT
public:
    explicit PeonyDesktopApplication(int &argc, char *argv[], const char *applicationName = "peony-qt-desktop");

protected Q_SLOTS:
    void parseCmd(quint32 id, QByteArray msg, bool isPrimary);
    bool isPrimaryScreen(QScreen *screen);

public Q_SLOTS:
    void layoutDirectionChangedProcess(Qt::LayoutDirection direction);
    void primaryScreenChangedProcess(QScreen *screen);
    void screenAddedProcess(QScreen *screen);
    void screenRemovedProcess(QScreen *screen);

    void addWindow(QScreen *screen, bool checkPrimay = true);
    void changeBgProcess(const QString& bgPath);

private:
    bool m_first_parse = true;
    QList<Peony::DesktopWindow*> m_window_list;
};

#endif // PEONYDESKTOPAPPLICATION_H
