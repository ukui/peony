#ifndef LISTVIEW_H
#define LISTVIEW_H

#include <QTreeView>
#include "directory-view-plugin-iface.h"
#include "peony-core_global.h"

#include <QTimer>

namespace Peony {

class FileItemModel;
class FileItemProxyFilterSortModel;

namespace DirectoryView {

class ListView : public QTreeView, public DirectoryViewIface
{
    Q_OBJECT
public:
    explicit ListView(QWidget *parent = nullptr);

    const QString viewId() override {return tr("List View");}

    void bindModel(FileItemModel *sourceModel, FileItemProxyFilterSortModel *proxyModel) override;
    void setProxy(DirectoryViewProxyIface *proxy) override;

    DirectoryViewProxyIface *getProxy() override;

    //location
    const QString getDirectoryUri() override;

    //selections
    const QStringList getSelections() override;

    //children
    const QStringList getAllFileUris() override;

    //QRect visualRect(const QModelIndex &index) const override;

public Q_SLOTS:
    //location
    void open(const QStringList &uris, bool newWindow) override;
    void setDirectoryUri(const QString &uri) override;
    void beginLocationChange() override;
    void stopLocationChange() override;
    void closeView() override;

    //selections
    void setSelections(const QStringList &uris) override;
    void invertSelections() override;
    void scrollToSelection(const QString &uri) override;

    //clipboard
    void setCutFiles(const QStringList &uris) override;

    int getSortType() override;
    void setSortType(int sortType) override;

    int getSortOrder() override;
    void setSortOrder(int sortOrder) override;

    void editUri(const QString &uri) override;
    void editUris(const QStringList uris) override;

private:
    FileItemModel *m_model = nullptr;
    FileItemProxyFilterSortModel *m_proxy_model = nullptr;

    QTimer m_edit_trigger_timer;

    DirectoryViewProxyIface *m_proxy = nullptr;

    QString m_current_uri;
};

}

}

#endif // LISTVIEW_H
