#include "grouplistwidget.h"
#include "grouplistview.h"
#include "src/UtilityFunction/fullcommonusewidget.h"
#include <QDir>
#include <QHBoxLayout>
#include <QSettings>


GroupListWidget::GroupListWidget(QWidget *parent) : QWidget(parent)
{

    QString path=QDir::homePath()+"/.config/ukui/ukui-menu-panel.ini";
    setting=new QSettings(path,QSettings::IniFormat);
    installEventFilter(this);
    initUi();
}

GroupListWidget::~GroupListWidget()
{

}

QVector<QString> GroupListWidget::keyVector=QVector<QString>();
QVector<int> GroupListWidget::keyValueVector=QVector<int>();

void GroupListWidget::initUi()
{

    this->setAttribute(Qt::WA_StyledBackground,true);
    this->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    this->setStyleSheet("border:0px;background:rgba(255,255,255,0.25);border-radius: 24px;");
    this->setAttribute(Qt::WA_X11DoNotAcceptFocus, false);


    QHBoxLayout *mainLayout=new QHBoxLayout;
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0,0,0,0);
    this->setLayout(mainLayout);

    initAppListWidget();
}

void GroupListWidget::initAppListWidget()
{
    m_groupview=new GroupListView(this);
    m_groupview->setFixedSize(800,461);
    QHBoxLayout *mainLayout=qobject_cast<QHBoxLayout*>(this->layout());
    mainLayout->insertWidget(1,m_groupview);

    connect(m_groupview,&GroupListView::sendDragLeaveSignal,this,&GroupListWidget::sendDragLeaveSignal);

}


void GroupListWidget::fillAppList(QString groupname)
{
    g_data.clear();;

    setting->beginGroup(QString(groupname));
    QStringList keyList=setting->childKeys();
    setting->endGroup();
    Q_FOREACH(QString desktopfn,keyList)
    {
        keyVector.append(desktopfn);
        keyValueVector.append(setting->value(desktopfn).toInt());
    }

    qSort(keyList.begin(),keyList.end(),cmpApp);

    Q_FOREACH(QString desktopfn,keyList)
        g_data.append("/usr/share/applications/"+desktopfn);
    m_groupview->addData(g_data);

//     connect(m_groupview,&FullListView::sendItemClickedSignal,this,&FullCommonUseWidget::execApplication);


}

bool GroupListWidget::cmpApp(QString &arg_1, QString &arg_2)
{
    if(keyValueVector.at(keyVector.indexOf(arg_1)) < keyValueVector.at(keyVector.indexOf(arg_2)))
        return true;
    else
        return false;
}


/**
 * 执行应用程序
 */
void GroupListWidget::execApplication(QString desktopfp)
{

    //    m_listView->hide();
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
           str = execnamestr.section(' ', 0, 0);
           QStringList list = str.split('/');
           str = list[list.size()-1];
          //关闭文件
           g_key_file_free(keyfile);
           //打开ini文件
           QString pathini=QDir::homePath()+"/.cache/ukui-menu/ukui-menu.ini";
           settt=new QSettings(pathini,QSettings::IniFormat);
           // settt->beginGroup("application");


           QString desktopfp1="application/"+str;
           //判断
           bool bo=settt->contains(desktopfp1.toLocal8Bit().data());// iskey
           bool bo1=settt->QSettings::value(desktopfp1.toLocal8Bit().data()).toBool();//isvalue
           if(bo && bo1==false)//都存在//存在并且为false，从filepathlist中去掉
           {
           return;
           }
        qDebug() <<"exec,,,,,";
    //    Q_EMIT sendHideMainWindowSignal();
        GDesktopAppInfo * desktopAppInfo=g_desktop_app_info_new_from_filename(desktopfp.toLocal8Bit().data());
        g_app_info_launch(G_APP_INFO(desktopAppInfo),nullptr, nullptr, nullptr);
        g_object_unref(desktopAppInfo);
}


