#ifndef FORMAT_DIALOG_H
#define FORMAT_DIALOG_H

#include <QDialog>
#include <sys/stat.h>
#include <udisks/udisks.h>
#include <libnotify/notify.h>
#include <glib/gi18n.h>
#include <errno.h>

#include "side-bar-menu.h"
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


    UDisksObject *get_object_from_block_device 	(UDisksClient *client,
                                                 const gchar *block_device);


    void kdisk_format(const gchar * device_name,const gchar *format_type,
                      const gchar * erase_type,const gchar * filesystem_name,int *format_finish);

    void cancel_format(const gchar* device_name);


    ~Format_Dialog();

public Q_SLOTS:

    void acceptFormat (bool);
    void colseFormat(bool);



private:
    Ui::Format_Dialog *ui;
    QString fm_uris;
    SideBarAbstractItem *fm_item;

};

#endif // FORMAT_DIALOG_H
