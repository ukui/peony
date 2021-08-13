#include "fullcommonusewidget.h"
#include <QDesktopWidget>
#include "src/Style/style.h"
#include <QDebug>
#include "src/Interface/ukuimenuinterface.h"
#include "src/UtilityFunction/fulllistview.h"

//#include "src/GroupListView/grouplistwidget.h"


FullCommonUseWidget::FullCommonUseWidget(QWidget *parent, int w, int h) : QWidget(parent)
{
    QString path=QDir::homePath()+"/.config/ukui/ukui-menu.ini";
    setting=new QSettings(path,QSettings::IniFormat);
    m_width=w;
    m_height=h;
    initUi();
}

QVector<QString> FullCommonUseWidget::keyVector=QVector<QString>();
QVector<int> FullCommonUseWidget::keyValueVector=QVector<int>();

void FullCommonUseWidget::initUi()
{
    this->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_StyledBackground,true);
    this->setAutoFillBackground(false);
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

 //   this->setFixedSize(1409,QApplication::primaryScreen()->geometry().height()-46);

    QHBoxLayout* mainLayout=new QHBoxLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(Style::AppListViewLeftMargin,Style::AppListViewTopMargin,Style::AppListViewRightMargin,Style::AppListViewBottomMargin);
//    mainLayout->setContentsMargins(0,0,0,0);
    this->setLayout(mainLayout);
    m_ukuiMenuInterface=new UkuiMenuInterface;
//    this->setStyleSheet("border:1px solid #ff0000;background:yellow;");
    initAppListWidget();
//    fillAppList();

}

void FullCommonUseWidget::initAppListWidget()
{
    m_listView=new FullListView(this,0);
    m_listView->setFixedSize(m_width-Style::AppListViewLeftMargin-Style::AppListViewRightMargin,m_height);
    QHBoxLayout *mainLayout=qobject_cast<QHBoxLayout*>(this->layout());
    mainLayout->insertWidget(1,m_listView);
    this->setStyleSheet("border-width:0px;border-style:solid;border-color:white;background:transparent;");
}

void FullCommonUseWidget::setStyleTable(int appnum)
{
    int lastpagenum=Style::appPage;
    Style::appColumn=6;
//    Style::appColumn=m_listView->width()/Style::AppListItemSizeWidth;
    Style::appLine=m_listView->height()/Style::AppListItemSizeHeight;
    Style::appNum=appnum;
    if(Style::appNum%(Style::appColumn * Style::appLine)==0)
        Style::appPage=Style::appNum/(Style::appColumn * Style::appLine);
    else
        Style::appPage=Style::appNum/(Style::appColumn * Style::appLine)+1;
    if(Style::appNum%(Style::appColumn)==0)
        Style::appRows=Style::appNum/ Style::appColumn;
    else
        Style::appRows=Style::appNum/ Style::appColumn+1;
    int newpagenum=Style::appPage;
    if(newpagenum!=lastpagenum)
        Q_EMIT drawButtonWidgetAgain();
}

void FullCommonUseWidget::fillAppList()
{
    m_data.clear();
    keyVector.clear();
    keyValueVector.clear();

    if(!setting->childGroups().contains("application"))
    {
        Q_FOREACH(QString desktopfp,UkuiMenuInterface::allAppVector)
            m_data.append(desktopfp);

        setting->beginGroup("application");
        for (int index = 0; index < m_data.size(); index++) {

            QFileInfo fileInfo(m_data.at(index));
            QString desktopfn=fileInfo.fileName();
            setting->setValue(desktopfn,index);
            setting->sync();
        }
        setting->endGroup();

    }

    else
    {
        setting->beginGroup("application");
        QStringList keyList=setting->childKeys();

        Q_FOREACH(QString desktopfn,keyList)
        {
            keyVector.append(desktopfn);
            keyValueVector.append(setting->value(desktopfn).toInt());
        }
        setting->endGroup();
        qSort(keyList.begin(),keyList.end(),cmpApp);

        Q_FOREACH(QString desktopfn,keyList)
            m_data.append("/usr/share/applications/"+desktopfn);
    }

    setStyleTable(m_data.size());
    m_listView->addData(m_data);

    connect(m_listView,&FullListView::sendHideMainWindowSignal,this,&FullCommonUseWidget::sendHideMainWindowSignal);
    connect(m_listView,&FullListView::sendItemClickedSignal,this,&FullCommonUseWidget::execApplication);
    connect(m_listView,&FullListView::pagenumchanged,this,&FullCommonUseWidget::pagenumchanged);
    connect(m_listView,&FullListView::sendUpdateAppListSignal,this,&FullCommonUseWidget::updateListViewSlot);
    connect(m_listView,&FullListView::pageCollapse,this,&FullCommonUseWidget::pageCollapse);
    connect(m_listView,&FullListView::pageSpread,this,&FullCommonUseWidget::pageSpread);

    qDebug() << "元素高" << Style::AppListItemSizeHeight << "每页应用行数" << Style::appLine << "页数" << Style::appPage;
    //限定滚动条范围，解决图标上端对齐问题
    m_listView->verticalScrollBar()->setMaximum(Style::AppListItemSizeHeight * Style::appLine * (Style::appPage - 1));
    qDebug() << "滚动条最大值1" << m_listView->verticalScrollBar()->maximum();
}

