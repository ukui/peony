#ifndef FullCommonUseWidget_H
#define FullCommonUseWidget_H

#include <QWidget>
#include <QSettings>
#include <QHBoxLayout>
#include <QScrollBar>
#include <QDir>
#include <QProcess>
#include "src/Interface/ukuimenuinterface.h"
#include "src/Style/style.h"
#include "src/UtilityFunction/fulllistview.h"
#include <gio/gdesktopappinfo.h>
#include <QSettings>
#include <QVector>


class FullCommonUseWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FullCommonUseWidget(QWidget *parent,int w, int h);
    ~FullCommonUseWidget();

    /**
     * @brief Update application list
     */
    void updateListView(QString desktopfp);

    /**
     * @brief fill application list
     */
    void fillAppList();

    /**
     * @brief repain the listview
     */
    void repaintWid(int type);

    void updateStyleValue();

    /**
     * @brief
     * @param isVertical 是否垂直屏幕模式
     */
    void updatePageData();

    void updatePageList();

    void insertPageToLayout();

private:
    UkuiMenuInterface* m_ukuiMenuInterface=nullptr;
    QStringList m_data;
    QSpacerItem *m_spaceItem=nullptr;
    QSettings* setting;
    static QVector<QString> keyVector;
    static QVector<int> keyValueVector;
    int m_width=0;
    int m_height=0;
    QSettings* settt;
    QHBoxLayout *m_mainLayout = nullptr;
    QList<FullListView*> m_pageList;
protected:
    /**
     * @brief Initializes UI
     */
    void initUi();
    /**
     * @brief Initialize the application list interface
     */
    void initAppListWidget();

    static bool cmpApp(QString &arg_1,QString &arg_2);


public Q_SLOTS:
    /**
     * @brief Open the application
     * @param arg: Desktop file path
     */
    void execApplication(QString desktopfp);
    void updateListViewSlot();

Q_SIGNALS:
    /**
     * @brief Send a hidden main window signal to the MainViewWidget
     */
    void sendHideMainWindowSignal();
    void sendSortApplistSignal();
    void pagenumchanged(qint32 signal); //翻页信号
    void sendUpdateAppListSignal(); //更新应用顺序信号
    void sendGroupClickSignal(QString desktopfn); //点击应用组信号
    void drawButtonWidgetAgain(); //重画按钮信号
    void pageCollapse(); //左滑页面收纳
    void pageSpread(); //右滑页面展开
    void moveRequest(qint32 length); //右滑页面展开
};

#endif // FullCommonUseWidget_H
