#ifndef DESKTOPMENU_H
#define DESKTOPMENU_H

#include <QMenu>

namespace Peony {

class DirectoryViewIface;

class DesktopMenu : public QMenu
{
    Q_OBJECT
public:
    explicit DesktopMenu(DirectoryViewIface *view, QWidget *parent = nullptr);

protected:
    void fillActions();
    const QList<QAction *> constructOpenOpActions();
    const QList<QAction *> constructViewOpActions();
    const QList<QAction *> constructFileOpActions();
    const QList<QAction *> constructMenuPluginActions(); //directory view menu extension.
    const QList<QAction *> constructFilePropertiesActions();

    void openWindow(const QString &uri);
    void openWindow(const QStringList &uris);
    void showProperties(const QString &uri);
    void showProperties(const QStringList &uris);

private:
    DirectoryViewIface *m_view;
    QString m_directory;
    QStringList m_selections;
};

}

#endif // DESKTOPMENU_H
