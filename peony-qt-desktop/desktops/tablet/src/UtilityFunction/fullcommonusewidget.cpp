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

    m_mainLayout = new QHBoxLayout(this);
    m_mainLayout->setSpacing(0);
    m_mainLayout->setAlignment(Qt::AlignHCenter);
    m_mainLayout->setContentsMargins(Style::AppListViewLeftMargin,Style::AppListViewTopMargin,Style::AppListViewRightMargin,Style::AppListViewBottomMargin);

    this->setLayout(m_mainLayout);
    m_ukuiMenuInterface=new UkuiMenuInterface;

    initAppListWidget();
    this->setStyleSheet("border-width:0px;border-style:solid;border-color:white;background:transparent;");
}

void FullCommonUseWidget::initAppListWidget()
{
//    m_listView=new FullListView(this,0);
//    m_listView->setFixedSize(m_width-Style::AppListViewLeftMargin-Style::AppListViewRightMargin,m_height);
//    m_mainLayout=qobject_cast<QHBoxLayout*>(this->layout());
//    m_mainLayout->insertWidget(1,m_listView);
}

void FullCommonUseWidget::fillAppList()
{
    m_data.clear();
    keyVector.clear();
    keyValueVector.clear();

    if (!setting->childGroups().contains("application")) {
        Q_FOREACH(QString desktopfp, UkuiMenuInterface::allAppVector)m_data.append(desktopfp);

        setting->beginGroup("application");
        for (int index = 0; index < m_data.size(); index++) {

            QFileInfo fileInfo(m_data.at(index));
            QString desktopfn = fileInfo.fileName();
            setting->setValue(desktopfn, index);
            setting->sync();
        }
        setting->endGroup();

    } else {
        setting->beginGroup("application");
        QStringList keyList = setting->childKeys();

        Q_FOREACH(QString desktopfn, keyList) {
                keyVector.append(desktopfn);
                keyValueVector.append(setting->value(desktopfn).toInt());
            }
        setting->endGroup();
        qSort(keyList.begin(), keyList.end(), cmpApp);

        Q_FOREACH(QString desktopfn, keyList)m_data.append("/usr/share/applications/" + desktopfn);
    }

//    updatePageData();
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

    updatePageData();
}


void FullCommonUseWidget::updateListView(QString desktopfp)
{
//    m_listView->insertData(desktopfp);
    m_data.clear();
    Q_FOREACH(QString desktopfp,m_ukuiMenuInterface->getAllApp())
        m_data.append(desktopfp);

    updatePageData();
}

FullCommonUseWidget::~FullCommonUseWidget()
{
    if(m_ukuiMenuInterface)
        delete m_ukuiMenuInterface;
    if(setting)
        delete setting;
    if(settt)
        delete settt;
    m_ukuiMenuInterface=nullptr;
    setting=nullptr;
    settt=nullptr;
}

void FullCommonUseWidget::updateStyleValue()
{
    Style::appColumn = 6;

    if (Style::ScreenRotation) {
        //竖屏7行
        Style::appLine = 7;
        Style::AppListViewTopMargin = 5;
    } else {
        //横屏4行
        Style::appLine = 4;
        Style::AppListViewTopMargin = 50;
    }

    Style::AppListViewWidth = this->width() - Style::AppListViewLeftMargin - Style::AppListViewRightMargin;
    Style::AppListViewHeight = this->height() - Style::AppListViewTopMargin - Style::AppListViewBottomMargin;

    Style::AppListItemSizeWidth = (Style::AppListViewWidth / Style::appColumn) - 1;
    Style::AppListItemSizeHeight = (Style::AppListViewHeight / Style::appLine) - 1;

    int lastPageNum = Style::appPage;
    Style::appNum = m_data.size();
    if (Style::appNum % (Style::appColumn * Style::appLine) == 0) {
        Style::appPage = Style::appNum / (Style::appColumn * Style::appLine);
    } else {
        Style::appPage = Style::appNum / (Style::appColumn * Style::appLine) + 1;
    }

    int newPageNum = Style::appPage;
    if (newPageNum != lastPageNum) {
        Q_EMIT drawButtonWidgetAgain();
    }
}

void FullCommonUseWidget::repaintWid(int type)
{
    //note 当屏幕发生变化后，对每个item的大小，appView的大小进行调整
    updateStyleValue();

    //TODO 实现可以只刷新某一页
    for (FullListView *listView : m_pageList) {
        listView->setGridSize(QSize(Style::AppListItemSizeWidth, Style::AppListItemSizeHeight));
        listView->setFixedSize(Style::AppListViewWidth, Style::AppListViewHeight);
        listView->update();
    }

    m_mainLayout->setContentsMargins(Style::AppListViewLeftMargin, Style::AppListViewTopMargin,
                                     Style::AppListViewRightMargin, Style::AppListViewBottomMargin);
    this->insertPageToLayout();
}

void FullCommonUseWidget::updatePageList()
{
    updateStyleValue();

    quint32 currentPageCount = m_pageList.count();

    if (Style::appPage < currentPageCount) {
        for (int i = Style::appPage; i < currentPageCount; ++i) {
            FullListView *listView = m_pageList.at(i);

            m_pageList.removeOne(listView);
            listView->deleteLater();
        }
    } else if (Style::appPage > currentPageCount) {
        for (int i = currentPageCount; i < Style::appPage; ++i) {
            FullListView *listView = new FullListView(this, 0);
            listView->setFixedSize(Style::AppListViewWidth, Style::AppListViewHeight);

            connect(listView, &FullListView::sendHideMainWindowSignal, this, &FullCommonUseWidget::sendHideMainWindowSignal);
            connect(listView, &FullListView::sendItemClickedSignal, this, &FullCommonUseWidget::execApplication);
            connect(listView, &FullListView::pagenumchanged, this, &FullCommonUseWidget::pagenumchanged);
            connect(listView, &FullListView::sendUpdateAppListSignal, this, &FullCommonUseWidget::updateListViewSlot);
            connect(listView, &FullListView::pageCollapse, this, &FullCommonUseWidget::pageCollapse);
            connect(listView, &FullListView::pageSpread, this, &FullCommonUseWidget::pageSpread);
            connect(listView, &FullListView::moveRequest, this, &FullCommonUseWidget::moveRequest);

            m_pageList.append(listView);
        }
    }
}

void FullCommonUseWidget::updatePageData()
{
    this->updatePageList();

    QList<QStringList> dataList;

    //每页应用数量
    quint32 appNumPerPage = Style::appColumn * Style::appLine;

    //每24个一组，进行分组
    for (int i = 0; i < (m_data.count() - 1); i += appNumPerPage) {
        dataList.append(m_data.mid(i, appNumPerPage));
    }

    for (int i = 0; i < dataList.count(); ++i) {
        m_pageList.at(i)->updateData(dataList.at(i));
    }

    this->insertPageToLayout();
}

void FullCommonUseWidget::insertPageToLayout()
{
    for (int i = 0; i < m_pageList.count(); ++i) {
        FullListView *page = m_pageList.at(i);
        if (page) {
            if (i == (Style::nowpagenum - 1)) {
                page->setHidden(false);
                m_mainLayout->insertWidget(1, page);
            } else {
                page->setHidden(true);
                m_mainLayout->removeWidget(page);
            }
        }
    }
}


