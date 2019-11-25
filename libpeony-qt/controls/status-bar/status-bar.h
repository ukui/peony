#ifndef STATUSBAR_H
#define STATUSBAR_H

#include <QStatusBar>
#include "peony-core_global.h"

class QLabel;
class QToolBar;

namespace Peony {

class FMWindow;

class PEONYCORESHARED_EXPORT StatusBar : public QStatusBar
{
    Q_OBJECT
public:
    explicit StatusBar(FMWindow *window, QWidget *parent = nullptr);
    ~StatusBar() override;

public Q_SLOTS:
    void update();
    void update(const QString &message);

protected:
    void paintEvent(QPaintEvent *e) override;

private:
    FMWindow *m_window = nullptr;

    QLabel *m_label = nullptr;
    QToolBar *m_styled_toolbar = nullptr;
};

}

#endif // STATUSBAR_H
