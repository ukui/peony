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

#include "file-info.h"
#include "file-info-job.h"
#include "file-enumerator.h"
#include "global-settings.h"

#include <QGSettings>
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
#include <QPainter>
#include <QPainterPath>

#include <QProxyStyle>
#include <QStyleOptionToolButton>
#include <syslog.h>

using namespace Peony;

class LocationBarButtonStyle;
class IndicatorToolButton;

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
    void drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const override;
};

class IndicatorToolButton : public QToolButton
{
public:
    explicit IndicatorToolButton(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *) override;
};

LocationBar::LocationBar(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_Hover);
    setMouseTracking(true);

    //comment to fix button text show incomplete issue, link to bug#72080
//    setStyleSheet("padding-right: 15;"
//                  "margin-left: 2");
    m_styled_edit = new QLineEdit;
    qDebug()<<sizePolicy();
    //connect(this, &LocationBar::groupChangedRequest, this, &LocationBar::setRootUri);

    m_layout = new QHBoxLayout;
    setLayout(m_layout);

    m_indicator = new IndicatorToolButton(this);
    m_indicator->setObjectName("peony_location_bar_indicator");
    m_indicator->setFocusPolicy(Qt::FocusPolicy(m_indicator->focusPolicy() & ~Qt::TabFocus));
    m_indicator->setAutoRaise(true);
    //m_indicator->setStyle(LocationBarButtonStyle::getStyle());
    m_indicator->setPopupMode(QToolButton::InstantPopup);
    m_indicator->setArrowType(Qt::RightArrow);
    m_indicator->setCheckable(true);
    m_indicator->setFixedSize(this->height() - 2, this->height() - 2);
    m_indicator->move(0, 1);

    m_indicator_menu = new QMenu(m_indicator);
    m_indicator->setMenu(m_indicator_menu);
    m_indicator->setArrowType(Qt::RightArrow);

    connect(m_indicator_menu, &QMenu::aboutToShow, this, [=](){
        m_indicator->setArrowType(Qt::DownArrow);
    });

    connect(m_indicator_menu, &QMenu::aboutToHide, this, [=](){
        m_indicator->setArrowType(Qt::RightArrow);
    });

    //fix bug 40503, button text not show completely issue when fontsize is very big
    if (QGSettings::isSchemaInstalled("org.ukui.style"))
    {
        //font monitor
        QGSettings *fontSetting = new QGSettings(FONT_SETTINGS, QByteArray(), this);
        connect(fontSetting, &QGSettings::changed, this, [=](const QString &key){
            if (key == "systemFontSize") {
                // note that updateButtons() will cost more time.
                // there is no need to query file infos again here.
                // use doLayout() is enough.
                // btw, Bug#76858 is directly caused by info querying
                // due to updateButtons().

                //updateButtons();
                doLayout();
            }
        });
    }
}

LocationBar::~LocationBar()
{
    m_styled_edit->deleteLater();
}

