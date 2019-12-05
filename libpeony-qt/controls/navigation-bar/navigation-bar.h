#ifndef NAVIGATIONBAR_H
#define NAVIGATIONBAR_H

#include <QToolBar>
#include "peony-core_global.h"

class QMenu;
class QActionGroup;

namespace Peony {

class DirectoryViewContainer;

class AdvancedLocationBar;
class NavigationToolBar;

class PEONYCORESHARED_EXPORT NavigationBar : public QToolBar
{
    Q_OBJECT
public:
    explicit NavigationBar(QWidget *parent = nullptr);
    ~NavigationBar();

    bool isPathEditing();
    const QString getLastPreviewPageId();

Q_SIGNALS:
    void updateWindowLocationRequest(const QString &uri, bool addHistory, bool forceUpdate = false);
    void refreshRequest();

    void switchPreviewPageRequest(const QString &id);

public Q_SLOTS:
    void bindContainer(DirectoryViewContainer *container);
    void updateLocation(const QString &uri);

    void setBlock(bool block = true);
    void startEdit();
    void finishEdit();

    void triggerAction(const QString &id);

protected:
    const QString getCurrentUri();

    void paintEvent(QPaintEvent *e);

private:
    NavigationToolBar *m_left_control;
    AdvancedLocationBar *m_center_control;
    QActionGroup *m_group;

    QAction *m_checked_preview_action;
    QString m_last_preview_page_id_in_window = nullptr;
};

}

#endif // NAVIGATIONBAR_H
