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

#ifndef UKUICHINESELETTER_H
#define UKUICHINESELETTER_H
#include <QString>


class UkuiChineseLetter
{
public:
    UkuiChineseLetter();
    static bool In(wchar_t start, wchar_t end, wchar_t code);

    static char Convert(int n);

    // 获取第一个汉字的首字母
    static QString getFirstLetter(const QString &src);

    // 获取所有汉字的首字母
    static QString getFirstLetters(const QString &src);

    static QString getFirstLettersAll(const QString &src);

    // 获取一个汉字编码的汉语拼音
    static QString getPinyin(int code);

    // 获取所有汉字的汉语拼音
    static QString getPinyins(const QString& text);
};

#endif // UKUICHINESELETTER_H
