#ifndef NAVIGATIONTOOLBAR_H
#define NAVIGATIONTOOLBAR_H

#include <QToolBar>
#include <QStack>

#include "peony-core_global.h"

namespace Peony {

class PEONYCORESHARED_EXPORT NavigationToolBar : public QToolBar
{
    Q_OBJECT
public:
    explicit NavigationToolBar(QWidget *parent = nullptr);
    bool canGoBack() {return !m_back_stack.isEmpty();}
    bool canGoForward() {return !m_forward_stack.isEmpty();}
    bool canCdUp();

Q_SIGNALS:
    void goBackRequest();
    void goForwardRequest();
    void goToUriRequest(const QString &uri, bool addHistory = false);
    void cdUpRequest();

public Q_SLOTS:
    void onGoBack();
    void onGoForward();
    void onGoToUri(const QString &uri, bool addHistory);

    void clearHistory();

private:
    QString m_current_uri;
    QStack<QString> m_back_stack;
    QStack<QString> m_forward_stack;
};

}

#endif // NAVIGATIONTOOLBAR_H
