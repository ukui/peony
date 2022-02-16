/*
 * Peony-Qt
 *
 * Copyright (C) 2021, KylinSoft Co., Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

#include "fullitemdelegate.h"
#include <QDebug>
#include "src/Interface/ukuimenuinterface.h"
#include "src/Style/style.h"

//qt's global function
extern void qt_blurImage(QImage &blurImage, qreal radius, bool quality, int transposed);

FullItemDelegate::FullItemDelegate(QObject *parent, int module):
    QStyledItemDelegate(parent)
{
    this->module=module;
    pUkuiMenuInterface=new UkuiMenuInterface;
    //打开ini文件
    QString pathini=QDir::homePath()+"/.cache/ukui-menu/ukui-menu.ini";
    settt=new QSettings(pathini,QSettings::IniFormat);

    QString path=QDir::homePath()+"/.config/ukui/ukui-menu.ini";
    setting=new QSettings(path,QSettings::IniFormat);

//    qDebug() << "---------------------gengxinleziti---------------------";

}

FullItemDelegate::~FullItemDelegate()
{
    if(pUkuiMenuInterface)
        delete pUkuiMenuInterface;
    if(settt)
        delete settt;
    if(setting)
        delete setting;
    pUkuiMenuInterface=nullptr;
    settt=nullptr;
    setting=nullptr;
}

void FullItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
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
        painter->setRenderHint(QPainter::Antialiasing);
        painter->setRenderHint(QPainter::SmoothPixmapTransform);
        QString desktopfp=index.data(Qt::DisplayRole).value<QString>();
        QString iconstr=pUkuiMenuInterface->getAppIcon(desktopfp);
        QIcon icon;
        QFileInfo iconFileInfo(iconstr);

        QString appname=pUkuiMenuInterface->getAppName(desktopfp);
        QRect iconRect;
        QRect textRect;
//        QString desktopfn=desktopfp.split('/').last();
//        setting->beginGroup("groupname");
//        bool iscontain=setting->contains(desktopfn);
//        setting->sync();
//        setting->endGroup();


        if(iconFileInfo.isFile() && (iconstr.endsWith(".png") || iconstr.endsWith(".svg")))
        {
            icon=QIcon(iconstr);
            //qDebug()<<"str-----------"<<iconstr;
        }
        else
        {
            //qDebug()<<"else"<<iconstr;
            iconstr.remove(".png");
            iconstr.remove(".svg");
            icon=QIcon::fromTheme(iconstr);
            //qDebug()<<"icon"<<icon;
            if(icon.isNull())
            {
//                qDebug()<<"isnottheme";
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

                    icon=QIcon::fromTheme(QString("application-x-desktop"));
                }
            }
        }


        //bool bigIcon = index.data(Qt::UserRole).value<bool>();
        bool bigIcon = index.data(Qt::UserRole+2).toBool();

        if(bigIcon)
        {
//            iconRect = QRect(rect.x()+Style::AppLeftSpace-3 ,
//                           rect.y()+Style::AppTopSpace-3,
//                           Style::AppListIconSize+12,
//                           Style::AppListIconSize+12);
            iconRect = QRect(rect.x()+Style::AppLeftSpace - 6 ,//94
                           rect.y()+Style::AppTopSpace - 6,//60
                           Style::AppListIconSize + 12,//96
                           Style::AppListIconSize + 12);
            textRect = QRect(rect.x(),
                           iconRect.bottom()-3,
                           rect.width(),
                           rect.height()-iconRect.height()-Style::AppTopSpace-30);

//            QPixmap pixmap;
//            pixmap = icon.pixmap((Style::AppListIconSize+20,Style::AppListIconSize+20));//wgx
//            icon=QIcon(pixmap);


        }else{
            Style::AppLeftSpace = (rect.width() - Style::AppListIconSize) / 2;
            Style::AppTopSpace = (rect.height() - Style::AppListIconSize) / 2;

            iconRect = QRect(rect.x() + Style::AppLeftSpace,
                             rect.y() + Style::AppTopSpace,
                             Style::AppListIconSize,
                             Style::AppListIconSize);
            textRect = QRect(rect.x(),
                             iconRect.bottom() + 5,
                             rect.width(),
                             Style::AppTopSpace - 5);

        }



        QString str;
        //打开文件.desktop
        GError** error = nullptr;
        GKeyFileFlags flags = G_KEY_FILE_NONE;
        GKeyFile* keyfile = g_key_file_new ();

        QByteArray fpbyte = desktopfp.toLocal8Bit();
        char* filepath = fpbyte.data();
        g_key_file_load_from_file(keyfile,filepath,flags,error);

        char* name = g_key_file_get_locale_string(keyfile,"Desktop Entry","Exec", nullptr, nullptr);
        //取出value值
        QString execnamestr = QString::fromLocal8Bit(name);
        //处理value值
        str = execnamestr;
//        str = execnamestr.section(' ', 0, 0);
//        QStringList list = str.split('/');
//        str = list[list.size()-1];
       //关闭文件
        g_key_file_free(keyfile);

        QString desktopfp1=str;//不带desktop
        settt->sync();
        settt->beginGroup("application");
        //判断禁用
        bool bo=settt->contains(desktopfp1.toLocal8Bit().data());// iskey
        bool bo1=settt->QSettings::value(desktopfp1.toLocal8Bit().data()).toBool();//isvalue
        settt->endGroup();

        if (bo && bo1==false)//都存在//存在并且为false
        {
            QPixmap pixmap;
            if(bigIcon)
            {
                pixmap = icon.pixmap((Style::AppListIconSize+12,Style::AppListIconSize+12),QIcon::Disabled,QIcon::Off);//wgx
            }else {
                pixmap = icon.pixmap((Style::AppListIconSize,Style::AppListIconSize),QIcon::Disabled,QIcon::Off);//wgx
            }
        icon = QIcon(pixmap);
        }
        else
        {
            QPixmap mPixmap;
            if(bigIcon)
            {
                mPixmap = icon.pixmap((Style::AppListIconSize+12,Style::AppListIconSize+12));//wgx
                mPixmap = mPixmap.scaled(108,108,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
            }else {
                mPixmap = icon.pixmap((Style::AppListIconSize,Style::AppListIconSize));//wgx
            }
        icon = QIcon(mPixmap);
        }

        //绘制图标阴影
        auto iconSize = QSize(Style::AppListIconSize,Style::AppListIconSize);
        QPixmap iconPixmap(icon.pixmap(iconSize));
        QPainter iconShadowPainter(&iconPixmap);
        iconShadowPainter.setCompositionMode(QPainter::CompositionMode_SourceIn);
        iconShadowPainter.fillRect(iconPixmap.rect(),QColor(0,0,0,80));
        iconShadowPainter.end();

        QImage iconShadowImage(iconSize + QSize(10,10), QImage::Format_ARGB32_Premultiplied);
        iconShadowImage.fill(Qt::transparent);
        iconShadowPainter.begin(&iconShadowImage);
        iconShadowPainter.drawPixmap(5,5,iconPixmap);

        qt_blurImage(iconShadowImage,10,false,false);
        for (int x = 0; x < iconShadowImage.width(); x++) {
              for (int y = 0; y < iconShadowImage.height(); y++) {
                  auto color = iconShadowImage.pixelColor(x, y);
                  if (color.alpha() > 0) {
                      color.setAlphaF(qMin(color.alphaF() * 1.5, 1.0));
                      iconShadowImage.setPixelColor(x, y, color);
                  }
              }
          }

        iconShadowPainter.end();
        painter->drawImage(iconRect.x() - 5,
                          iconRect.y() - 5,
                          iconShadowImage);

//        qDebug()<<"iconRect"<<iconRect;
        icon.paint(painter,iconRect);

        //文本换行
        QColor shadow=Qt::black;
        shadow.setAlpha(127);
        painter->setPen(shadow);
        QRect textLineRect;
        textLineRect.setLeft(textRect.left()+1);
        textLineRect.setRight(textRect.right()+1);
        textLineRect.setTop(textRect.top()+1);
        textLineRect.setBottom(textRect.bottom()+1);
        painter->drawText(textLineRect,Qt::TextWordWrap | Qt::AlignHCenter | Qt::AlignTop,appname);

        painter->setPen(QPen(Qt::white));
        painter->drawText(textRect,Qt::TextWordWrap | Qt::AlignHCenter | Qt::AlignTop, appname);

        painter->restore();

    }

}

QSize FullItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(index.row()>=Style::appNum)
    {
        return QSize(0,0);
    }

    return QSize(Style::AppListItemSizeWidth,Style::AppListItemSizeHeight);
}

