#ifndef GROUPLISTWIDGET_H
#define GROUPLISTWIDGET_H

#include <QWidget>
#include <QSettings>
#include "src/GroupListView/grouplistview.h"

class GroupListWidget : public QWidget
{
    Q_OBJECT
public:
    GroupListWidget(QWidget *parent);
    ~GroupListWidget();

    void fillAppList(QString groupname);

protected:
    void initUi();
    void initAppListWidget();
    static bool cmpApp(QString &arg_1,QString &arg_2);


private:
    QSettings *setting=nullptr;
    QSettings *settt=nullptr;
    GroupListView *m_groupview=nullptr;
    QStringList g_data;

    static QVector<QString> keyVector;
    static QVector<int> keyValueVector;

private Q_SLOTS:
    void execApplication(QString desktopfp);


Q_SIGNALS:
    void sendDragLeaveSignal(QString desktopfp);
};

#endif // GROUPLISTWIDGET_H
