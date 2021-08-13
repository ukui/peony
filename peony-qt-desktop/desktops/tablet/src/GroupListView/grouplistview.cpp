#include "grouplistview.h"
#include "src/UtilityFunction/thumbnail.h"
#include <QDrag>
#include <QListWidgetItem>


GroupListView::GroupListView(QWidget *parent):
    QListView(parent)
{

    initWidget();

    QString path=QDir::homePath()+"/.config/ukui/ukui-menu-panel.ini";
    setting=new QSettings(path,QSettings::IniFormat);
    setDragEnabled(true);
}

GroupListView::~GroupListView()
{

}

void GroupListView::initWidget()
{
    this->setSelectionMode(QAbstractItemView::SingleSelection);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setAttribute(Qt::WA_AcceptDrops);
    this->setViewMode(QListView::IconMode);
    this->viewport()->setUpdatesEnabled(true);
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    this->setResizeMode(QListView::Adjust);
    this->setTextElideMode(Qt::ElideRight);
    this->setMouseTracking(true);
    this->setFocusPolicy(Qt::NoFocus);
    this->setMovement(QListView::Snap);
    this->setGridSize(QSize(Style::AppListItemSizeWidth,Style::AppListItemSizeHeight));

}

void GroupListView::addData(QStringList data)
{
    listmodel=new QStandardItemModel(this);
    this->setModel(listmodel);
    Q_FOREACH(QString desktopfp,data)
    {
        QStandardItem* item=new QStandardItem;
        item->setData(QVariant::fromValue<QString>(desktopfp),Qt::DisplayRole);
        listmodel->appendRow(item);
    }
    m_delegate= new GroupListDelegate(this);
    this->setItemDelegate(m_delegate);

}

void GroupListView::mousePressEvent(QMouseEvent *event)
{
    if(event->button()==Qt::LeftButton){
            startPos = event->pos();

    }

}

void GroupListView::mouseReleaseEvent(QMouseEvent *event)
{

    QString desktopfp=this->indexAt(event->pos()).data(Qt::DisplayRole).value<QString>();
    qDebug() << desktopfp << "释放了！！！";



}


void GroupListView::mouseMoveEvent(QMouseEvent *event)
{

    if(event->buttons() & Qt::LeftButton){
            if((event->pos() - startPos).manhattanLength() < QApplication::startDragDistance()) return;

            QModelIndex theDragIndex = indexAt(startPos);
            theDragRow = theDragIndex.row();
            qDebug() << theDragRow << "-------------";


            setCurrentIndex(theDragIndex);  //拖拽即选中

    //[1]把拖拽数据放在QMimeData容器中

            QString desktopfp=this->indexAt(event->pos()).data(Qt::DisplayRole).value<QString>();
            QByteArray itemData=desktopfp.toLocal8Bit();;

            QMimeData *mimeData = new QMimeData;
            mimeData->setData("INFO", itemData);

    //[2]设置拖拽时的缩略图
            ThumbNail *DragImage = new ThumbNail(this);
            DragImage->setupthumbnail(desktopfp);
            QPixmap pixmap = DragImage->grab();

            QDrag *drag = new QDrag(this);
            drag->setMimeData(mimeData);
            drag->setPixmap(pixmap);
            drag->setHotSpot(QPoint(pixmap.width() / 2, pixmap.height() / 2));

      //删除的行需要根据theInsertRow和theDragRow的大小关系来判断(这个也是我根据实际情况测试发现的)
            if(drag->exec(Qt::MoveAction) == Qt::MoveAction){

            }
       }

}


void GroupListView::dragEnterEvent(QDragEnterEvent *event)
{

    GroupListView *source = qobject_cast<GroupListView *>(event->source());
    if (source && source == this) {
        //IsDraging(标志位)判断是否正在拖拽
        IsDraging = true;
        event->setDropAction(Qt::MoveAction);
        event->accept();
    }
}

void GroupListView::dragLeaveEvent(QDragLeaveEvent *event)
{

    IsDraging = false;  //IsDraging(标志位)判断是否正在拖拽
    QString desktopfp=listmodel->item(theDragRow)->index().data(Qt::DisplayRole).value<QString>();

    qDebug()<< "release the item" << desktopfp;


    Q_EMIT sendDragLeaveSignal(desktopfp);

}

void GroupListView::dragMoveEvent(QDragMoveEvent *event)
{
    qDebug() << "-----" << "dragMoveEvent" << "-----";
    //拖拽特效绘制





}

void GroupListView::dropEvent(QDropEvent *event)
{
    qDebug() << "-----" << "dropEvent" << "-----";
    GroupListView *source = qobject_cast<GroupListView *>(event->source());
    if (source && source == this){

        IsDraging = false;  //IsDraging(标志位)判断是否正在拖拽


    }

}

void GroupListView::onClicked(QModelIndex index)
{
         QVariant var = listmodel->data(index, Qt::DisplayRole);

         if(var.isValid())
         {
             QString desktopfp=var.value<QString>();
             QFileInfo fileInfo(desktopfp);
             QString desktopfn=fileInfo.fileName();

             Q_EMIT sendItemClickedSignal(desktopfp);
         }
}

