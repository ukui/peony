#ifndef PERMISSIONSPROPERTIESPAGE_H
#define PERMISSIONSPROPERTIESPAGE_H

#include <QWidget>

#include "peony-core_global.h"
#include <memory>
#include <gio/gio.h>

class QLabel;
class QTableWidget;
class QCheckBox;

namespace Peony {

class FileWatcher;

class PEONYCORESHARED_EXPORT PermissionsPropertiesPage : public QWidget
{
    Q_OBJECT
public:
    explicit PermissionsPropertiesPage(const QStringList &uris, QWidget *parent = nullptr);
    ~PermissionsPropertiesPage();

protected:
    static GAsyncReadyCallback async_query_permisson_callback(GObject *obj,
                                                              GAsyncResult *res,
                                                              PermissionsPropertiesPage *p_this);

    void queryPermissionsAsync(const QString&, const QString &uri);

protected Q_SLOTS:
    void changePermission(int row, int column, bool checked);

Q_SIGNALS:
    void checkBoxChanged(int row, int column, bool checked);

private:
    QString m_uri;
    std::shared_ptr<FileWatcher> m_watcher;

    QLabel *m_label;
    QLabel *m_message;
    QTableWidget *m_table;

    bool m_permissions[3][3];
};

}

#endif // PERMISSIONSPROPERTIESPAGE_H
