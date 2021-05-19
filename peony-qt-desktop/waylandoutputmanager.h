#ifndef WAYLANDOUTPUTMANAGER_H
#define WAYLANDOUTPUTMANAGER_H

#include <QObject>

#include <KWayland/Client/xdgoutput.h>
#include <KWayland/Client/output.h>
#include <QMap>

class QSocketNotifier;
class QTimeLine;

namespace KWayland {
namespace Client {
class Registry;
}
}

class WaylandOutputManager : public QObject
{
    Q_OBJECT
public:
    explicit WaylandOutputManager(QObject *parent = nullptr);

    QList<KWayland::Client::Output *> outputs();
    QList<KWayland::Client::XdgOutput *> xdgOutputs();

Q_SIGNALS:
    void outputAdded(KWayland::Client::Output *output);
    void xdgOutputsAdded(KWayland::Client::XdgOutput *xdgOutput);

public Q_SLOTS:
    void run();
    void setUKUIOutputEnable(bool enable);

protected:
    void addXdgOutput(KWayland::Client::Output *output);

private Q_SLOTS:
    void setUKUIOutputEnableInternal();

private:
    struct wl_display *m_display = nullptr;
    int m_fd = -1;
    KWayland::Client::Registry *m_registry = nullptr;
    KWayland::Client::EventQueue *m_eventQueue = nullptr;

    KWayland::Client::XdgOutputManager *m_xdgOutputManager = nullptr;

    QMap<KWayland::Client::Output *, KWayland::Client::XdgOutput *> m_outputs;

    QSocketNotifier *m_socketNotifier = nullptr;

    struct ukui_output *m_ukuiOutput = nullptr;
    QTimeLine *m_timeLine = nullptr;
};

#endif // WAYLANDOUTPUTMANAGER_H
