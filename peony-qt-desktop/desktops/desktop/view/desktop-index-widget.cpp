/*
 * Peony-Qt
 *
 * Copyright (C) 2019, Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Authors: Yue Lan <lanyue@kylinos.cn>
 *
 */

#include "icon-view-delegate.h"
#include "desktop-index-widget.h"

#include "desktop-icon-view-delegate.h"
#include "desktop-icon-view.h"

#include <QPainter>
#include <QStyle>
#include <QApplication>

#include <QTextEdit>
#include <QTextOption>

#include <QMouseEvent>
#include <QStyleHints>

#include <QDebug>

//qt's global function
extern void qt_blurImage(QImage &blurImage, qreal radius, bool quality, int transposed);

using namespace Peony;

DesktopIndexWidget::DesktopIndexWidget(DesktopIconViewDelegate *delegate,
                                       const QStyleOptionViewItem &option,
                                       const QModelIndex &index,
                                       QWidget *parent) : QWidget(parent)
{
    setContentsMargins(0, 0, 0, 0);
    m_option = option;

    m_index = index;
    m_delegate = delegate;

    updateItem();

    //FIXME: how to handle it in old version?
#if (QT_VERSION >= QT_VERSION_CHECK(5, 11, 0))
    connect(qApp, &QApplication::fontChanged, this, [=]() {
        m_delegate->getView()->setIndexWidget(m_index, nullptr);
    });
#endif

    auto view = m_delegate->getView();
    view->m_real_do_edit = false;
    view->m_edit_trigger_timer.stop();
    view->m_edit_trigger_timer.start();
}

DesktopIndexWidget::~DesktopIndexWidget()
{

}

bool DesktopIndexWidget::eventFilter(QObject *watched, QEvent *event)
{
    switch (event->type()) {
    case QEvent::ApplicationFontChange:
    case QEvent::FontChange: {
        updateItem();
        break;
    }
    default:
        break;
    }
    return false;
}

void DesktopIndexWidget::paintEvent(QPaintEvent *e)
{
    auto view = m_delegate->getView();
    //qDebug()<<"paint";
    auto visualRect = m_delegate->getView()->visualRect(m_index);
    if (this->pos() != visualRect.topLeft()) {
        move(visualRect.topLeft());
        return;
    }

    if (!view->selectionModel()->selectedIndexes().contains(m_index)) {
        view->m_real_do_edit = false;
        view->m_edit_trigger_timer.stop();
        this->close();
        return;
    }

    Q_UNUSED(e)
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    auto bgColor = m_option.palette.highlight().color();
    p.save();
    p.setPen(Qt::transparent);
    bgColor.setAlpha(255*0.7);
    p.setBrush(bgColor);
    p.drawRoundedRect(this->rect(), 6, 6);
    p.restore();

    //auto font = view->getViewItemFont(&m_option);

    auto opt = m_option;
    auto iconSizeExcepted = m_delegate->getView()->iconSize();
    auto iconRect = QApplication::style()->subElementRect(QStyle::SE_ItemViewItemDecoration, &opt, opt.widget);
    int y_delta = iconSizeExcepted.height() - iconRect.height();
    opt.rect.moveTo(opt.rect.x(), opt.rect.y() + y_delta);

    int maxTextHight = this->height() - iconSizeExcepted.height() - 10;

    //setFixedHeight(opt.rect.height() + y_delta);

    //draw icon shadow
    p.save();

    auto iconSize = opt.decorationSize;
    QPixmap iconPixmap(opt.icon.pixmap(iconSize));
    QPainter iconShadowPainter(&iconPixmap);
    iconShadowPainter.end();

    QImage iconShadowImage(iconSize + QSize(10,10), QImage::Format_ARGB32_Premultiplied);
    iconShadowImage.fill(Qt::transparent);
    iconShadowPainter.begin(&iconShadowImage);
    auto tmpRect = opt.rect;
    opt.rect = QRect(-(iconRect.x() - opt.rect.x()) + 5,
                     -(iconRect.y() - opt.rect.y()) + 5,
                     opt.rect.width(),
                     opt.rect.height());
    auto tmpText = opt.text;
    opt.text = nullptr;

    QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &opt, &iconShadowPainter, opt.widget);
    iconShadowPainter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    iconShadowPainter.fillRect(iconShadowImage.rect(),QColor(0,0,0,80));

    opt.rect = tmpRect;
    opt.text = tmpText;
    qt_blurImage(iconShadowImage, 10, false, false);

    for (int x = 0; x < iconShadowImage.width(); x++) {
        for (int y = 0; y < iconShadowImage.height(); y++) {
            auto color = iconShadowImage.pixelColor(x, y);
            if (color.alpha() > 0) {
                color.setAlphaF(qMin(color.alphaF() * 1.5, 1.0));
                iconShadowImage.setPixelColor(x, y, color);
            }
        }
    }

    iconShadowPainter.end();
    p.drawImage(opt.rect.x() + (iconRect.x() - opt.rect.x()) - 5,
                      opt.rect.y() + (iconRect.y() - opt.rect.y()) - 5,
                     iconShadowImage);
    p.restore();

    // draw icon
    opt.text = nullptr;
