#ifndef STUDYSTATUSWIDGET_H
#define STUDYSTATUSWIDGET_H

#include <QWidget>
#include <QMap>
#include <QString>
#include <QStringList>
#include "common.h"

class QVBoxLayout;
class QLabel;
class QTableView;
class QGridLayout;
class ProgressWidget;
class QDBusInterface;
class QPushButton;
class QScrollArea;
//class QListView;

class StudyStatusWidget : public QWidget
{
    Q_OBJECT
public:
    explicit StudyStatusWidget(QList<TABLETAPP> appList,QWidget *parent = nullptr);
    ~StudyStatusWidget();

public:
   QString  getStudyTime(QString &strMethod, QStringList &appList);
   QPixmap PixmapToRound(const QString &src, int radius);

protected:
    void initWidget();
    void initUserInfo();
    void resizeScrollAreaControls();
private:
    QVBoxLayout* m_mainVboxLayout=nullptr;
    QGridLayout* m_progressGridLayout = nullptr;
   // QTableView* m_timeView=nullptr;
    //QListView* m_showProgressView=nullptr;
    QLabel* m_todayTimeLabel=nullptr;
    QLabel* m_weekTimeLabel=nullptr;
    QLabel* m_monthTimeLabel=nullptr;
    QLabel* m_userIconLabel=nullptr;
    QLabel* m_userNameLabel=nullptr;
    QPushButton* m_updateTimeBt=nullptr;
    QScrollArea*   m_scrollArea=nullptr;
    QDBusInterface* m_userInterface=nullptr;
   // QMap<QString, ProgressWidget*> m_progressMap;
    QList<TABLETAPP> m_appList;

public Q_SLOTS:
   void paintProgressSlot(QList<TABLETAPP> applist);
   void timeChangeSlot(QString strMethod ,QString strTime);
   void markTimeSlot();

Q_SIGNALS:
    /**
     * 向主界面发送更新时间点击信号
     * @param btnname存放按钮名称
     */
    void updateTimeSignal();


};

#endif // STUDYSTATUSWIDGET_H