#include "thumbnail.h"
#include <QVBoxLayout>
#include "src/Style/style.h"
#include <QDir>
#include <gio/gdesktopappinfo.h>
#include <QIcon>
#include <QSettings>

ThumbNail::ThumbNail(QWidget *parent):
    QWidget(parent)
{

    pUkuiMenuInterface=new UkuiMenuInterface;
    initUi();
    QString path=QDir::homePath()+"/.config/ukui/ukui-menu.ini";
    setting=new QSettings(path,QSettings::IniFormat);
    QString pathini=QDir::homePath()+"/.cache/ukui-menu/ukui-menu.ini";
    disableSetting=new QSettings(pathini,QSettings::IniFormat);

}


void ThumbNail::initUi()
{
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    this->setStyleSheet("border:0px;background:transparent;");
    this->setFixedSize(Style::AppListIconSize,Style::AppListIconSize);

    iconLabel= new QLabel(this);
    iconLabel->setStyleSheet("border:0px;background-color:transparent;");
    iconLabel->setFocusPolicy(Qt::NoFocus);
    iconLabel->setFixedSize(Style::AppListIconSize,Style::AppListIconSize);

    layout = new QVBoxLayout(this);
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(0);
    layout->addWidget(iconLabel);

}
ThumbNail::~ThumbNail()
{
    if(pUkuiMenuInterface)
        delete pUkuiMenuInterface;
    if(setting)
        delete setting;
    if(disableSetting)
        delete disableSetting;
    if(iconLabel)
        delete iconLabel;
    if(m_icon)
        delete m_icon;
    pUkuiMenuInterface=nullptr;
    setting=nullptr;
    disableSetting=nullptr;
    iconLabel=nullptr;

    if(layout)
    delete layout;

}

void ThumbNail::setupthumbnail(QString desktopfp)
{
    QString fn=desktopfp.split('/').last();
    setting->beginGroup("groupname");
    QStringList keyList=setting->allKeys();
    bool isgroup=setting->contains(fn);

    if(!isgroup)
    {
        QString iconstr=pUkuiMenuInterface->getAppIcon(desktopfp);
        QString appname=pUkuiMenuInterface->getAppName(desktopfp);
        QIcon icon;
        QFileInfo iconFileInfo(iconstr);

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
                   icon=QIcon::fromTheme(QString("application-x-desktop"));

                 }
            }
        }

//        QPixmap pixmap = icon.pixmap((Style::AppListIconSize,Style::AppListIconSize),QIcon::Selected,QIcon::Off);
        QPixmap pixmap ;
        if(appDisable(desktopfp)==1)
        {
            pixmap =icon.pixmap((Style::AppListIconSize,Style::AppListIconSize),QIcon::Disabled,QIcon::Off);

            //qDebug()<<"appDisable(desktopfp)"<<appDisable(desktopfp);
        }else
        {
            //qDebug()<<"appDisable(desktopfp)"<<appDisable(desktopfp);
            pixmap = icon.pixmap((Style::AppListIconSize,Style::AppListIconSize),QIcon::Selected,QIcon::Off);

        }
        iconLabel->setPixmap(pixmap);

    }


    else
    {
        int k=0;
        iconLabel->setStyleSheet("border:0px;background: rgba(0, 0, 0, 0.25);border-radius: 16px;");
        QGridLayout *iconLayout=new QGridLayout();
        iconLayout->setContentsMargins(0,0,0,0);
        iconLayout->setSpacing(0);
        iconLabel->setLayout(iconLayout);


        for(int i=0;i<3;i++)
        {
            for (int j=1;j<4;j++)
              {

                if(k<keyList.size())
                {

                    QString fp="/usr/share/applications/"+keyList.at(k);
                    m_icon=new QLabel;
                    m_icon->setFixedSize(28,28);
                    QPixmap pixmap=getIcon(fp,28,28);
//                    QPixmap pixmap = icon.pixmap((28,28),QIcon::Selected,QIcon::Off);
                    m_icon->setPixmap(pixmap);
                    iconLayout->addWidget(m_icon,i,j-1);
                    k=k+1;
                }

                else
                    break;

              }
        }

    }



}

QPixmap ThumbNail::getIcon(QString desktopfp,int width,int height)
{

    QString iconstr=pUkuiMenuInterface->getAppIcon(desktopfp);
    QIcon icon;
    QFileInfo iconFileInfo(iconstr);
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
                   icon=QIcon::fromTheme(QString("application-x-desktop"));

             }
        }
    }
    if(appDisable(desktopfp)==1)
    {
        //return icon.pixmap((width,height),QIcon::Selected,QIcon::Off);
        return icon.pixmap((width,height),QIcon::Disabled,QIcon::Off);

    }

    return icon.pixmap((width,height),QIcon::Selected,QIcon::Off);

 }

bool ThumbNail::appDisable(QString desktopfp)//判断是否是禁用应用(这个还有问题暂时不会用)
{
    QString str;
    //打开文件.desktop
    GError** error=nullptr;
    GKeyFileFlags flags=G_KEY_FILE_NONE;
    GKeyFile* keyfile=g_key_file_new ();
    QByteArray fpbyte=desktopfp.toLocal8Bit();
    char* filepath=fpbyte.data();
    g_key_file_load_from_file(keyfile,filepath,flags,error);
    char* name=g_key_file_get_locale_string(keyfile,"Desktop Entry","Exec", nullptr, nullptr);
    //取出value值
    QString execnamestr=QString::fromLocal8Bit(name);
    //处理value值
    str=execnamestr;
//    str = execnamestr.section(' ', 0, 0);
//    QStringList list = str.split('/');
//    str = list[list.size()-1];
   //关闭文件
    g_key_file_free(keyfile);

    QString desktopfp1=/*"application/"+*/str;
    //QString group=desktopfp.split('/').last();
    disableSetting->beginGroup("application");
    //判断
    bool bo=disableSetting->contains(desktopfp1.toLocal8Bit().data());// iskey
    bool bo1=disableSetting->QSettings::value(desktopfp1.toLocal8Bit().data()).toBool();//isvalue
    disableSetting->endGroup();
    //qDebug()<<bo<<bo1;
    if (bo && bo1==false)//都存在//存在并且为false
    {
    return 1;
    }
    return 0;
}

