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

#ifndef STUDYDIRECTORYWIDGET_H
#define STUDYDIRECTORYWIDGET_H

#include "study-list-view.h"

#include <QWidget>
#include <QVBoxLayout>
#include <QScrollBar>
#include <QLabel>
#include <QStringList>
#include <QScrollArea>
#include "pushbutton.h"
namespace Peony {

class TabletAppEntity;

class StudyDirectoryWidget : public QWidget
{
    Q_OBJECT
public:
    explicit StudyDirectoryWidget(QStringList &strListTitleStyle, QList<QPair<QString, QList<TabletAppEntity*>>> &subtitleMap, int mode = 0, QWidget *parent = nullptr);
    ~StudyDirectoryWidget();
    void updateAppData(QList<QPair<QString, QList<TabletAppEntity*>>> &subAppMap);
    void setSize();
private:
    //主界面
    QVBoxLayout*  m_mainLayout=nullptr;
    QVBoxLayout*  m_scrollAreaWidLayout=nullptr;
    QScrollArea*  m_scrollArea=nullptr;
    QWidget*      m_scrollareawid=nullptr;
    QLabel*       m_titleLabel=nullptr;
    QStringList   m_strListTitleStyle;
    QMap<QString, StudyListView*> m_viewMap;
    QList<QPair<QString, QList<TabletAppEntity*>>> m_studyCenterDataList;
    QMap<QString, TitleWidget*> m_titleMap;

    int m_iMode;
    QColor m_colorMask;

protected:
    void initWidget(QStringList &strListTitleStyle);
    //初始化应用列表界面
    void initAppListWidget();
    void resizeScrollAreaControls();

    void paintEvent(QPaintEvent *);

private Q_SLOTS:
    //执行应用程序
    void execApplication(QModelIndex appname);

Q_SIGNALS:
    void changeTheme( QString);
};
}

#endif // STUDYDIRECTORYWIDGET_H
