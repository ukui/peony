#ifndef DIRECTORYVIEWPLUGINIFACE_H
#define DIRECTORYVIEWPLUGINIFACE_H

#include "plugin-iface.h"

#include <QString>

#define DirectoryViewPluginIface_iid "org.ukui.peony-qt.plugin-iface.DirectoryViewPluginInterface"

namespace Peony {

class DirectoryViewIface;
class DirectoryViewProxyIface;

/*!
 * \brief The DirectoryViewPluginIface class
 * \details
 * It is a few complex to implement a peony-qt's directory view's plugin.
 * There are 3 interface class must to be implemented.
 * 1. DirectoryViewPluginIface:
 * This is the entry of a plugin, peony-qt's application will dynamicly
 * load external view modules when it started. After a plugin loaded
 * it will be registered in peony-qt's application globally, then we can
 * create such a view by create();
 * \see DirectoryViewFactoryManager
 *
 * 2. DirectoryViewIface:
 * This class define a set of methods a dervied class of directory view
 * must implement. a dervied view could most be a dervied QAbstractItemView,
 * such as QListView or QTreeView, but not certain.
 * the directory view must implement these virtual method and make them
 * seem worked in there own implement.
 * \see DirectoryViewIface
 *
 * 3. DirectoryViewProxyIface
 * \see DirectoryViewProxyIface
 */
class DirectoryViewPluginIface : public PluginInterface
{
public:
    virtual QString viewIdentity() = 0;
    virtual QIcon viewIcon() = 0;
    virtual bool supportUri(const QString &uri) = 0;

    virtual DirectoryViewIface *create() = 0;
};

/*!
 * \brief The DirectoryViewIface class
 * \details
 * This class define a set of interface for view operation.
 * In peony-qt the directory view is desgin to be extensive,
 * and every kind of view will have a global factory to create
 * the view's instance. the factories will be managed by peony-qt
 * in global, too.
 * A peony-qt's file item view will implement this interface.
 * \note
 * Every DirectoryViewIface instance should have a DirectoryViewProxyIface
 * instance, we usually use this proxy control the view.
 * \see DirectoryViewProxyIface
 */
class DirectoryViewIface
{

public:
    virtual ~DirectoryViewIface() {}

    //location
    const virtual QString getDirectoryUri() = 0;

    //selections
    virtual QStringList getSelections() = 0;

    /*
    //loaction
    void openRequest(const QStringList &uri, bool newWindow);
    void viewDirectoryChanged();
    void viewSelectionChanged();

    //menu
    void menuRequest(const QPoint &pos);

    //clipboard
    void cutSelectionsRequest();
    void copySelectionsRequest();
    void pasteSelectionsRequest();

    //zoom
    void zoomedIn();
    void zoomedOut();
    */

    //location
    virtual void open(const QStringList &uris, bool newWindow) = 0;
    virtual void setDirectoryUri(const QString &uri) = 0;
    virtual void beginLocationChange() = 0;
    virtual void stopLocationChange() = 0;
    virtual void close() = 0;

    //selections
    virtual void setSelections(const QStringList &uris) = 0;
    virtual void invertSelections() = 0;
    virtual void scrollToSelection(const QString &uri) = 0;

    //clipboard
    virtual void setCutFiles(const QStringList &uris) = 0;

    virtual DirectoryViewProxyIface *getProxy() = 0;
};

/*!
 * \brief The DirectoryViewProxyIface class
 * \details
 * Unfortunately, Qt doesn't allow us dervied from over one QObject based classes.
 * This means we have to define our signal and slot interface outside the DirectoryViewIface.
 * This class define the signal and slot of a DirectoryViewIface should have.
 * Except the view proxy method, this class also define some higher level
 * function above the view class, such as history and clipboard.
 *
 * \note
 * You must bind a DirectoryViewIface(dervied) instance in your dervied class instance.
 * In libpeony-qt, there have been a proxy has been implemented,
 * which are used in icon view and list view by default.
 *
 * \see DirectoryViewIface, StandardViewProxy, IconView, ListView.
 */
class DirectoryViewProxyIface: public QObject
{
    Q_OBJECT
public:
    explicit DirectoryViewProxyIface(QObject *parent = nullptr) : QObject(parent) {}
    ~DirectoryViewProxyIface() {}

    virtual void switchView(DirectoryViewIface *view) = 0;

    virtual DirectoryViewIface *getView() = 0;

    //location
    const virtual QString getDirectoryUri() = 0; 

    //selections
    virtual QStringList getSelections() = 0;

Q_SIGNALS:
    //loaction
    //FIXME: support open in new TAB?
    void openRequest(const QStringList &uri, bool newWindow);
    void viewDoubleClicked(const QString &uri);
    void viewDirectoryChanged();
    void viewSelectionChanged();

    //menu
    void menuRequest(const QPoint &pos);

public Q_SLOTS:
    //location
    virtual void open(const QStringList &uris, bool newWindow) = 0;
    virtual void setDirectoryUri(const QString &uri) = 0;
    virtual void beginLocationChange() = 0;
    virtual void stopLocationChange() = 0;

    virtual void close() = 0;

    //selections
    virtual void setSelections(const QStringList &uris) = 0;
    virtual void invertSelections() = 0;
    virtual void scrollToSelection(const QString &uri) = 0;

    //clipboard
    //cut items should be drawn differently.
    virtual void setCutFiles(const QStringList &uris) = 0;
};

}

Q_DECLARE_INTERFACE(Peony::DirectoryViewPluginIface, DirectoryViewPluginIface_iid)

#endif // DIRECTORYVIEWPLUGINIFACE_H
