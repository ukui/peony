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

#include "file-operation.h"

using namespace Peony;

FileOperation::FileOperation(QObject *parent) : QObject (parent)
{
    m_cancellable_wrapper = wrapGCancellable(g_cancellable_new());
    setAutoDelete(true);
}

FileOperation::~FileOperation()
{

}

void FileOperation::run()
{

}

void FileOperation::cancel()
{
    g_cancellable_cancel(m_cancellable_wrapper.get()->get());
    m_is_cancelled = true;
}
