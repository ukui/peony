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
#ifndef PEONYLOG_H
#define PEONYLOG_H

#include<stdio.h>
#include<syslog.h>

/* 打印调试 */
#define PEONY_DESKTOP_LOG_DEBUG(...) \
    do{ \
        openlog("peony-dekstop", LOG_CONS | LOG_PID, LOG_SYSLOG);\
        syslog(LOG_DEBUG, __VA_ARGS__);\
        closelog();\
    }while(0);

/* 打印错误 */
#define PEONY_DESKTOP_LOG_ERROR(...) \
    do{\
        openlog("peony-desktop", LOG_CONS | LOG_PID, LOG_SYSLOG);\
        syslog(LOG_ERR, __VA_ARGS__);\
        closelog();\
    }while(0);

/* 打印警告 */
#define PEONY_DESKTOP_LOG_WARN(...) \
    do{\
        openlog("peony-desktop", LOG_CONS | LOG_PID, LOG_SYSLOG);\
        syslog(LOG_WARNING, __VA_ARGS__);\
        closelog();\
    }while(0);

#endif // PEONYLOG_H
