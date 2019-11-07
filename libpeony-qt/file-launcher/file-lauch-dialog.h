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

class PEONYCORESHARED_EXPORT FileLauchDialog : public QDialog
{
    Q_OBJECT
public:
    explicit FileLauchDialog(const QString &uri, QWidget *parent = nullptr);

    QSize sizeHint() const override {return QSize(400, 600);}
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