void LocationBar::setRootUri(const QString &uri)
{
    //when is the same uri and has buttons return
    if (m_current_uri == uri && m_buttons.count() >0)
        return;

    m_current_uri = uri;

    //clear buttons
    clearButtons();
    if (m_current_uri.startsWith("search://")) {
        //m_indicator->setArrowType(Qt::NoArrow);
        addButton(m_current_uri, false, false);
        return;
    }

    m_current_info = FileInfo::fromUri(uri);
    m_buttons_info.clear();
    auto tmpUri = uri;
    while (!tmpUri.isEmpty() && tmpUri != "") {
        m_buttons_info.prepend(FileInfo::fromUri(tmpUri));
        if(tmpUri.startsWith("kmre:///") && tmpUri != "kmre:///"){
            m_buttons_info.prepend(FileInfo::fromUri("kmre:///"));
        }
        tmpUri = FileUtils::getParentUri(tmpUri);
    }

    m_querying_buttons_info = m_buttons_info;

    for (auto info : m_buttons_info) {
        auto infoJob = new FileInfoJob(info);
        infoJob->setAutoDelete();
        connect(infoJob, &FileInfoJob::queryAsyncFinished, this, [=](){
            // enumerate buttons info directory
            auto enumerator = new FileEnumerator;
            enumerator->setEnumerateDirectory(info.get()->uri());
            //comment to fix kydroid path show abnormal issue
            //enumerator->setEnumerateWithInfoJob();

            connect(enumerator, &FileEnumerator::enumerateFinished, this, [=](bool successed){
                if (successed) {
                    auto infos = enumerator->getChildren();
                    m_infos_hash.insert(info.get()->uri(), infos);
                    m_querying_buttons_info.removeOne(info);
                    if (m_querying_buttons_info.isEmpty()) {
                        // add buttons
                        clearButtons();
                        for (auto info : m_buttons_info) {
                            addButton(info.get()->uri().toLocal8Bit(), true, true);
                        }
                        doLayout();
                    }
                }

                enumerator->deleteLater();
            });

            enumerator->enumerateAsync();
        });
        infoJob->queryAsync();
    }

    return;
}

void LocationBar::clearButtons()
{
    for (auto button : m_buttons) {
        button->hide();
        button->deleteLater();
    }

    m_buttons.clear();
}

