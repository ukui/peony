#ifndef PROGRESSWIDGET_H
#define PROGRESSWIDGET_H

#include <QWidget>

class QProgressBar;
class QString;
class QLabel;
class QGridLayout;
//class QHBoxLayout;

class ProgressWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ProgressWidget(const QString &strAppName,long int iTime = 0,QWidget *parent = nullptr);
    ~ProgressWidget();
    void initUi();

private:
    long int m_iTime ;
    QString m_strAppName;
    QProgressBar* m_processBar=nullptr;
    QLabel* m_nameLabel=nullptr;
    QLabel* m_iconLabel=nullptr;
    QLabel* m_timeLabel=nullptr;
    QGridLayout* m_timeGridLayout = nullptr;
  //  QHBoxLayout* m_iconGridLayout = nullptr;


};

#endif // PROGRESSWIDGET_H
