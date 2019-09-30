#ifndef ADVANCEDLOCATIONBAR_H
#define ADVANCEDLOCATIONBAR_H

#include <QWidget>
#include "peony-core_global.h"

class QStackedLayout;

namespace Peony {

class LocationBar;
class PathEdit;

class PEONYCORESHARED_EXPORT AdvancedLocationBar : public QWidget
{
    Q_OBJECT
public:
    explicit AdvancedLocationBar(QWidget *parent = nullptr);

Q_SIGNALS:
    void updateWindowLocationRequest(const QString &uri, bool addHistory = true);

public Q_SLOTS:
    void updateLocation(const QString &uri);

private:
    QStackedLayout *m_layout;

    LocationBar *m_bar;
    PathEdit *m_edit;
};

}

#endif // ADVANCEDLOCATIONBAR_H
