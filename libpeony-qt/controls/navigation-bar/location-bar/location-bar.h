#ifndef LOCATIONBAR_H
#define LOCATIONBAR_H

#include <QToolBar>
#include "peony-core_global.h"

namespace Peony {

class PEONYCORESHARED_EXPORT LocationBar : public QToolBar
{
    Q_OBJECT
public:
    explicit LocationBar(QWidget *parent = nullptr);
    const QString getCurentUri() {return m_current_uri;}

Q_SIGNALS:
    void groupChangedRequest(const QString &uri);
    void blankClicked();

public Q_SLOTS:
    void setRootUri(const QString &uri);

protected:
    void addButton(const QString &uri, bool setMenu = true);

    void mousePressEvent(QMouseEvent *e) override;

private:
    QString m_current_uri;
};

}

#endif // LOCATIONBAR_H
