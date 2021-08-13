#include "grouplistdelegate.h"
#include <QDebug>


GroupListDelegate::GroupListDelegate(QObject *parent):QStyledItemDelegate(parent)
{
    pUkuiMenuInterface=new UkuiMenuInterface;

}

GroupListDelegate::~GroupListDelegate()
{
    delete pUkuiMenuInterface;
}

void GroupListDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(index.isValid())
    {
        painter->save();
        QStyleOptionViewItem viewOption(option);//用来在视图中画一个item
        QRectF rect;
        rect.setX(option.rect.x());
        rect.setY(option.rect.y());
        rect.setWidth(option.rect.width());
        rect.setHeight(option.rect.height());

        painter->setOpacity(1);
        QString desktopfp=index.data(Qt::DisplayRole).value<QString>();
        QString iconstr=pUkuiMenuInterface->getAppIcon(desktopfp);
        QIcon icon;
        QFileInfo iconFileInfo(iconstr);

        QString appname=pUkuiMenuInterface->getAppName(desktopfp);
        QRect iconRect;
        QRect textRect;

        if(iconFileInfo.isFile() && (iconstr.endsWith(".png") || iconstr.endsWith(".svg")))
            icon=QIcon(iconstr);
        else
        {
            iconstr.remove(".png");
            iconstr.remove(".svg");
            icon=QIcon::fromTheme(iconstr);
            if(icon.isNull())
            {
                if(QFile::exists(QString("/usr/share/icons/hicolor/scalable/apps/%1.%2").arg(iconstr).arg("svg")))
                    icon=QIcon(QString("/usr/share/icons/hicolor/scalable/apps/%1.%2").arg(iconstr).arg("svg"));
                else if(QFile::exists(QString("/usr/share/icons/hicolor/scalable/apps/%1.%2").arg(iconstr).arg("png")))
                    icon=QIcon(QString("/usr/share/icons/hicolor/scalable/apps/%1.%2").arg(iconstr).arg("png"));
                else if(QFile::exists(QString("/usr/share/icons/hicolor/96x96/apps/%1.%2").arg(iconstr).arg("png")))
                    icon=QIcon(QString("/usr/share/icons/hicolor/96x96/apps/%1.%2").arg(iconstr).arg("png"));
                else if(QFile::exists(QString("/usr/share/icons/hicolor/96x96/apps/%1.%2").arg(iconstr).arg("svg")))
                    icon=QIcon(QString("/usr/share/icons/hicolor/96x96/apps/%1.%2").arg(iconstr).arg("svg"));
                else if(QFile::exists(QString("/usr/share/icons/hicolor/64x64/apps/%1.%2").arg(iconstr).arg("png")))
                    icon=QIcon(QString("/usr/share/icons/hicolor/64x64/apps/%1.%2").arg(iconstr).arg("png"));
                else if(QFile::exists(QString("/usr/share/icons/hicolor/64x64/apps/%1.%2").arg(iconstr).arg("svg")))
                    icon=QIcon(QString("/usr/share/icons/hicolor/64x64/apps/%1.%2").arg(iconstr).arg("svg"));
                else if(QFile::exists(QString("/usr/share/icons/hicolor/48x48/apps/%1.%2").arg(iconstr).arg("png")))
                    icon=QIcon(QString("/usr/share/icons/hicolor/48x48/apps/%1.%2").arg(iconstr).arg("png"));
                else if(QFile::exists(QString("/usr/share/icons/hicolor/48x48/apps/%1.%2").arg(iconstr).arg("svg")))
                    icon=QIcon(QString("/usr/share/icons/hicolor/48x48/apps/%1.%2").arg(iconstr).arg("svg"));
                else if(QFile::exists(QString("/usr/share/icons/hicolor/32x32/apps/%1.%2").arg(iconstr).arg("png")))
                    icon=QIcon(QString("/usr/share/icons/hicolor/32x32/apps/%1.%2").arg(iconstr).arg("png"));
                else if(QFile::exists(QString("/usr/share/icons/hicolor/32x32/apps/%1.%2").arg(iconstr).arg("svg")))
                    icon=QIcon(QString("/usr/share/icons/hicolor/32x32/apps/%1.%2").arg(iconstr).arg("svg"));
                else
                   {

//                    icon=QIcon::fromTheme(QString("application-x-desktop"));


                }
            }
        }

        iconRect=QRect(rect.x()+Style::AppLeftSpace ,
                       rect.y()+Style::AppTopSpace,
                       Style::AppListIconSize,
                       Style::AppListIconSize);
        textRect=QRect(rect.x(),
                       iconRect.bottom(),
                       rect.width(),
                       rect.height()-iconRect.height()-Style::AppTopSpace-20);

        icon.paint(painter,iconRect);

        //文本换行
        painter->setPen(QPen(Qt::white));
        painter->drawText(textRect,Qt::TextWordWrap | Qt::AlignCenter, appname);
        painter->restore();

    }
}

QSize GroupListDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{

      return QSize(Style::AppListItemSizeWidth,Style::AppListItemSizeHeight);
}

