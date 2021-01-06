/*
 * Peony-Qt's Library
 *
 * Copyright (C) 2019, Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Authors: Yue Lan <lanyue@kylinos.cn>
 *
 */

#include "location-bar.h"

#include "path-bar-model.h"
#include "file-utils.h"

#include "search-vfs-uri-parser.h"

#include "fm-window.h"

#include <QUrl>
#include <QMenu>

#include <QMouseEvent>
#include <QPainter>
#include <QStyleOptionFocusRect>
#include <QStyleOptionFrame>
#include <QLineEdit>

#include <QStandardPaths>
#include <QApplication>
#include <QClipboard>

#include <QHBoxLayout>
#include <QToolButton>

#include <QProxyStyle>
#include <QStyleOptionToolButton>

using namespace Peony;

class LocationBarButtonStyle;

static LocationBarButtonStyle *buttonStyle = nullptr;

class LocationBarButtonStyle : public QProxyStyle
{
public:
    explicit LocationBarButtonStyle() : QProxyStyle() {}
    static LocationBarButtonStyle *getStyle() {
        if (!buttonStyle) {
            buttonStyle = new LocationBarButtonStyle;
        }
        return buttonStyle;
    }

    void polish(QWidget *widget) override;
    void unpolish(QWidget *widget) override;
    void drawComplexControl(ComplexControl control, const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget = nullptr) const override;
};

LocationBar::LocationBar(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_Hover);
    setMouseTracking(true);

    setToolTip(tr("click the blank area for edit"));

    setStyleSheet("padding-right: 15;"
                  "margin-left: 2");
    m_styled_edit = new QLineEdit;
    qDebug()<<sizePolicy();
    //connect(this, &LocationBar::groupChangedRequest, this, &LocationBar::setRootUri);

    m_layout = new QHBoxLayout;
    setLayout(m_layout);

    m_indicator = new QToolButton(this);
    m_indicator->setFocusPolicy(Qt::FocusPolicy(m_indicator->focusPolicy() & ~Qt::TabFocus));
    m_indicator->setAutoRaise(true);
    m_indicator->setStyle(LocationBarButtonStyle::getStyle());
    m_indicator->setPopupMode(QToolButton::InstantPopup);
    m_indicator->setArrowType(Qt::RightArrow);
    m_indicator->setCheckable(true);
    m_indicator->setFixedSize(this->height() - 2, this->height() - 2);
    m_indicator->move(-2, 1);

    m_indicator_menu = new QMenu(m_indicator);
    m_indicator->setMenu(m_indicator_menu);

    connect(m_indicator_menu, &QMenu::aboutToShow, this, [=](){
        m_indicator->setArrowType(Qt::DownArrow);
    });

    connect(m_indicator_menu, &QMenu::aboutToHide, this, [=](){
        m_indicator->setArrowType(Qt::RightArrow);
    });
}

LocationBar::~LocationBar()
{
    m_styled_edit->deleteLater();
}

void LocationBar::setRootUri(const QString &uri)
{
    if (m_current_uri == uri)
        return;

    m_current_uri = uri;

    //clear buttons
    clearButtons();

    if (m_current_uri.startsWith("search://")) {
        //qDebug() <<"location-bar setRootUri:" <<uri;
        //QString nameRegexp = SearchVFSUriParser::getSearchUriNameRegexp(m_current_uri);
        //QString targetDirectory = SearchVFSUriParser::getSearchUriTargetDirectory(m_current_uri);
        m_indicator->setArrowType(Qt::NoArrow);
        addButton(m_current_uri, false, false);
        //addAction(QIcon::fromTheme("edit-find-symbolic"), tr("Search \"%1\" in \"%2\"").arg(nameRegexp).arg(targetDirectory));
        return;
    }

    QStringList uris;
    QString tmp = uri;
    while (!tmp.isEmpty()) {
        uris.prepend(tmp);
        QUrl url = tmp;
        //FIXME: replace BLOCKING api in ui thread.
        if (FileUtils::isMountRoot(tmp))
            break;

//        if (url.path() == QStandardPaths::writableLocation(QStandardPaths::HomeLocation)) {
//            break;
//        }
        tmp = Peony::FileUtils::getParentUri(tmp);
    }

    m_indicator->setArrowType(Qt::RightArrow);
    for (auto uri : uris) {
        //addButton(uri, uri != uris.last());
        addButton(uri, uris.first() == uri);
    }

    doLayout();
}

void LocationBar::clearButtons()
{
    for (auto button : m_buttons) {
        button->hide();
        button->deleteLater();
    }

    m_buttons.clear();
}

