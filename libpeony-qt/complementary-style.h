#ifndef COMPLEMENTARYSTYLE_H
#define COMPLEMENTARYSTYLE_H

#include <QProxyStyle>
#include "peony-core_global.h"

class QMenu;
class QPushButton;

namespace Peony {

/*!
 * \brief The ComplementaryStyle class
 * \details
 * This class provide a fixed style for painting qt's control
 * with system theme which qt5-gtk2-platformtheme provided.
 *
 * If just use the qpa plugin, there were be some incorrect styled
 * controls. Such as QToolBarButton's indicator.
 *
 * \note
 * If you are not using gtk-theme as default system theme, you should not use this
 * proxy style for painting.
 *
 * \todo
 * add border radius support.
 */
class ComplementaryStyle : public QProxyStyle
{
    Q_OBJECT
public:
    static ComplementaryStyle *getStyle();

    void drawPrimitive(QStyle::PrimitiveElement element,
                       const QStyleOption *option,
                       QPainter *painter,
                       const QWidget *widget = nullptr) const;

    void drawComplexControl(QStyle::ComplexControl cc,
                            const QStyleOptionComplex *opt,
                            QPainter *p,
                            const QWidget *widget = nullptr) const;

private:
    explicit ComplementaryStyle(QStyle *parent = nullptr);
    ~ComplementaryStyle();

    QMenu *m_styled_menu;
    QPushButton *m_styled_button;
};

}

#endif // COMPLEMENTARYSTYLE_H
