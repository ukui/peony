#ifndef TOOLBOX_H
#define TOOLBOX_H

#include <QObject>
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QGraphicsDropShadowEffect>
#include <QGSettings>
#include <QHBoxLayout>
#include "src/Interface/currenttimeinterface.h"

class ToolBox : public QWidget
{
    Q_OBJECT
public:
    explicit ToolBox(QWidget *parent,int w, int h);
    ~ToolBox();
    int type=-1;
    void setToolOpacityEffect(const qreal& num);

protected:
    void initUi();
    void openSearchBox();
    void openFocusMode();
    void openNote();
    void drawfocusStatus();
    void changeToolBoxBackground();
private:
    int m_width=0;
    int m_height=0;
    QGSettings *themeSetting=nullptr;
    QString themeName;
    QWidget* timeWidget=nullptr;
    QWidget* toolWidget=nullptr;
    QVBoxLayout* timeLayout=nullptr;
    QVBoxLayout* toolLayout=nullptr;
    QLabel* hourLabel=nullptr;
    QLabel* minuteLabel=nullptr;
    QGSettings* pluginSetting=nullptr;
    QGSettings *timeSetting=nullptr;
    QTimer* myTimer=nullptr;

    bool isFocusmodeRun=false;
    QLabel* focusStatus=nullptr;
    QHBoxLayout* focusBoxLayout=nullptr;

    CurrentTimeInterface* Time=nullptr;

    QPushButton* searchBtn;
    QPushButton* focusBtn;
    QPushButton* noteBtn;

    QWidget* searchWidget=nullptr;
    QWidget* noteWidget=nullptr;

    QGraphicsDropShadowEffect* effect=nullptr;

    qreal opacity;


Q_SIGNALS:
    void pageSpread(); //右滑页面展开
};

#endif // TOOLBOX_H
