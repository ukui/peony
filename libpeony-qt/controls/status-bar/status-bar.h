#ifndef STATUSBAR_H
#define STATUSBAR_H

#include <QStatusBar>
#include "peony-core_global.h"

class QLabel;

namespace Peony {

class FMWindow;

class PEONYCORESHARED_EXPORT StatusBar : public QStatusBar
{
    Q_OBJECT
public:
    explicit StatusBar(FMWindow *window, QWidget *parent = nullptr);

public Q_SLOTS:
    void update();
    void update(const QString &message);

protected:
    void paintEvent(QPaintEvent *e) override;

private:
    FMWindow *m_window = nullptr;

    QLabel *m_label = nullptr;
};

}

#endif // STATUSBAR_H