void LocationBar::addButton(const QString &uri, bool setIcon, bool setMenu)
{
    setIcon = true;
    QToolButton *button = new QToolButton(this);
    button->setFocusPolicy(Qt::FocusPolicy(button->focusPolicy() & ~Qt::TabFocus));
    button->setAutoRaise(true);
    button->setStyle(LocationBarButtonStyle::getStyle());
    button->setProperty("uri", uri);
    button->setFixedHeight(this->height());
    button->setIconSize(QSize(16, 16));
    button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    button->setPopupMode(QToolButton::MenuButtonPopup);

    //FIXME: replace BLOCKING api in ui thread.
    auto displayName = FileUtils::getFileDisplayName(uri);
    m_buttons.insert(uri, button);
    if (m_current_uri.startsWith("search://")) {
        QString nameRegexp = SearchVFSUriParser::getSearchUriNameRegexp(m_current_uri);
        QString targetDirectory = SearchVFSUriParser::getSearchUriTargetDirectory(m_current_uri);
        button->setIcon(QIcon::fromTheme("edit-find-symbolic"));
        displayName = tr("Search \"%1\" in \"%2\"").arg(nameRegexp).arg(targetDirectory);
        button->setText(displayName);
        button->setFixedWidth(button->sizeHint().width());
        return;
    }

    QUrl url = uri;

    auto parent = FileUtils::getParentUri(uri);
    if (setIcon) {
        //FIXME: replace BLOCKING api in ui thread.
        QIcon icon = QIcon::fromTheme(Peony::FileUtils::getFileIconName(uri), QIcon::fromTheme("folder"));
        button->setIcon(icon);
    }

    if (!url.fileName().isEmpty()) {
        if (FileUtils::getParentUri(uri).isNull()) {
            setMenu = false;
        }
        button->setText(url.fileName());
    } else {
        if (uri == "file:///") {
            auto text = FileUtils::getFileDisplayName("computer:///root.link");
            if (text.isNull()) {
                text = tr("File System");
            }
            button->setText(text);
        } else {
            button->setText(displayName);
        }
    }

    //if button text is too long, elide it
    displayName = button->text();
    if (displayName.length() > ELIDE_TEXT_LENGTH)
    {
        int  charWidth = fontMetrics().averageCharWidth();
        displayName = fontMetrics().elidedText(displayName, Qt::ElideRight, ELIDE_TEXT_LENGTH * charWidth);
    }
    button->setText(displayName);

    connect(button, &QToolButton::clicked, [=]() {
        //this->setRootUri(uri);
        Q_EMIT this->groupChangedRequest(uri);
    });

    if (setMenu) {
        Peony::PathBarModel m;
        m.setRootUri(uri);
        m.sort(0);

        auto suburis = m.stringList();
        if (!suburis.isEmpty()) {
            QMenu *menu = new QMenu;
            connect(button, &QToolButton::destroyed, menu, &QMenu::deleteLater);
            const int WIDTH_EXTEND = 5;
            connect(menu, &QMenu::aboutToShow, this, [=](){
                menu->setMinimumWidth(button->width() + WIDTH_EXTEND);
            });
            QList<QAction *> actions;
            for (auto uri : suburis) {
                QUrl url = uri;
                QString tmp = uri;
                displayName = url.fileName();
                if (displayName.length() > ELIDE_TEXT_LENGTH)
                {
                    int  charWidth = fontMetrics().averageCharWidth();
                    displayName = fontMetrics().elidedText(displayName, Qt::ElideRight, ELIDE_TEXT_LENGTH * charWidth);
                }
                QIcon icon = QIcon::fromTheme(Peony::FileUtils::getFileIconName(uri), QIcon::fromTheme("folder"));
                QAction *action = new QAction(icon, displayName, this);
                actions<<action;
                connect(action, &QAction::triggered, [=]() {
                    Q_EMIT groupChangedRequest(tmp);
                });
            }
            menu->addActions(actions);

            button->setMenu(menu);
        } else {
            // no subdir directory should not display an indicator arrow.
            button->setPopupMode(QToolButton::InstantPopup);
        }
    }

    button->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(button, &QWidget::customContextMenuRequested, this, [=](){
        QMenu menu;
        FMWindowIface *windowIface = dynamic_cast<FMWindowIface *>(this->topLevelWidget());
        auto copy = menu.addAction(QIcon::fromTheme("edit-copy-symbolic"), tr("&Copy Directory"));

        menu.addAction(QIcon::fromTheme("tab-new-symbolic"), tr("Open In New &Tab"), [=](){
            windowIface->addNewTabs(QStringList()<<uri);
        });

        menu.addAction(QIcon::fromTheme("window-new-symbolic"), tr("Open In &New Window"), [=](){
            auto newWindow = windowIface->create(uri);
            dynamic_cast<QWidget *>(newWindow)->show();
        });

        if (copy == menu.exec(QCursor::pos())) {
            if (uri.startsWith("file://")) {
                QUrl url = uri;
                QApplication::clipboard()->setText(url.path());
            } else {
                QApplication::clipboard()->setText(uri);
            }
        }
    });
}

