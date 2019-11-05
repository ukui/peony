#ifndef ICONVIEWINDEXWIDGET_H
#define ICONVIEWINDEXWIDGET_H

#include <QWidget>

#include <QModelIndex>
#include <QStyleOptionViewItem>

#include <memory>

namespace Peony {

class FileInfo;

namespace DirectoryView {

class IconViewDelegate;

class IconViewIndexWidget : public QWidget
{
    Q_OBJECT
public:
    explicit IconViewIndexWidget(const IconViewDelegate *delegate,
                                 const QStyleOptionViewItem &option,
                                 const QModelIndex &index,
                                 QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *e) override;

private:
    QStyleOptionViewItem m_option;
    QModelIndex m_index;
    const IconViewDelegate *m_delegate;

    std::weak_ptr<FileInfo> m_info;
};

}

}


#endif // ICONVIEWINDEXWIDGET_H
