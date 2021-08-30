#ifndef STUDYSTATUSWIDGET_H
#define STUDYSTATUSWIDGET_H

#include <QWidget>
#include <QMap>
#include <QString>
#include <QStringList>
class QVBoxLayout;
class QLabel;
class QTableView;
class QGridLayout;
class ProgressWidget;
//class QListView;

class StudyStatusWidget : public QWidget
{
    Q_OBJECT
public:
    explicit StudyStatusWidget(QWidget *parent = nullptr);
    ~StudyStatusWidget();

public:

   QString  getStudyTime(QString &strMethod, QStringList &appList);

protected:
    void initWidget();

private:
    QVBoxLayout* m_mainVboxLayout=nullptr;
    QGridLayout* m_timeGridLayout = nullptr;
   // QTableView* m_timeView=nullptr;
    //QListView* m_showProgressView=nullptr;
    QLabel* m_todayTimeLabel=nullptr;
    QLabel* m_weekTimeLabel=nullptr;
    QLabel* m_monthTimeLabel=nullptr;
    QMap<QString, ProgressWidget*> m_progressMap;
//public Q_SLOTS:
  // void setAppTimeSlot();
    //Q_SIGNALS:
//    /**
//     * 向LetterWidget界面发送字母分类按钮被点击信号
//     * @param btnname存放按钮名称
//     */
//    void sendLetterBtnSignal(QString btnname);

//private Q_SLOTS:

};

#endif // STUDYSTATUSWIDGET_H