bool FullCommonUseWidget::cmpApp(QString &arg_1, QString &arg_2)
{
    if(keyValueVector.at(keyVector.indexOf(arg_1)) < keyValueVector.at(keyVector.indexOf(arg_2)))
        return true;
    else
        return false;
}
//执行应用程序
void FullCommonUseWidget::execApplication(QString desktopfp)
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
    str=execnamestr;
    //qDebug()<<"2 exec"<<str;

   //关闭文件
    g_key_file_free(keyfile);
    //打开ini文件
    QString pathini=QDir::homePath()+"/.cache/ukui-menu/ukui-menu.ini";
    settt=new QSettings(pathini,QSettings::IniFormat);
    settt->beginGroup("application");
    QString desktopfp1=str;
   //判断
    bool bo=settt->contains(desktopfp1.toLocal8Bit().data());// iskey
    bool bo1=settt->QSettings::value(desktopfp1.toLocal8Bit().data()).toBool();//isvalue
    settt->endGroup();
  
    if(bo && bo1==false)//都存在//存在并且为false，从filepathlist中去掉
    {
        //qDebug()<<"bool"<<bo<<bo1;

        return;
    }   
    QString exe=execnamestr;
    QStringList parameters;
    if (exe.indexOf("%") != -1) {
        exe = exe.left(exe.indexOf("%") - 1);
        //qDebug()<<"=====dd====="<<exe;
    }
    if (exe.indexOf("-") != -1) {
        parameters = exe.split(" ");
        exe = parameters[0];
        parameters.removeAt(0);
        //qDebug()<<"===qqq==="<<exe;
    }

    if(exe=="/usr/bin/indicator-china-weather")
    {
        parameters.removeAt(0);
        parameters.append("showmainwindow");
    }
    qDebug()<<"5 exe"<<exe<<parameters;
    QDBusInterface session("org.gnome.SessionManager", "/com/ukui/app", "com.ukui.app");
    if (parameters.isEmpty())
        session.call("app_open", exe, parameters);
    else
        session.call("app_open", exe, parameters);


    //Q_EMIT sendHideMainWindowSignal();
    return;


}

void FullCommonUseWidget::updateListViewSlot()
{

    m_data.clear();
    keyVector.clear();
    keyValueVector.clear();
    setting->beginGroup("application");
    QStringList keyList=setting->childKeys();

    Q_FOREACH(QString desktopfn,keyList)
    {
        keyVector.append(desktopfn);
        keyValueVector.append(setting->value(desktopfn).toInt());
    }
    setting->endGroup();
    qSort(keyList.begin(),keyList.end(),cmpApp);

    Q_FOREACH(QString desktopfn,keyList)
        m_data.append("/usr/share/applications/"+desktopfn);

    setStyleTable(m_data.size());
    m_listView->updateData(m_data);
}


void FullCommonUseWidget::updateListView(QString desktopfp)
{
//    m_listView->insertData(desktopfp);
    m_data.clear();
    Q_FOREACH(QString desktopfp,m_ukuiMenuInterface->getAllApp())
        m_data.append(desktopfp);

    setStyleTable(m_data.size());
    m_listView->updateData(m_data);


}

FullCommonUseWidget::~FullCommonUseWidget()
{
    if(m_ukuiMenuInterface)
        delete m_ukuiMenuInterface;
    if(setting)
        delete setting;
    if(m_listView)
        delete m_listView;
    if(settt)
        delete settt;
    m_ukuiMenuInterface=nullptr;
    setting=nullptr;
    m_listView=nullptr;
    settt=nullptr;

}
void FullCommonUseWidget::deleteAppListView()
{
    m_data.clear();
    keyVector.clear();
    keyValueVector.clear();

    setting->beginGroup("application");
    QStringList keyList=setting->childKeys();
    Q_FOREACH(QString desktopfn,keyList)
    {
        keyVector.append(desktopfn);
        keyValueVector.append(setting->value(desktopfn).toInt());
    }
    setting->endGroup();
    qSort(keyList.begin(),keyList.end(),cmpApp);

    Q_FOREACH(QString desktopfn,keyList)
        m_data.append("/usr/share/applications/"+desktopfn);

    setStyleTable(m_data.size());

    m_listView->addData(m_data);
}

void FullCommonUseWidget::repaintWid(int type)
{
    if (!m_listView) {
        m_listView = new FullListView(this, 0);
    }

    QHBoxLayout *mainLayout = qobject_cast<QHBoxLayout*>(this->layout());

    if (type == 0) {//横屏
        if (Style::IsWideScreen) {
            Style::AppListItemSizeWidth  = 284;
            Style::AppListViewLeftMargin = 44;
            Style::AppLeftSpace          = 94;
        } else {
            Style::AppListItemSizeWidth  = 216;
            Style::AppListViewLeftMargin = 52;
            Style::AppLeftSpace          = 60;
        }
        m_listView->setGridSize(QSize(Style::AppListItemSizeWidth, Style::AppListItemSizeHeight));
        m_listView->setFixedSize(this->width()-Style::AppListViewLeftMargin-Style::AppListViewRightMargin, this->height());
    } else if (type == 1) {   //竖屏
        QHBoxLayout *mainLayout = qobject_cast<QHBoxLayout*>(this->layout());
        m_listView->setGridSize(QSize());
        m_listView->setFixedSize(this->width()-Style::AppListViewLeftMargin-Style::AppListViewRightMargin,
                                 this->height()-Style::AppListViewTopMargin-Style::AppListViewBottomMargin - 48);
    }

    mainLayout->setContentsMargins(Style::AppListViewLeftMargin,Style::AppListViewTopMargin,Style::AppListViewRightMargin,Style::AppListViewBottomMargin);
    mainLayout->addWidget(m_listView);
}


