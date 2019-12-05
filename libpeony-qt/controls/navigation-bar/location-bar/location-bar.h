#ifndef LOCATIONBAR_H
#define LOCATIONBAR_H

#include <QToolBar>
#include "peony-core_global.h"

class QLineEdit;

namespace Peony {

class PEONYCORESHARED_EXPORT LocationBar : public QToolBar
{
    Q_OBJECT
public:
    explicit LocationBar(QWidget *parent = nullptr);
    ~LocationBar() override;
    const QString getCurentUri() {return m_current_uri;}

Q_SIGNALS:
    void groupChangedRequest(const QString &uri);
    void blankClicked();

public Q_SLOTS:
    void setRootUri(const QString &uri);

protected:
    void addButton(const QString &uri, bool setIcon = false, bool setMenu = true);

    void mousePressEvent(QMouseEvent *e) override;
    void paintEvent(QPaintEvent *e) override;

private:
    QString m_current_uri;
    QLineEdit *m_styled_edit;
};

}

#endif // LOCATIONBAR_H
