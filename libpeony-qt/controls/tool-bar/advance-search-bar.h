#ifndef ADVANCE_SEARCH_BAR_H
#define ADVANCE_SEARCH_BAR_H

#include "advanced-location-bar.h"

#include <QWidget>
#include <QLineEdit>
#include <QComboBox>

namespace Peony {

class FMWindow;

class AdvanceSearchBar : public QWidget
{
    Q_OBJECT
public:
    explicit AdvanceSearchBar(FMWindow *window, QWidget *parent = nullptr);

Q_SIGNALS:

public Q_SLOTS:
    void clearData();
    void browsePath();
    void searchFilter();
    void filterUpdate();
    void setShowHidden();
    void updateLocation(const QString uri);

public:
    //advance search filter options
    QStringList m_file_type_list = {tr("all"), tr("file folder"), tr("image"), tr("video"), tr("text file"), tr("audio"), tr("others")};
    QStringList m_file_mtime_list = {tr("all"), tr("today"), tr("this week"), tr("this month"), tr("this year"), tr("year ago")};
    QStringList m_file_size_list = {tr("all"), tr("tiny(0-16K)"), tr("small(16k-1M)"), tr("medium(1M-100M)"), tr("big(100M-1G)"),tr("large(>1G)")};

protected:
    void init(bool hasTopWindow);

private:
    FMWindow *m_top_window;

    QWidget *m_filter;
    QLineEdit *m_advanced_key;
    QComboBox *typeViewCombox, *timeViewCombox, *sizeViewCombox;
    AdvancedLocationBar *m_advance_bar;

    QString m_advance_target_path;
};

}

#endif // ADVANCE_SEARCH_BAR_H