//    p.save();
//    p.setRenderHint(QPainter::Antialiasing);
//    p.setRenderHint(QPainter::SmoothPixmapTransform);
//    if(view->m_animation->state() == QAbstractAnimation::Running){
//        double currentValue = view->m_animation->currentValue().toDouble();
//        p.scale(currentValue, currentValue);
//        int offsetX = opt.decorationSize.width();
//        int offsetY = opt.decorationSize.height();
//        offsetX /= -2;
//        offsetY /= -2;
//        p.translate(offsetX*(currentValue - 1), offsetY*(currentValue - 1));
//    }
    QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &opt, &p, m_delegate->getView());
//    p.restore();

    p.save();
    p.translate(0, 5 + m_delegate->getView()->iconSize().height() + 5);

    if (b_elide_text)
    {
        int  charWidth = opt.fontMetrics.averageCharWidth();
        m_option.text = opt.fontMetrics.elidedText(m_option.text, Qt::ElideRight, ELIDE_TEXT_LENGTH * charWidth);
    }
    // draw text shadow
    p.save();

    auto expectedSize = Peony::DirectoryView::IconViewTextHelper::getTextSizeForIndex(m_option, m_index, 2);
    QPixmap pixmap(expectedSize);
    pixmap.fill(Qt::transparent);
    QPainter shadowPainter(&pixmap);
    shadowPainter.setPen(Qt::black);
    Peony::DirectoryView::IconViewTextHelper::paintText(&shadowPainter, m_option, m_index, maxTextHight, 2, 4, false, Qt::black);
    shadowPainter.end();

    QImage shadowImage(expectedSize + QSize(4, 4), QImage::Format_ARGB32_Premultiplied);
    shadowImage.fill(Qt::transparent);
    shadowPainter.begin(&shadowImage);
    shadowPainter.drawPixmap(2, 2, pixmap);
    qt_blurImage(shadowImage, 8, false, false);

    for (int x = 0; x < shadowImage.width(); x++) {
        for (int y = 0; y < shadowImage.height(); y++) {
            auto color = shadowImage.pixelColor(x, y);
            if (color.alpha() > 0) {
                color.setAlphaF(qMin(color.alphaF() * 1.5, 1.0));
                shadowImage.setPixelColor(x, y, color);
            }
        }
    }

    shadowPainter.end();

    p.translate(-3, -1);
    p.drawImage(0, 0, shadowImage);

    p.restore();

    // draw text
    p.setPen(m_option.palette.highlightedText().color());
    p.setFont(qApp->font());
    Peony::DirectoryView::IconViewTextHelper::paintText(&p,
            m_option,
            m_index,
            maxTextHight,
            2,
            4,
            false);

    p.restore();

    bgColor.setAlpha(255*0.8);
    p.setPen(bgColor);
    p.drawRoundedRect(this->rect().adjusted(0, 0, -1, -1), 6, 6);
}

void DesktopIndexWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        auto view = m_delegate->getView();
        view->m_real_do_edit = true;
        if (view->m_edit_trigger_timer.isActive()) {
            if (view->m_edit_trigger_timer.remainingTime() < 3000 - qApp->styleHints()->mouseDoubleClickInterval() && view->m_edit_trigger_timer.remainingTime() > 0) {
                QTimer::singleShot(300, this, [=]() {
                    if (view->m_real_do_edit) {
                        //not allow to edit special items:computer,trash and personal home path folder name
                        bool special_index = false;
                        QString homeUri = "file://" + QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
                        special_index = (m_index.data(Qt::UserRole).toString() == "computer:///" ||
                                         m_index.data(Qt::UserRole).toString() == "trash:///" ||
                                         m_index.data(Qt::UserRole).toString() == homeUri);
                        if (special_index)
                            return ;

                        if (!view->selectionModel()->selectedIndexes().contains(m_index)) {
                            view->m_real_do_edit = false;
                            view->m_edit_trigger_timer.stop();
                            this->close();
                            return;
                        }

                        view->setIndexWidget(m_index, nullptr);
                        view->edit(m_index);
                    }
                });
            } else {
                view->m_real_do_edit = false;
                view->m_edit_trigger_timer.stop();
            }
        } else {
            view->m_real_do_edit = false;
            view->m_edit_trigger_timer.start();
        }
        event->accept();
        return;
