#ifndef STUDYLISTVIEW_H
#define STUDYLISTVIEW_H

#include <QObject>
#include <QListView>
#include <QWidget>

#include "../../tablet/data/tablet-app-manager.h"

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
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseDoubleClickEvent(QMouseEvent *event);

private:
    QStandardItemModel* m_listmodel=nullptr;
};

}

#endif // STUDYLISTVIEW_H
