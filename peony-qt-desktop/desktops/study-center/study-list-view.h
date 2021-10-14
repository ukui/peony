#ifndef STUDYLISTVIEW_H
#define STUDYLISTVIEW_H

#include <QObject>
#include <QListView>
#include <QWidget>

#include "../../tablet/data/tablet-app-manager.h"
#include "progress-item-delegate.h"

class QMouseEvent;
class QStandardItemModel;
namespace Peony {

class StudyListView : public QListView
{
    Q_OBJECT
public:
    explicit StudyListView(QWidget *parent = nullptr);
    ~StudyListView(){};

    void setData(QList<TabletAppEntity*> tabletAppList);
private:
    QStandardItemModel *m_animation = nullptr;
    //QStandardItemModel* listmodel=nullptr;

protected:
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    int horizontalOffset() const override;

private:
    QStandardItemModel* m_listmodel=nullptr;
    bool m_iconClicked = false;
    QPoint m_pressedPos;
};

}

#endif // STUDYLISTVIEW_H
