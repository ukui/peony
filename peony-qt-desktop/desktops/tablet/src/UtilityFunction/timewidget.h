#ifndef TIMEWIDGET_H
#define TIMEWIDGET_H

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QLocale>
#include <QDateTime>
#include <QGSettings>
#include <QSettings>
#include "src/UtilityFunction/AbstractInterface.h"
#include <QGraphicsDropShadowEffect>
#include "src/UtilityFunction/pluginwidget.h"
#include "src/Interface/currenttimeinterface.h"

class TimeWidget : public QWidget
{
    Q_OBJECT
public:
    TimeWidget(QWidget *parent);
    ~TimeWidget();
    //QWidget* focusPlug=nullptr;
    pluginwidget *focusPlug=nullptr;
    void setDownOpacityEffect(const qreal& num);

protected:
    void initUi();
    void changeSearchBoxBackground();


private:
    CurrentTimeInterface *Time=nullptr;
    QGSettings *themeSetting=nullptr;
    QGSettings *timeSetting=nullptr;
    QString themeName;
    QObject *plugin=nullptr;
    AbstractInterface *app;
    QGraphicsDropShadowEffect *effect;
    QPalette palette;
    QTimer* myTimer=nullptr;

    //上方(时间+搜索)
    QWidget* upWidget=nullptr;
    QWidget* leftUpWidget=nullptr;
    QWidget* upLeftWidget=nullptr;
    QWidget* downWidget=nullptr;


    QVBoxLayout* upLayout;
    QHBoxLayout* leftUpLayout;
    QVBoxLayout* downLayout;
    QVBoxLayout* focusModeLayout;

    QHBoxLayout* upLeftLayout;
    QVBoxLayout* upRightLayout;

    QLabel* timeLabel;
    QLabel* weekLabel;
    QLabel* dateLabel;
    QPushButton* searchEditBtn;

    //右侧
    QWidget* upRightWidget=nullptr;

    qreal opacity; //透明系数

private Q_SLOTS:
    void obtainSearchResult();

};

#endif // TIMEWIDGET_H
