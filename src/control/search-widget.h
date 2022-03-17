#ifndef SEARCHWIDGET_H
#define SEARCHWIDGET_H

#include <QWidget>

class QToolButton;
namespace Peony {

class AdvancedLocationBar;

class SearchWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SearchWidget(QWidget *parent = nullptr);
    void searchButtonClicked();
    void setSearchMode(bool mode);
    void updateCloseSearch(QString icon);

Q_SIGNALS:
    void updateLocationRequest(const QString &uri, bool addHistory = true, bool force = true);
    void updateSearchRequest(bool showSearch);
    void refreshRequest();
    void updateFileTypeFilter(const int &index);
    void updateLocation(const QString &uri);
    void cancelEdit();
    void finishEdit();
    void clearSearchBox();
    void changeSearchMode(bool mode);

public Q_SLOTS:
    void startEdit(bool bSearch = false);
    void updateSearchRecursive(bool recursive);
    void closeSearch();
    void setGlobalFlag(bool isGlobal);

private:
    void initAnimation();

    QToolButton *m_closeSearchButton;
    QToolButton *m_searchButton;
    AdvancedLocationBar *m_locationBar;

    bool m_searchMode = false;
    bool m_searchGlobal = false;
    bool m_searchRecursive = true;
};

}

#endif // SEARCHWIDGET_H
