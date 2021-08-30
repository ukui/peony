#include "study-center-mode.h"
#include "study-status-widget.h"
#include "study-directory-widget.h"
#include <QStandardItemModel>
#include <QListView>
#include <QStandardItem>
#include <QGridLayout>
#include <QString>
#include <QApplication>
#include <QLabel>
#include <QScreen>
#include "../../tablet/data/tablet-app-manager.h"

using namespace Peony;


#define STUDY_CENTER_PRACTICE          "Practice"
#define STUDY_CENTER_STUGUARDS         "Student Guards"
#define STUDY_CENTER_SYN               "Synchronized"
#define STUDY_CENTER_STATISTICS        "Statistics"

StudyCenterMode::StudyCenterMode()
{
    initUi();
}

StudyCenterMode::~StudyCenterMode()
{

}

void StudyCenterMode::setActivated(bool activated)
{
    DesktopWidgetBase::setActivated(activated);
}

DesktopWidgetBase *StudyCenterMode::initDesktop(const QRect &rect)
{
    return DesktopWidgetBase::initDesktop(rect);
}
void StudyCenterMode::initUi()
{
    this->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    //this->setAttribute(Qt::WA_TranslucentBackground,true);
    this->setAttribute(Qt::WA_StyledBackground,true);
    this->setStyleSheet("border:0px;background:transparent;");
    this->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    int ScreenWidth=QApplication::primaryScreen()->geometry().width();
    int ScreenHeight=QApplication::primaryScreen()->geometry().height();
    this->setFixedSize(ScreenWidth,ScreenHeight);
    m_tableAppMangager = TabletAppManager::getInstance();

    QMap<QString, QList<TabletAppEntity*>> studyCenterDataMap = m_tableAppMangager->getStudyCenterData();
    QStringList strListTitleStyle;
    strListTitleStyle<<"精准练习"<<"color:#009ACD";
    QMap<QString, QList<TabletAppEntity*>> dataMap;

    dataMap.insert(STUDY_CENTER_MATH,studyCenterDataMap[STUDY_CENTER_MATH]);
    dataMap.insert(STUDY_CENTER_ENGLISH,studyCenterDataMap[STUDY_CENTER_ENGLISH]);
    dataMap.insert(STUDY_CENTER_CHINESE,studyCenterDataMap[STUDY_CENTER_CHINESE]);
    dataMap.insert(STUDY_CENTER_OTHER,studyCenterDataMap[STUDY_CENTER_OTHER]);
    StudyDirectoryWidget* practiceWidget = new StudyDirectoryWidget(strListTitleStyle,dataMap, 1);

    dataMap.clear();
    strListTitleStyle.clear();
    strListTitleStyle<<"守护中心"<<"color:#43CD80";
    dataMap.insert(STUDY_CENTER_SYNCHRONIZED,studyCenterDataMap[STUDY_CENTER_SYNCHRONIZED]);
    StudyDirectoryWidget* guradWidget = new StudyDirectoryWidget(strListTitleStyle,dataMap);

    dataMap.clear();
    strListTitleStyle.clear();
    strListTitleStyle<<"同步学习"<<"color:#FF8247";
    dataMap.insert(STUDY_CENTER_STUDENT_GUARD,studyCenterDataMap[STUDY_CENTER_STUDENT_GUARD]);
    StudyDirectoryWidget* synWidget = new StudyDirectoryWidget(strListTitleStyle,dataMap);

    StudyStatusWidget* widget4 = new StudyStatusWidget(this);



//    widget1->setFixedSize(iWidth/2, iHeight);
//    widget2->setFixedSize(iWidth/4, iHeight/3);
//    widget3->setFixedSize(iWidth/4, iHeight/3);
//    widget4->setFixedSize(iWidth/4, iHeight/3*2);
//    QHBoxLayout hboxLayout;
//    hboxLayout->addWidget(widget2);
//    hboxLayout->addWidget(widget3);
//    QVBoxLayout vboxLayout;
//    vboxLayout->addLayout(hboxLayout);
//    vboxLayout->addWidget(widget4);

    QGridLayout* gridLayout = new QGridLayout;

    gridLayout->addWidget(practiceWidget,0,0,3,2);
    gridLayout->addWidget(guradWidget,0,2,1,1);
    gridLayout->addWidget(synWidget,0,3,1,1);
    gridLayout->addWidget(widget4,1,2,2,2);

//    gridLayout->setColumnStretch(0,1);
//   // gridLayout->setColumnStretch(3,1);
//    gridLayout->setRowStretch(2,1);
//    gridLayout->setRowStretch(3,1);
    //gridLayout->setRowStretch(4,1);
    gridLayout->setMargin(80);
    gridLayout->setSpacing(20);
    this->setLayout(gridLayout);


//    QVBoxLayout* m_mainLayout = new QVBoxLayout(this);
//    //m_mainLayout->setContentsMargins(1,0,0,0);
//    QListView* m_listview = new QListView;

//    QStringList m_strListData ={"1","2","3","4"};
//    QStandardItemModel* listmodel=new QStandardItemModel(this);
//    m_listview->setModel(listmodel);
//    m_listview->setFixedSize(200,200);
//    for(int i = 0;i< m_strListData.size();i++)
//    {
//        QString str= static_cast<QString>(m_strListData.at(i));
//        QStandardItem* item=new QStandardItem(str);
//        listmodel->appendRow(item);
//    }
//    //this->addWidget(m_listview);
//    m_mainLayout->addWidget(m_listview);
    //this->setLayout(m_mainLayout);
}
