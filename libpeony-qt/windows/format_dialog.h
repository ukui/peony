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
 * Authors: ZJAY998 <zhangjun@kylinos.cn>
 *
 */

#ifndef FORMAT_DIALOG_H
#define FORMAT_DIALOG_H

#include <QDialog>
#include <sys/stat.h>
#include <udisks/udisks.h>
#include <libnotify/notify.h>
#include <glib/gi18n.h>
#include <QTimer>
#include <errno.h>

#include "side-bar-menu.h"
#include "volume-manager.h"
#include "file-utils.h"
#include "side-bar-abstract-item.h"
namespace Ui {
class Format_Dialog;
}

namespace Peony {
class SideBarAbstractItem;
}

class Format_Dialog;

using namespace Peony;


struct CreateformatData{
    UDisksObject *object,*drive_object;
    UDisksBlock *block,*drive_block;
    UDisksClient *client;
    const gchar *format_type;
    const gchar *device_name;
    const gchar *erase_type;
    const gchar *filesystem_name;
    int *format_finish;
    Format_Dialog *dl;
};


class Format_Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Format_Dialog(const QString &uris,SideBarAbstractItem *m_item,QWidget *parent = nullptr);

    gboolean is_iso(const gchar *device_path);

    void ensure_unused_cb(CreateformatData *data);

    void ensure_format_cb (CreateformatData *data);

    void ensure_format_disk(CreateformatData *data);

    UDisksObject *get_object_from_block_device 	(UDisksClient *client,
                                                 const gchar *block_device);

    void kdisk_format(const gchar * device_name,const gchar *format_type,
                      const gchar * erase_type,const gchar * filesystem_name,int *format_finish);

    void cancel_format(const gchar* device_name);

    double get_format_bytes_done(const gchar * device_name);

    void format_ok_dialog();


    void format_err_dialog();

    bool format_makesure_dialog();
    
    ~Format_Dialog();


    Ui::Format_Dialog *ui;

    QTimer *my_time;

    QWidget *m_parent;

    int m_cost_seconds = 0;

    int m_total_predict = 1200;

    double m_simulate_progress = 0;

Q_SIGNALS:
     void ensure_format(bool flags);

public Q_SLOTS:

    void acceptFormat (bool);
    void colseFormat(bool);

    void formatloop();

private:

    QString fm_uris;
    SideBarAbstractItem *fm_item;

};

#endif // FORMAT_DIALOG_H