void LocationBar::mousePressEvent(QMouseEvent *e)
{
    //eat this event.
    //QToolBar::mousePressEvent(e);
    qDebug()<<"black clicked";
    if (e->button() == Qt::LeftButton) {
        Q_EMIT blankClicked();
    }
}

void LocationBar::paintEvent(QPaintEvent *e)
{
    //QToolBar::paintEvent(e);

    QPainter p(this);
    QStyleOptionFocusRect opt;
    opt.initFrom(this);

    QStyleOptionFrame fopt;
    fopt.initFrom(this);
    fopt.state |= QStyle::State_HasFocus;
    //fopt.state.setFlag(QStyle::State_HasFocus);
    fopt.rect.adjust(-2, 0, 0, 0);
    fopt.palette.setColor(QPalette::Highlight, fopt.palette.base().color());
    fopt.palette.setColor(QPalette::Base, fopt.palette.window().color());

    style()->drawPrimitive(QStyle::PE_PanelLineEdit, &fopt, &p, this);

    style()->drawControl(QStyle::CE_ToolBar, &opt, &p, this);
}

void LocationBar::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    doLayout();
}

void LocationBar::doLayout()
{
    m_indicator->setVisible(false);

    QList<int> sizeHints;

    m_indicator_menu->clear();

    for (auto button : m_buttons) {
        button->setVisible(true);
        button->resize(button->sizeHint().width(), button->height());
        button->setToolButtonStyle(Qt::ToolButtonTextOnly);
        button->adjustSize();
        sizeHints<<button->sizeHint().width();
        button->setVisible(false);
    }

    int totalWidth = this->width();
    int currentWidth = 0;
    int visibleButtonCount = 0;
    for (int index = sizeHints.count() - 1; index >= 0; index--) {
        int tmp = currentWidth + sizeHints.at(index);
        if (tmp <= totalWidth) {
            visibleButtonCount++;
            currentWidth = tmp;
        } else {
            break;
        }
    }

    int offset = 0;

    bool indicatorVisible = visibleButtonCount < sizeHints.count();
    if (indicatorVisible) {
        m_indicator->setVisible(true);
        offset += m_indicator->width();
    } else {
        m_indicator->setVisible(false);
    }

    for (int index = sizeHints.count() - visibleButtonCount; index < sizeHints.count(); index++) {
        auto button = m_buttons.values().at(index);
        button->setVisible(true);
        button->move(offset, 0);
        if (index == sizeHints.count() - visibleButtonCount) {
            button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
            button->adjustSize();
        }
        offset += button->width();
    }

    if (visibleButtonCount == 0 && !m_buttons.isEmpty()) {
        auto button = m_buttons.values().at(sizeHints.count() - 1);
        button->setVisible(true);
        button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        button->resize(totalWidth - 20, button->height());
    }

    int spaceCount = 0;
    QList<QAction *> actions;
    for (auto button : m_buttons) {
        if (button->isVisible()) {
            break;
        }
        auto uri = button->property("uri").toString();
        QString space;
        int i = 0;
        while (i < spaceCount) {
            space.append(' ');
            i++;
        }
        auto action = new QAction(space + button->text(), nullptr);
        actions.append(action);

        connect(action, &QAction::triggered, this, [=](){
            Q_EMIT groupChangedRequest(uri);
        });
        spaceCount++;
    }
    m_indicator_menu->addActions(actions);
}

void LocationBarButtonStyle::polish(QWidget *widget)
{
    QProxyStyle::polish(widget);

    widget->setProperty("useIconHighlightEffect", true);
    widget->setProperty("iconHighLightEffectMode", 1);
}

void LocationBarButtonStyle::unpolish(QWidget *widget)
{
    QProxyStyle::unpolish(widget);

    widget->setProperty("useIconHighlightEffect", QVariant());
    widget->setProperty("iconHighLightEffectMode", QVariant());
}

void LocationBarButtonStyle::drawComplexControl(QStyle::ComplexControl control, const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget) const
{
    if (control == QStyle::CC_ToolButton) {
        auto toolButton = qstyleoption_cast<const QStyleOptionToolButton *>(option);
        auto opt = *toolButton;
        if (toolButton->arrowType == Qt::NoArrow)
            opt.rect.adjust(0, 1, 0, -1);
        else
            opt.rect.adjust(-2, 1, 2, -1);
        return QProxyStyle::drawComplexControl(control, &opt, painter, widget);
    }
    return QProxyStyle::drawComplexControl(control, option, painter, widget);
}
