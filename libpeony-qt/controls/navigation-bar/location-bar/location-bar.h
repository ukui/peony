/*
 * Peony-Qt's Library
 *
 * Copyright (C) 2019, Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Authors: Yue Lan <lanyue@kylinos.cn>
 *
 */

#ifndef LOCATIONBAR_H
#define LOCATIONBAR_H

#include <QToolBar>
#include "peony-core_global.h"

class QLineEdit;

namespace Peony {

class PEONYCORESHARED_EXPORT LocationBar : public QToolBar
{
    Q_OBJECT
public:
    explicit LocationBar(QWidget *parent = nullptr);
    ~LocationBar() override;
    const QString getCurentUri() {return m_current_uri;}

Q_SIGNALS:
    void groupChangedRequest(const QString &uri);
    void blankClicked();

public Q_SLOTS:
    void setRootUri(const QString &uri);

protected:
    void addButton(const QString &uri, bool setIcon = false, bool setMenu = true);

    void mousePressEvent(QMouseEvent *e) override;
    void paintEvent(QPaintEvent *e) override;

private:
    QString m_current_uri;
    QLineEdit *m_styled_edit;
};

}

#endif // LOCATIONBAR_H
