#ifndef STANDARDVIEWPROXY_H
#define STANDARDVIEWPROXY_H

#include <QStack>
#include "directory-view-plugin-iface.h"

namespace Peony {

namespace DirectoryView {

/*!
 * \brief The StandardViewProxy class
 * \details
 * this class implement a part of methods of DirectoryViewProxyIface.
 * This class is the peony-qt's icon view and list view proxy interface's
 * implement.
 * \note
 * If you want to write a directory view plugin for peony-qt,
 * you can use this proxy class as your plugin-view's proxy. It
 * is recommend, otherwise you should spend a while to re-write a new
 * proxy by yourself.
 */
class StandardViewProxy : public DirectoryViewProxyIface
{
    Q_OBJECT
public:
    explicit StandardViewProxy(DirectoryViewIface *view, QObject *parent = nullptr);
    ~StandardViewProxy() override;

    DirectoryViewIface *getView() override {return m_view;}

    //location
    const QString getDirectoryUri() override;

    //selections
    QStringList getSelections() override;

public Q_SLOTS:
    //location
    void open(const QStringList &uris, bool newWindow) override;
    void setDirectoryUri(const QString &uri) override;
    void beginLocationChange() override;
    void stopLocationChange() override;

    void close() override;

    //selections
    void setSelections(const QStringList &uris) override;
    void invertSelections() override;
    void scrollToSelection(const QString &uri) override;

    //clipboard
    void setCutFiles(const QStringList &uris) override;

private:
    DirectoryViewIface *m_view = nullptr;
};

}

}

#endif // STANDARDVIEWPROXY_H
