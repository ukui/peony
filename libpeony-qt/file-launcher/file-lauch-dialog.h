/*
 * Peony-Qt's Library
 *
 * Copyright (C) 2020, KylinSoft Co., Ltd.
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

#ifndef FILELAUCHDIALOG_H
#define FILELAUCHDIALOG_H

#include <QDialog>

#include "peony-core_global.h"

#include <QHash>

class QVBoxLayout;
class QListWidget;
class QCheckBox;
class QDialogButtonBox;
class QListWidgetItem;

namespace Peony {

class FileLaunchAction;

/*!
 * \brief The FileLauchDialog class
 * provides the dialog for choosing which application to open a file.
 */
class PEONYCORESHARED_EXPORT FileLauchDialog : public QDialog
{
    Q_OBJECT
public:
    explicit FileLauchDialog(const QString &uri, QWidget *parent = nullptr);

    QSize sizeHint() const override {
        return QSize(400, 600);
    }

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QVBoxLayout *m_layout;

    QListWidget *m_view;
    QCheckBox *m_check_box;
    QDialogButtonBox *m_button_box;

    QString m_uri;
    QHash<QListWidgetItem*, FileLaunchAction*> m_hash;
};

}

#endif // FILELAUCHDIALOG_H
