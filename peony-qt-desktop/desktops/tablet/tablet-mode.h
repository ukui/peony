#ifndef TABLETMODE_H
#define TABLETMODE_H

#include "desktop-widget-base.h"

#include "src/Interface/ukuimenuinterface.h"
#include "src/UtilityFunction/fulllistview.h"
#include "src/UtilityFunction/fullcommonusewidget.h"
#include "src/MainViewWidget/directorychangedthread.h"
#include "src/UtilityFunction/toolbox.h"
#include "src/UtilityFunction/tablet-plugin-widget.h"

#include <QSettings>
#include <QAction>
#include <qgsettings.h>
#include <QButtonGroup>
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
#include <QVariantAnimation>
#include <QEvent>
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

    void initGSettings();

    void initRightButton();
    void initAllWidget();

    void updateByDirection();

    void screenHorizontal();
    void buttonWidgetShow();
    void collapse();
    void spread();
    void setToolsOpacityEffect(const qreal& num);
    void showPCMenu();          //打开pc的开始菜单

protected:
    void mousePressEvent(QMouseEvent *event) override;

    void mouseReleaseEvent(QMouseEvent *event) override;

    void mouseMoveEvent(QMouseEvent *event) override;

    void screenVertical();
    void keyPressEvent(QKeyEvent* event);

    bool appListFile();
    void centerToScreen(QWidget* widget);

private:
    bool checkAppList();
    void paintEvent(QPaintEvent *event);

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

private:
    QPoint m_startPoint;
    QPoint m_endPoint;
    QPoint m_lastEndPoint;//上一次move事件鼠标停留的位置
    quint64 m_minWidth = 500;
    bool m_leftKeyPressed = false;
    bool m_isTabletMode;//平板模式标志

//    //主界面布局，横屏时为水平布局，竖屏时为垂直布局
//    QBoxLayout *m_layout = nullptr;
//
//    //左侧的时间label，搜索框和小插件。
//    TimeWidget *m_TimeWidget = nullptr;
//
//    //显示应用图标的主体容器
//    FullCommonUseWidget* m_mainAppListContainer = nullptr;


    FullCommonUseWidget* m_CommonUseWidget=nullptr;

    //左侧
    TabletPluginWidget *m_leftWidget=nullptr;

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
    QGSettings* m_rotationGSettings = nullptr;
    QString m_direction;
    QGSettings* m_tabletModeGSettings = nullptr;
    bool m_autoRotation=false;

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

    //工具框显示
    ToolBox *toolBox=nullptr;

};

}

#endif // TABLETMODE_H