//        if (m_edit_trigger.isActive()) {
//            qDebug()<<"IconViewIndexWidget::mousePressEvent: edit"<<e->type();
//            m_delegate->getView()->setIndexWidget(m_index, nullptr);
//            m_delegate->getView()->edit(m_index);
//            return;
//        }
    }
    QWidget::mousePressEvent(event);
}

void DesktopIndexWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    auto view = m_delegate->getView();
    if (!view->selectionModel()->selectedIndexes().contains(m_index)) {
        view->m_real_do_edit = false;
        view->m_edit_trigger_timer.stop();
        this->close();
        return;
    }

    m_delegate->getView()->activated(m_index);
    m_delegate->getView()->setIndexWidget(m_index, nullptr);
    return;
}

void DesktopIndexWidget::updateItem()
{
    auto view = m_delegate->getView();
    m_option = view->viewOptions();
    m_option.font = qApp->font();
    m_option.fontMetrics = qApp->fontMetrics();
    m_delegate->initStyleOption(&m_option, m_index);
    QSize size = m_delegate->sizeHint(m_option, m_index);
    auto visualRect = m_delegate->getView()->visualRect(m_index);
    auto rectCopy = visualRect;
    move(visualRect.topLeft());
    setFixedWidth(visualRect.width());

    m_option.rect.setWidth(visualRect.width());

    int rawHeight = size.height();
    auto textSize = Peony::DirectoryView::IconViewTextHelper::getTextSizeForIndex(m_option, m_index, 2);
    int fixedHeight = 5 + m_delegate->getView()->iconSize().height() + 5 + textSize.height() + 10;

    int y_bottom = rectCopy.y() + fixedHeight;
    qDebug() << "Y:" <<rectCopy.y() <<fixedHeight <<m_delegate->getView()->height();
    b_elide_text = false;
    if ( y_bottom > m_delegate->getView()->height() && m_option.text.length() > ELIDE_TEXT_LENGTH)
    {
        b_elide_text = true;
        int  charWidth = m_option.fontMetrics.averageCharWidth();
        m_option.text = m_option.fontMetrics.elidedText(m_option.text, Qt::ElideRight, ELIDE_TEXT_LENGTH * charWidth);
        //recount size
        textSize = Peony::DirectoryView::IconViewTextHelper::getTextSizeForIndex(m_option, m_index, 2);
        fixedHeight = 5 + m_delegate->getView()->iconSize().height() + 5 + textSize.height() + 10;
    }

    qDebug() << "updateItem fixedHeight:" <<fixedHeight <<rawHeight <<m_option.text;
    if (fixedHeight < rawHeight)
        fixedHeight = rawHeight;

    m_option.text = m_index.data().toString();
    //qDebug()<<m_option.text;
    //m_option.features.setFlag(QStyleOptionViewItem::WrapText);
    m_option.features |= QStyleOptionViewItem::WrapText;
    m_option.textElideMode = Qt::ElideNone;

    //m_option.rect.setHeight(9999);
    m_option.rect.moveTo(0, 0);

    //qDebug()<<m_option.rect;
    //auto font = view->getViewItemFont(&m_option);

    auto rawTextRect = QApplication::style()->subElementRect(QStyle::SE_ItemViewItemText, &m_option, m_option.widget);
    auto iconSizeExcepted = m_delegate->getView()->iconSize();

    auto iconRect = QApplication::style()->subElementRect(QStyle::SE_ItemViewItemDecoration, &m_option, m_option.widget);

    auto y_delta = iconSizeExcepted.height() - iconRect.height();

    rawTextRect.setTop(iconRect.bottom() + y_delta + 5);
    rawTextRect.setHeight(9999);

    setFixedHeight(fixedHeight);
}
