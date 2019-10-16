#ifndef ICONVIEWSTYLE_H
#define ICONVIEWSTYLE_H

#include <QProxyStyle>

namespace Peony {

namespace DirectoryView {

/*!
 * \brief The IconViewStyle class
 * \details
 * This class is aim to provide a custom style for IconView's items.
 * In Qt5, item view draw its items from styled delegate by default,
 * even though we can implement a item's painting in delegate's paint().
 * I think it is better to follow Qt's desgin.
 * I mainly use this class control the internal layout of a item. It will
 * combine with delegate's paint() method.
 */
class IconViewStyle : public QProxyStyle
{
    Q_OBJECT
public:
    static IconViewStyle *getStyle();
    void release();

    void drawControl(QStyle::ControlElement element,
                     const QStyleOption *option,
                     QPainter *painter,
                     const QWidget *widget = nullptr) const override;

    void drawItemPixmap(QPainter *painter,
                        const QRect &rect,
                        int alignment,
                        const QPixmap &pixmap) const override;

    void drawItemText(QPainter *painter,
                      const QRect &rect,
                      int flags,
                      const QPalette &pal,
                      bool enabled,
                      const QString &text,
                      QPalette::ColorRole textRole = QPalette::NoRole) const override;

private:
    explicit IconViewStyle(QStyle *style = nullptr);
    ~IconViewStyle() override {}
};

}

}

#endif // ICONVIEWSTYLE_H
