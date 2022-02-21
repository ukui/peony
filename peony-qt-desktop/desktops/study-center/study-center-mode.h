#ifndef STUDYCENTERMODE_H
#define STUDYCENTERMODE_H

#include "desktop-widget-base.h"
#include "common.h"
#include "../../tablet/data/tablet-app-manager.h"
#include "study-status-widget.h"
#include "study-directory-widget.h"
#include "syn-and-daemon.h"
#include <QDBusInterface>
class QGridLayout;

namespace Peony {

class TabletAppManager;

class StudyCenterMode : public DesktopWidgetBase
{
Q_OBJECT
public:
    explicit StudyCenterMode(QWidget *parent = nullptr);

    ~StudyCenterMode();

    void setActivated(bool activated);

    DesktopWidgetBase *initDesktop(const QRect &rect) override;

public:
    QList<TABLETAPP>   getTimeOrder(QMap<QString, QList<TabletAppEntity*>> studyCenterDataMap );
    void  initTime();
    long int getStudyTime(QString strMethod, QString appName);
    QString  getTime(QString strMethod, QStringList appList);

    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    void initUi();
    void updateAppData();
    void screenRotation();
    void initPageButton();
    void updatePageButton();
    void changeTheme();
    void screenChange();

private:
    QGridLayout* m_mainGridLayout=nullptr;
    StudyDirectoryWidget* practiceWidget=nullptr;
    SynAndDaemon* guradWidget=nullptr;
    SynAndDaemon* synWidget=nullptr;
    StudyStatusWidget* statusWidget=nullptr;

    TabletAppManager* m_tableAppMangager=nullptr;
    QList<QString> m_appList;

    QDBusInterface *m_statusManagerDBus = nullptr;
    QPoint m_pressPoint;
    QPoint m_releasePoint;
    QPoint m_lastPressPoint;
    bool   m_leftButtonPressed = false;
    bool m_isTabletMode;
    QString m_direction;

    QWidget *m_pageButtonWidget = nullptr;
    QHBoxLayout *m_buttonLayout = nullptr;
    QButtonGroup *m_buttonGroup = nullptr;

public Q_SLOTS:
   void updateTimeSlot();
   void updateRotationsValue(QString rotation);
   void updateTabletModeValue(bool mode);

   void pageButtonClicked(QAbstractButton *button);
protected Q_SLOTS:
   void centerToScreen(QWidget *widget);

    /**
     * @brief mdm 禁用或解除禁用应用时，刷新界面
     */
    void updateUiSlot();

Q_SIGNALS:
    void valueChangedSingal(QList<TABLETAPP> applist);
    void timeChangedSingal(QString strMethod ,QString strTime);
    void markTimeSingal();
};

}

#endif // STUDYCENTERMODE_H
