#ifndef TABLETMODE_H
#define TABLETMODE_H

#include "desktop-widget-base.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QWidget>
#include <QVBoxLayout>
#include <QListView>
#include <QDateTime>
#include <QTimer>
#include <QStandardItemModel>
#include <QFrame>
#include "src/Interface/ukuimenuinterface.h"
#include "src/UtilityFunction/fulllistview.h"
#include "src/UtilityFunction/fullcommonusewidget.h"
#include "src/MainViewWidget/directorychangedthread.h"
#include "src/UtilityFunction/timewidget.h"
//#include "src/GroupListView/grouplistwidget.h"
#include <QSettings>
#include <QAction>
#include <qgsettings.h>
#include <QButtonGroup>
#include "src/UtilityFunction/toolbox.h"
#include <QVariantAnimation>
#include <QEvent>
//#include <QSpacerItem>
#include <QKeyEvent>
#include <QTimer>

#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QParallelAnimationGroup>

namespace Peony {

class TabletMode : public DesktopWidgetBase
{
Q_OBJECT
public:
    TabletMode(QWidget *parent = nullptr);

    ~TabletMode() override;

    void setActivated(bool activated) override;

    DesktopWidgetBase *initDesktop(const QRect &rect) override;

protected:
    void mousePressEvent(QMouseEvent *event) override;

    void mouseReleaseEvent(QMouseEvent *event) override;

    void mouseMoveEvent(QMouseEvent *event) override;

private:
    QPoint m_startPoint;
    QPoint m_endPoint;
    QPoint m_lastEndPoint;//上一次move事件鼠标停留的位置

    bool m_leftKeyPressed = false;

    bool m_isTabletMode;//平板模式标志
    //左侧
    TimeWidget *leftWidget=nullptr;

    //翻页
    QVariantAnimation *m_animation=nullptr;

    //监控
    QFileSystemWatcher* m_fileWatcher=nullptr;
    QFileSystemWatcher* m_fileWatcher1=nullptr;
    DirectoryChangedThread* m_directoryChangedThread=nullptr;

    //背景
    bool hideBackground=false;
    int appRun=2;

    //翻转
    QGSettings* rotation=nullptr;
    QString direction;
    QGSettings* tabletMode=nullptr;
    bool autoRotation=false;

    int m_width=0;
    int m_height=0;

    //最右侧的button
    QWidget* buttonWidget=nullptr;
    QButtonGroup* buttonGroup=nullptr;
    QVBoxLayout* vbox=nullptr;
    QPushButton* button=nullptr;

    /*pc*/
    QGSettings* bg_effect = nullptr;
    bool m_winFlag = false;
    void paintEvent(QPaintEvent *event);

    //工具框显示
    ToolBox *toolBox=nullptr;

    bool checkapplist();

protected:
    void screenVertical();
    void keyPressEvent(QKeyEvent* event);
    /*pc 功能*/
    bool event(QEvent *event);
    bool appListFile();
    void centerToScreen(QWidget* widget);

public:
    void screenHorizontal();
    void buttonWidgetShow();
    FullCommonUseWidget* m_CommonUseWidget=nullptr;
    void collapse();
    void spread();
    void setOpacityEffect(const qreal& num);
    void showPCMenu();          //打开pc的开始菜单

public Q_SLOTS:
    void directoryChangedSlot();
    void requestUpdateSlot(QString desktopfp);
    void requestDeleteAppSlot();
    void screenRotation();
    void buttonClicked(QAbstractButton *button);
    void pageNumberChanged();
    void desktopSwitch(int res); //receive the dbus signal for application run or close
    void hideOrShowMenu(bool res); //hide or show start menu

    void client_get(QString str);

    /**/
    /**
     * @brief Monitor win key events
     */
    void XkbEventsPress(const QString &keycode);
    void XkbEventsRelease(const QString &keycode);
    /**
     * @brief Open the start menu by signaling
     */
    void recvStartMenuSlot();
    /**
     * @brief Monitor win-key-release key value.
     */
    void winKeyReleaseSlot(const QString &key);
    /**
     * @brief Load the full screen window
     */
    void recvHideMainWindowSlot();//接收隐藏主窗口信号
    /**
     * @brief unSetEffect
     * @param str
     */
    void unSetEffect(std::string str);
    /**
     * @brief setEffect
     * @param str
     */
    void setEffect(std::string str);

Q_SIGNALS:
    void sendDirectoryPath(QString arg);
    void UpdateSignal();
};

}

#endif // TABLETMODE_H
