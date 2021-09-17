#ifndef STUDYCENTERMODE_H
#define STUDYCENTERMODE_H

#include "desktop-widget-base.h"
#include "common.h"
#include "../../tablet/data/tablet-app-manager.h"

#include <QDBusInterface>

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
    TabletAppManager* m_tableAppMangager=nullptr;
    QList<QString> m_appList;

    QDBusInterface *m_statusManagerDBus = nullptr;
    QPoint m_pressPoint;
    QPoint m_releasePoint;
    QPoint m_lastPressPoint;
    bool   m_leftButtonPressed = false;

public Q_SLOTS:
   void updateTimeSlot();

Q_SIGNALS:
    void valueChangedSingal(QList<TABLETAPP> applist);
    void timeChangedSingal(QString strMethod ,QString strTime);
    void markTimeSingal();
};

}

#endif // STUDYCENTERMODE_H
