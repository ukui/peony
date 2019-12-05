#ifndef SIDEBAR_H
#define SIDEBAR_H

#include <QTreeView>
#include "peony-core_global.h"

namespace Peony {

class SideBarDelegate;

class PEONYCORESHARED_EXPORT SideBar : public QTreeView
{
    friend class SideBarDelegate;
    Q_OBJECT
public:
    explicit SideBar(QWidget *parent = nullptr);

    QSize sizeHint() const override;

Q_SIGNALS:
    void updateWindowLocationRequest(const QString &uri, bool addHistory = true, bool forceUpdate = false);

protected:
    void paintEvent(QPaintEvent *e) override;
    QRect visualRect(const QModelIndex &index) const override;
    //int horizontalOffset() const override {return 100;}

    void dragEnterEvent(QDragEnterEvent *e) override;
    void dragMoveEvent(QDragMoveEvent *e) override;
};

}

#endif // SIDEBAR_H
