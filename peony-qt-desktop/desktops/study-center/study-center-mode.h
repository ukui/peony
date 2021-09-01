#ifndef STUDYCENTERMODE_H
#define STUDYCENTERMODE_H

#include "desktop-widget-base.h"
#include "common.h"
#include "../../tablet/data/tablet-app-manager.h"
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

private:
    void initUi();
    TabletAppManager* m_tableAppMangager=nullptr;
    QList<QString> m_appList;

Q_SIGNALS:
    void valueChangedSingal(QList<TABLETAPP> applist);
    void timeChangedSingal(QString strMethod ,QString strTime);

};

}

#endif // STUDYCENTERMODE_H
