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

#include "path-edit.h"
#include "path-bar-model.h"
#include "path-completer.h"

#include <QKeyEvent>
#include <QAction>

#include <QDebug>

using namespace Peony;

PathEdit::PathEdit(QWidget *parent) : QLineEdit(parent)
{
    setFocusPolicy(Qt::ClickFocus);

    m_model = new PathBarModel(this);
    m_completer = new PathCompleter(this);
    m_completer->setModel(m_model);
    m_completer->setCaseSensitivity(Qt::CaseInsensitive);

    setLayoutDirection(Qt::LeftToRight);

    QAction *goToAction = new QAction(QIcon::fromTheme("forward"), tr("Go To"), this);
    addAction(goToAction, QLineEdit::TrailingPosition);

    connect(goToAction, &QAction::triggered, this, &QLineEdit::returnPressed);

    setCompleter(m_completer);

    connect(this, &QLineEdit::returnPressed, [=]{
        if (this->text().isEmpty()) {
            this->setText(m_last_uri);
            this->editCancelled();
            return;
        } else {
            qDebug()<<"change dir request"<<this->text();
            Q_EMIT this->uriChangeRequest(this->text());
            //NOTE: we have send the signal for location change.
            //so we can use editCancelled hide the path edit.
            this->editCancelled();
        }
    });
}

void PathEdit::setUri(const QString &uri)
{
    m_last_uri = uri;
    setText(uri);
}

void PathEdit::focusOutEvent(QFocusEvent *e)
{
    QLineEdit::focusOutEvent(e);
    Q_EMIT editCancelled();
}

void PathEdit::focusInEvent(QFocusEvent *e)
{
    QLineEdit::focusInEvent(e);
    m_model->setRootUri(this->text());
    m_completer->complete();
}

void PathEdit::keyPressEvent(QKeyEvent *e)
{
    QLineEdit::keyPressEvent(e);
    if (e->key() == Qt::Key_Escape) {
        Q_EMIT editCancelled();
    }
}
