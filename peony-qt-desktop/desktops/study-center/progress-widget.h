#ifndef PROGRESSWIDGET_H
#define PROGRESSWIDGET_H

#include <QWidget>
#include "common.h"
class QProgressBar;
class QString;
class QLabel;
class QGridLayout;
//class QHBoxLayout;

class ProgressWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ProgressWidget(const TABLETAPP &AppInfo,QWidget *parent = nullptr);
    ~ProgressWidget();
    void initUi();

private:
    long int m_iTime ;
    quint32 m_serialNumber; //排序序号
    QString m_strAppName;
    QString m_strAppIcon;
    QProgressBar* m_processBar=nullptr;
    QLabel* m_nameLabel=nullptr;
    QLabel* m_iconLabel=nullptr;
    QLabel* m_timeLabel=nullptr;
    QGridLayout* m_timeGridLayout = nullptr;
  //  QHBoxLayout* m_iconGridLayout = nullptr;
public Q_SLOTS:
   void paintSlot(TABLETAPP &applist);
   void setMaximum(int iMaxValue);

};

#endif // PROGRESSWIDGET_H