void LocationBar::updateButtons()
{
    //clear buttons
    clearButtons();

    if (m_current_uri.startsWith("search://")) {
        //m_indicator->setArrowType(Qt::NoArrow);
        addButton(m_current_uri, false, false);
        return;
    }

    auto uri = m_current_uri;
    m_current_info = FileInfo::fromUri(uri);
    m_buttons_info.clear();
    while (!uri.isEmpty() && uri != "") {
        m_buttons_info.prepend(FileInfo::fromUri(uri));
        uri = FileUtils::getParentUri(uri);
    }

    m_querying_buttons_info = m_buttons_info;

    for (auto info : m_buttons_info) {
        auto infoJob = new FileInfoJob(info);
        infoJob->setAutoDelete();
        connect(infoJob, &FileInfoJob::queryAsyncFinished, this, [=](){
            // enumerate buttons info directory
            auto enumerator = new FileEnumerator;
            enumerator->setEnumerateDirectory(info.get()->uri());
            enumerator->setEnumerateWithInfoJob();

            connect(enumerator, &FileEnumerator::enumerateFinished, this, [=](bool successed){
                if (successed) {
                    auto infos = enumerator->getChildren();
                    m_infos_hash.insert(info.get()->uri(), infos);
                    m_querying_buttons_info.removeOne(info);
                    if (m_querying_buttons_info.isEmpty()) {
                        // add buttons
                        clearButtons();
                        for (auto info : m_buttons_info) {
                            addButton(info.get()->uri(), true, true);
                        }
                        doLayout();
                    }
                }

                enumerator->deleteLater();
            });

            enumerator->enumerateAsync();
        });
        infoJob->queryAsync();
    }
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

    auto displayName = FileUtils::getFileDisplayName(uri);
    button->setToolTip(displayName);
    m_buttons.insert(QUrl(uri).toEncoded(), button);
    if (m_current_uri.startsWith("search://")) {
        QString nameRegexp = SearchVFSUriParser::getSearchUriNameRegexp(m_current_uri);
        QString targetDirectory = SearchVFSUriParser::getSearchUriTargetDirectory(m_current_uri);
        button->setIcon(QIcon::fromTheme("edit-find-symbolic"));
        displayName = tr("Search \"%1\" in \"%2\"").arg(nameRegexp).arg(targetDirectory);
        button->setText(displayName);
        button->setFixedWidth(button->sizeHint().width());
        return;
    }

    if (setIcon) {
        QIcon icon = QIcon::fromTheme(Peony::FileUtils::getFileIconName(uri), QIcon::fromTheme("folder"));
        button->setIcon(icon);
    }

    //comment to fix button text show incomplete issue, link to bug#72080
    //button->setStyleSheet("QToolButton{padding-left: 13px; padding-right: 13px}");

    //fix bug#84324
    //    QUrl url = uri;
    QUrl url = FileUtils::urlEncode(uri);
    if (!url.fileName().isEmpty())
    {
        button->setText(displayName);
        m_current_uri = uri.left(uri.lastIndexOf("/")+1) + displayName;
    } else {
        if (uri == "file:///") {
//            auto text = FileUtils::getFileDisplayName("computer:///root.link");
//            if (text.isNull()) {
//                text = tr("File System");
//            }
            //fix bug#47597, show as root.link issue
            QString text = tr("File System");
            button->setText(text);
            //comment to fix button text show incomplete issue, link to bug#72080
            //button->setStyleSheet("QToolButton{padding-left: 15px; padding-right: 15px}");
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
        Q_EMIT this->groupChangedRequest(uri);
    });

    if (setMenu) {
        auto infos = m_infos_hash.value(uri);
        QStringList uris;
        for (auto info : infos) {
            if (info.get()->isDir() && !info.get()->displayName().startsWith("."))
                uris<<info.get()->uri();
        }
        if (uris.isEmpty())
            button->setPopupMode(QToolButton::InstantPopup);
        Peony::PathBarModel m;
        m.setStringList(uris);
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
                QString tmp = uri;
                QIcon icon;
                displayName = Peony::FileUtils::getFileDisplayName(uri);
                if (displayName.length() > ELIDE_TEXT_LENGTH)
                {
                    int  charWidth = fontMetrics().averageCharWidth();
                    displayName = fontMetrics().elidedText(displayName, Qt::ElideRight, ELIDE_TEXT_LENGTH * charWidth);
                }
                if(uri.startsWith("filesafe:///")){
                    icon = QIcon::fromTheme(Peony::FileUtils::getFileIconName(uri, false), QIcon::fromTheme("folder"));
                }else{
                    icon = QIcon::fromTheme(Peony::FileUtils::getFileIconName(uri), QIcon::fromTheme("folder"));
                }
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
        auto copy = menu.addAction(QIcon::fromTheme("edit-copy-symbolic"), tr("Copy Directory"));

        menu.addAction(QIcon::fromTheme("tab-new-symbolic"), tr("Open In New Tab"), [=](){
            windowIface->addNewTabs(QStringList()<<uri);
        });

        menu.addAction(QIcon::fromTheme("window-new-symbolic"), tr("Open In New Window"), [=](){
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
    fopt.rect.adjust(0, 0, 0, 0);
    fopt.palette.setColor(QPalette::Highlight, fopt.palette.button().color());
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
        offset += m_indicator->width() + 2;
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
    //add some space for switch to edit
    for (int i = 0; i < 10; i++) {
         m_indicator_menu->addSeparator();
    }
}

void LocationBarButtonStyle::polish(QWidget *widget)
{
    if (widget->objectName() == "peony_location_bar_indicator") {
        return;
    }
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
        if (widget && widget->objectName() == "peony_location_bar_indicator") {
            opt.features.setFlag(QStyleOptionToolButton::HasMenu, false);
            return QProxyStyle::drawComplexControl(control, &opt, painter);
        } else {
            opt.rect.adjust(1, 1, -1, -1);
        }
        return QProxyStyle::drawComplexControl(control, &opt, painter, widget);
    }
    return QProxyStyle::drawComplexControl(control, option, painter, widget);
}

void LocationBarButtonStyle::drawControl(QStyle::ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    return QProxyStyle::drawControl(element, option, painter, widget);
}

IndicatorToolButton::IndicatorToolButton(QWidget *parent) : QToolButton(parent)
{

}

void IndicatorToolButton::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    QStyleOptionToolButton opt;
    initStyleOption(&opt);
    LocationBarButtonStyle::getStyle()->drawComplexControl(QStyle::CC_ToolButton, &opt, &p, this);
}
