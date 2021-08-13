#ifndef PLUGINWIDGET_H
#define PLUGINWIDGET_H

#include <QObject>
#include <QWidget>
#include <QStackedWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPluginLoader>
#include <src/UtilityFunction/AbstractInterface.h>
#include <src/UtilityFunction/abstractInterface.h>
#include <QLibrary>
#include <QEvent>
#include <QLabel>
#include <QGSettings>
#include <QSettings>
class pluginwidget : public QWidget
{
    Q_OBJECT
public:
    explicit pluginwidget(QWidget *parent = nullptr);
    ~pluginwidget();
    QStackedWidget *stackWidget=nullptr;
    void init();
    QObject *plugin=nullptr;
    QObject *pluginnote=nullptr;
    AbstractInterface *app;
    kyMenuPluginInterface *app1;
    QWidget* focuswidget;
    QWidget* notewidget;

    QWidget *secondwidget=nullptr;//提醒 名字之后要改
    QWidget *threeWidget=nullptr;//计算器

    QLabel* label = nullptr;
    QLabel* label2 = nullptr;
    QLabel* label3 = nullptr;
    QLabel* label4 = nullptr;

private:
    QGSettings *themeSetting=nullptr;
    QString themeName;
    QWidget *toolbutton_widget;
    void changeSearchBoxBackground();
protected:
    void mouseReleaseEvent(QMouseEvent *e);
    void mousePressEvent(QMouseEvent *event);
private Q_SLOTS:

public Q_SLOTS:
    void button_1_click();
    void button_2_click();
    void button_3_click();
    void button_4_click();
Q_SIGNALS:

};

#endif // PLUGINWIDGET_H
