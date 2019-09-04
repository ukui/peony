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
    bool canBack() override;
    bool canForward() override;

    //selections
    QStringList getSelections() override;

    //clipboard
    QStringList getClipboardSelections() override;
    bool getClipboardSelectionsIsCut() override;

    //zoom
    bool canZoomIn() override;
    bool canZoomOut() override;

public Q_SLOTS:
    //location
    void open(const QStringList &uris, bool newWindow) override;
    void setDirectoryUri(const QString &uri, bool addHistory) override;
    void beginLocationChange() override;
    void stopLocationChange() override;
    void goBack() override;
    void goForward() override;
    void close() override;

    //selections
    void setSelections(const QStringList &uris) override;
    void invertSelections() override;
    void scrollToSelection(const QString &uri) override;

    //clipboard
    void cutSelections() override;
    void copySelections() override;
    void pasteSelections() override;

    //zoom
    void zoomIn() override;
    void zoomOut() override;

private:
    DirectoryViewIface *m_view = nullptr;

    //should i provide public method operate these stacks?
    QStack<QString> m_back_stack;
    QStack<QString> m_forward_statck;
};

}

}

#endif // STANDARDVIEWPROXY_H
