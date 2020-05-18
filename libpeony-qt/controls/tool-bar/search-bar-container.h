#ifndef SEARCH_BAR_CONTAINER_H
#define SEARCH_BAR_CONTAINER_H

#include <QWidget>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QStringListModel>
#include <QListView>
#include <QStringList>

namespace Peony {

class SearchBarContainer : public QWidget
{
    Q_OBJECT
public:
    explicit SearchBarContainer(QWidget *parent = nullptr);

    QSize sizeHint() const override;

    void setPlaceholderText(const QString &content) {
        m_search_box->setPlaceholderText(content);
    }
    void setFocus() {
        m_search_box->setFocus();
    }
    QString text() {
        return m_search_box->text();
    }
    void setText(QString text) {
        m_search_box->setText(text);
    }

    //get user selected index of file type
    int getFilterIndex() {
        return m_filter_box->currentIndex();
    }
    void clearFilter() {
        m_filter_box->setCurrentIndex(0);
    }

Q_SIGNALS:
    void returnPressed();
    void filterUpdate(const int &index);

public Q_SLOTS:
    void onTableClicked(const QModelIndex &index);

private:
    QHBoxLayout *m_layout = nullptr;

    QLineEdit *m_search_box;
    QComboBox *m_filter_box;

    QStringListModel *m_model = nullptr;
    QListView *m_list_view = nullptr;

    QStringList m_file_type_list = {tr("all"), tr("file folder"), tr("image"),
                                    tr("video"), tr("text file"), tr("audio"), tr("others")
                                   };
};

}

#endif // SEARCH_BAR_CONTAINER_H
