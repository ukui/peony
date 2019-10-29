#ifndef SEARCHBAR_H
#define SEARCHBAR_H

#include <QLineEdit>
#include "peony-core_global.h"

class QStringListModel;

namespace Peony {

class SearchBar : public QLineEdit
{
    Q_OBJECT
public:
    explicit SearchBar(QWidget *parent = nullptr);

Q_SIGNALS:
    void searchKeyChanged(const QString &key);
    void searchRequest(const QString &key);

protected:
    void focusInEvent(QFocusEvent *e) override;
    void focusOutEvent(QFocusEvent *e) override;

public Q_SLOTS:
    void clear_search_record();

private:
    QStringListModel *m_model = nullptr;
};

}

#endif // SEARCHBAR_H
