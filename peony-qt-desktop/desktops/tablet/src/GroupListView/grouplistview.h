#ifndef GroupListView_H
#define GroupListView_H
#include <QListView>
#include "src/Interface/ukuimenuinterface.h"
#include "src/GroupListView/grouplistdelegate.h"
#include <QSettings>
#include <QStandardItemModel>
#include <QMouseEvent>
#include <QDrag>

class GroupListView : public QListView
{
    Q_OBJECT
public:
    GroupListView(QWidget *parent);
    ~GroupListView();

   QSettings *setting;
   void addData(QStringList data);
   bool isDraging() const {return IsDraging;}
   int dragRow() const {return theDragRow;}

protected:
   void initWidget();
   void mousePressEvent(QMouseEvent *event) override;
   void mouseReleaseEvent(QMouseEvent *event) override;
   void mouseMoveEvent(QMouseEvent *event) override;
   void dragEnterEvent(QDragEnterEvent *event) override;
   void dragLeaveEvent(QDragLeaveEvent *event) override;
   void dragMoveEvent(QDragMoveEvent *event) override;
   void dropEvent(QDropEvent *event) override;
   void onClicked(QModelIndex index);

   
private:
   GroupListDelegate* m_delegate=nullptr;
   UkuiMenuInterface* pUkuiMenuInterface=nullptr;
   QStandardItemModel* listmodel=nullptr;

   QPoint startPos;
   bool IsDraging = false;
   int theDragRow = -1;


Q_SIGNALS:
    void sendDragLeaveSignal(QString desktopfp);//发送app移出选框的信号
    void sendItemClickedSignal(QString desktopfp);

};

#endif // GroupListView_H
