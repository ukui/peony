#include "fullcommonusewidget.h"
#include "src/Interface/ukuimenuinterface.h"
#include "src/UtilityFunction/fulllistview.h"
#include "src/Style/style.h"

#include <QDesktopWidget>
#include <QDebug>


FullCommonUseWidget::FullCommonUseWidget(QWidget *parent, int w, int h) : QStackedWidget(parent)
{
    QString path=QDir::homePath()+"/.config/ukui/ukui-menu.ini";
    setting=new QSettings(path,QSettings::IniFormat);
    m_width=w;
    m_height=h;
    initUi();
}

QVector<QString> FullCommonUseWidget::keyVector=QVector<QString>();
QVector<int> FullCommonUseWidget::keyValueVector=QVector<int>();
QMap<QString, qint32> FullCommonUseWidget::m_appMap = QMap<QString, qint32>();

void FullCommonUseWidget::initUi()
{
    this->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_StyledBackground,true);
    this->setAutoFillBackground(false);
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

    setContentsMargins(Style::AppListViewLeftMargin,Style::AppListViewTopMargin,
                       Style::AppListViewRightMargin,Style::AppListViewBottomMargin);
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
        this->loadAllApp();
    }

//    updatePageData();
}

bool FullCommonUseWidget::cmpApp(QString &arg_1, QString &arg_2)
{
    if(m_appMap.value(arg_1) < m_appMap.value(arg_2))
        return true;
    else
        return false;
}
//执行应用程序
void FullCommonUseWidget::execApplication(QString desktopfp)
{
    //打开文件.desktop
    GError **error = nullptr;
    GKeyFileFlags flags = G_KEY_FILE_NONE;
    GKeyFile *keyfile = g_key_file_new();

    QByteArray fpbyte = desktopfp.toLocal8Bit();
    char *filepath = fpbyte.data();
    g_key_file_load_from_file(keyfile, filepath, flags, error);

    char *name = g_key_file_get_locale_string(keyfile, "Desktop Entry", "Exec", nullptr, nullptr);
    //取出value值
    QString exe = QString::fromLocal8Bit(name);
    //关闭文件
    g_key_file_free(keyfile);
    //打开ini文件
    QString pathini = QDir::homePath() + "/.cache/ukui-menu/ukui-menu.ini";
    if (!settt) {
        settt = new QSettings(pathini, QSettings::IniFormat);
    }
    settt->sync();
    settt->beginGroup("application");
    //判断
    bool bo = settt->contains(exe.toLocal8Bit().data());// iskey
    bool bo1 = settt->QSettings::value(exe.toLocal8Bit().data()).toBool();//isvalue
    settt->endGroup();

    if (bo && !bo1) {
        return;
    }

    QStringList parameters;
    if (exe.indexOf("%") != -1) {
        exe = exe.left(exe.indexOf("%") - 1);
    }

    if (exe.indexOf(" ") != -1) {
        parameters = exe.split(QRegExp("\\s+"));
        exe = parameters[0];
        parameters.removeAt(0);
    }

    if (exe == "/usr/bin/indicator-china-weather") {
        parameters.removeAt(0);
        parameters.append("showmainwindow");
    }
    qDebug() << "tablet launch app, exe:" << exe << "param:" << parameters;
    QDBusInterface session("org.gnome.SessionManager", "/com/ukui/app", "com.ukui.app");
    session.call("app_open", exe, parameters);
}

void FullCommonUseWidget::updateListViewSlot()
{
    this->loadAllApp();
    updatePageData();
}

void FullCommonUseWidget::loadAllApp()
{
    m_data.clear();
    m_appMap.clear();

    //同步一次，及时同步开始菜单的改动
    setting->sync();
    setting->beginGroup("application");
    QStringList keyList = setting->childKeys();
    for (QString &key: keyList) {
        m_appMap.insert(key, setting->value(key).toInt());
    }
    setting->endGroup();

    qSort(keyList.begin(), keyList.end(), cmpApp);

    //TODO 安卓app，第三方app呢？？？
    for (QString &key: m_ukuiMenuInterface->getAllApp()) {
        m_data.append(key);
    }
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

    setContentsMargins(Style::AppListViewLeftMargin, Style::AppListViewTopMargin,
                                     Style::AppListViewRightMargin, Style::AppListViewBottomMargin);
    this->insertPageToLayout();
}

void FullCommonUseWidget::updatePageList()
{
    updateStyleValue();

    quint32 currentPageCount = count();

    if (Style::appPage < currentPageCount) {
        for (int i = Style::appPage; i < currentPageCount; ++i) {
            FullListView *listView = m_pageList.at(i);

            removeWidget(listView);
            m_pageList.removeOne(listView);
            listView->deleteLater();
            m_backToMain = true;
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

            addWidget(listView);
            m_pageList.append(listView);
        }
    }
}

void FullCommonUseWidget::updatePageData()
{
    m_backToMain = false;
    this->updatePageList();

    QList<QStringList> dataList;

    //每页应用数量
    quint32 appNumPerPage = Style::appColumn * Style::appLine;

    //每24个一组，进行分组
    for (int i = 0; i < m_data.count(); i += appNumPerPage) {
        dataList.append(m_data.mid(i, appNumPerPage));
    }

    for (int i = 0; i < dataList.count(); ++i) {
        m_pageList.at(i)->updateData(dataList.at(i));
    }

    if (m_backToMain) {
        Style::appPage = 2;
        Style::nowpagenum = 2;
        Q_EMIT pagenumchanged(-1, true);
    } else {
        this->insertPageToLayout();
    }
}

void FullCommonUseWidget::insertPageToLayout()
{
    setCurrentIndex(Style::nowpagenum - 1);
}


