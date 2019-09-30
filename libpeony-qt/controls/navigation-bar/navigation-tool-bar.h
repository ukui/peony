#ifndef NAVIGATIONTOOLBAR_H
#define NAVIGATIONTOOLBAR_H

#include <QToolBar>
#include <QStack>

#include "peony-core_global.h"

namespace Peony {

class DirectoryViewContainer;

class PEONYCORESHARED_EXPORT NavigationToolBar : public QToolBar
{
    Q_OBJECT
public:
    explicit NavigationToolBar(QWidget *parent = nullptr);
    bool canGoBack();
    bool canGoForward();
    bool canCdUp();

Q_SIGNALS:
    void updateWindowLocationRequest(const QString &uri, bool addHistory);
    void refreshRequest();

public Q_SLOTS:
    void setCurrentContainer(DirectoryViewContainer *container);

    void updateActions();

    void onGoBack();
    void onGoForward();
    void onGoToUri(const QString &uri, bool addHistory);

    void clearHistory();

private:
    DirectoryViewContainer *m_current_container = nullptr;

    QAction *m_back_action;
    QAction *m_forward_action;
    QAction *m_history_action;
    QAction *m_cd_up_action;
    QAction *m_refresh_action;
};

}

#endif // NAVIGATIONTOOLBAR_H
