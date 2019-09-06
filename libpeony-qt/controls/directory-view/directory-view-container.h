#ifndef DIRECTORYVIEWCONTAINER_H
#define DIRECTORYVIEWCONTAINER_H

#include "peony-core_global.h"
#include <QWidget>
#include <QStack>

namespace Peony {

class DirectoryViewProxyIface;

/*!
 * \brief The DirectoryViewContainer class
 * \details
 * This class define a set of higher level operation for the directory view,
 * such as history functions and view switch.
 * In peony-qt, tab-window and view-type-switch are reserved from peony.
 * This class is mainly to archive these higher ui logic.
 *
 * A directory view can not swith it type itself because it was created
 * from its own factory, and it aslo could and should not contain more details
 * implement above the view it self. Those should be implement by a wrapper class
 * in higher level.
 * You can interpret it as the middleware of window and directory view.
 */
class DirectoryViewContainer : public QWidget
{
    Q_OBJECT
public:
    explicit DirectoryViewContainer(QWidget *parent = nullptr);
    ~DirectoryViewContainer();

    bool canGoBack();
    bool canGoForward();
    bool canCdUp();

Q_SIGNALS:
    void viewTypeChanged();
    void directoryChanged();

public Q_SLOTS:
    void setDirectoryUri(const QString &uri, bool addHistory);
    void switchViewType(const QString &viewId);

    void goBack();
    void goForward();
    void cdUp();

    void refresh();

protected:
    void bindNewProxy(DirectoryViewProxyIface *proxy);

private:
    DirectoryViewProxyIface *m_active_view_prxoy = nullptr;
    QList<DirectoryViewProxyIface *> m_proxys;

    QStack<QString> m_back_stack;
    QStack<QString> m_forward_stack;
};

}

#endif // DIRECTORYVIEWCONTAINER_H
