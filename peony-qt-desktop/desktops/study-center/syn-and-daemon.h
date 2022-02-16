/*
 * Peony-Qt
 *
 * Copyright (C) 2021, KylinSoft Co., Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

#ifndef SYNANDDAEMON_H
#define SYNANDDAEMON_H

#include "study-list-view.h"

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QStringList>
#include <QModelIndex>

namespace Peony {

class TabletAppEntity;

class SynAndDaemon : public QWidget
{
    Q_OBJECT
public:
    explicit SynAndDaemon(QStringList &strListTitleStyle, QList<QPair<QString, QList<TabletAppEntity*>>> &subtitleMap, QWidget *parent = nullptr);
    ~SynAndDaemon();
    void updateAppData(QList<QPair<QString, QList<TabletAppEntity*>>> &subAppMap);

private:

    //主界面
    QVBoxLayout*  m_mainLayout=nullptr;
    QLabel*       m_titleLabel=nullptr;
    QStringList   m_strListTitleStyle;
    QList<QPair<QString, QList<TabletAppEntity*>>> m_studyCenterDataList;
    QMap<QString, StudyListView*> m_viewMap;
    QColor m_colorMask;

protected:
    void initWidget(QStringList &strListTitleStyle);
    //初始化应用列表界面
    void initAppListWidget();

    void paintEvent(QPaintEvent *);

private Q_SLOTS:
    //执行应用程序
    void execApplication(QModelIndex appname);

Q_SIGNALS:
    void changeTheme( QString);
};
}

#endif // SYNANDDAEMON_H
