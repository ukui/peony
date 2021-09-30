/*
 * Copyright (C) 2019 Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/&gt;.
 *
 */

#ifndef STUDYDIRECTORYWIDGET_H
#define STUDYDIRECTORYWIDGET_H

#include <QWidget>
#include <QHBoxLayout>
#include <QTableWidget>
#include <QScrollBar>
#include <QLabel>
#include <QPushButton>
#include <QDir>
#include <QProcess>
//#include <QSvgRenderer>
#include <QStringList>
#include <QScrollArea>

//#include "../tablet/src/Interface/ukuimenuinterface.h"
//#include "../tablet/src/RightClickMenu/rightclickmenu.h"
//#include "../tablet/src/Style/style.h"
namespace Peony {

class TabletAppEntity;

class StudyDirectoryWidget : public QWidget
{
    Q_OBJECT
public:
    explicit StudyDirectoryWidget(QStringList &strListTitleStyle, QList<QPair<QString, QList<TabletAppEntity*>>> &subtitleMap, int mode = 0, QWidget *parent = nullptr);
    ~StudyDirectoryWidget();

private:
//    UkuiMenuInterface* pUkuiMenuInterface=nullptr;

//    QSettings *m_setting=nullptr;
    //主界面
    QVBoxLayout*  m_mainLayout=nullptr;
    QVBoxLayout*  m_scrollAreaWidLayout=nullptr;
    QScrollArea*  m_scrollArea=nullptr;
    QWidget*      m_scrollareawid=nullptr;
    QList<QPair<QString, QList<TabletAppEntity*>>> m_studyCenterDataList;
    int m_iMode;
    QSize m_widgetSize;
    QPoint m_widgetPoint;
    QPixmap *m_windowBg;

protected:
    void initWidget(QStringList &strListTitleStyle);
    //初始化应用列表界面
    void initAppListWidget();
    void resizeScrollAreaControls();
    void resizeEvent(QResizeEvent *event);
    void moveEvent(QMoveEvent *event);
    void paintEvent(QPaintEvent *event);

private Q_SLOTS:
    //执行应用程序
    void execApplication(QModelIndex appname);

Q_SIGNALS:
    void changeTheme( QString);
};
}

#endif // STUDYDIRECTORYWIDGET_H
