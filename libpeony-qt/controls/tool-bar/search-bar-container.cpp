/*
 * Peony-Qt
 *
 * Copyright (C) 2020, Tianjin KYLIN Information Technology Co., Ltd.
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
 * Authors: MeihongHe <hemeihong@kylinos.cn>
 *
 */

#include <QStringListModel>

#include "search-bar-container.h"

#include <QAction>
#include <QCompleter>
#include <QVector4D>

#include <QDebug>
#include <QToolButton>
#include <QPainter>
#include <QPainterPath>

using namespace Peony;
static ToolButtonStyle *global_instance = nullptr;
SearchBarContainer::SearchBarContainer(QWidget *parent): QWidget(parent)
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    this->setLayout(layout);
    m_layout = layout;
    layout->setContentsMargins(0,0,0,0);

//    QComboBox *filter = new QComboBox(this);
//    m_filter_box = filter;
//    filter->setToolTip(tr("Choose File Type"));
    auto model = new QStringListModel(this);
    model->setStringList(m_file_type_list);
//    filter->setModel(model);
//    filter->setFixedWidth(80);
//    filter->setFixedHeight(parent->height());

    QLineEdit *edit = new QLineEdit(this);
    m_search_box = edit;
    edit->setFixedHeight(parent->height());

//    layout->addWidget(filter, Qt::AlignLeft);
    layout->addWidget(edit, Qt::AlignLeft);

    //search history
    m_model = new QStringListModel(m_search_box);
    QCompleter *completer = new QCompleter(m_search_box);
    completer->setModel(m_model);
    completer->setMaxVisibleItems(10);

    auto m_list = m_model->stringList();
    m_list.prepend(tr("Clear"));
    m_model->setStringList(m_list);
    m_list_view = new QListView(m_search_box);

    m_list_view->setAttribute(Qt::WA_TranslucentBackground);
    //m_list_view->viewport()->setAttribute(Qt::WA_TranslucentBackground);

    m_list_view->setProperty("useCustomShadow", true);
    m_list_view->setProperty("customShadowDarkness", 0.5);
    m_list_view->setProperty("customShadowWidth", 20);
    m_list_view->setProperty("customShadowRadius", QVector4D(6, 6, 6, 6));
    m_list_view->setProperty("customShadowMargins", QVector4D(20, 20, 20, 20));

    m_list_view->setModel(m_model);
    completer->setPopup(m_list_view);

    //change QCompleter Mode from PopupCompletion to InlineCompletion，
    //show list in pop up window way will effect the input method
    //to fix can not input chinese continuous issue,link to bug#90621
    completer->setCompletionMode(QCompleter::InlineCompletion);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    m_search_box->setCompleter(completer);

    m_search_trigger.setInterval(500);
    m_clear_action = true;
    connect(&m_search_trigger, SIGNAL(timeout()), this, SLOT(startSearch()));
    connect(m_search_box, &QLineEdit::textChanged, [=]()
    {
        //fix input key words can not search issue, link to bug#77977
        if (m_clear_action && ! m_search_trigger.isActive())
            m_search_trigger.start();
        else
            m_clear_action = false;
    });
//    connect(m_filter_box, &QComboBox::currentTextChanged, [=]()
//    {
//        Q_EMIT this->filterUpdate(m_filter_box->currentIndex());
//    });

    //bug#93521 搜索界面中 添加搜索图标
    QHBoxLayout* editlayout = new QHBoxLayout(edit);
    editlayout->addStretch();

    QToolButton *searchButton = new QToolButton(edit);
    searchButton->setObjectName("toolButton");
    searchButton->setStyle(ToolButtonStyle::getStyle());
    searchButton->setIcon(QIcon::fromTheme("ukui-down-symbolic", QIcon(":/icons/ukui-down-symbolic")));
    searchButton->setProperty("isWindowButton", 1);
    searchButton->setProperty("useIconHighlightEffect", 0x2);
    searchButton->setAutoRaise(true);
    searchButton->setFixedSize(edit->height() - 2, edit->height() - 2);
    editlayout->addWidget(searchButton,Qt::AlignRight);
    connect(searchButton, &QToolButton::clicked, this, [=]() {
        //qDebug() << "triggered search history!";
        m_search_box->completer()->complete();
    });

    QToolButton* clearButton = new QToolButton(edit);
    clearButton->setObjectName("toolButton");
    clearButton->setStyle(ToolButtonStyle::getStyle());
    editlayout->addWidget(clearButton,Qt::AlignRight);
    clearButton->setFixedSize(edit->height() - 2, edit->height() - 2);
    clearButton->setAutoRaise(true);
//    QToolButton* goToButton = new QToolButton(edit);
//    goToButton->setAttribute(Qt::WA_TranslucentBackground);
//    goToButton->setObjectName("toolButton");
//    goToButton->setStyle(ToolButtonStyle::getStyle());
//    goToButton->setFixedSize(edit->height() - 2, edit->height() - 2);
//    QPalette palette = goToButton->palette();
//    palette.setColor(QPalette::Active, QPalette::Button, QColor(Qt::blue));
//    goToButton->setPalette(palette);
//    editlayout->addWidget(goToButton,Qt::AlignRight);

    editlayout->setMargin(2);
    edit->setLayout(editlayout);

    clearButton->setIcon(QIcon::fromTheme("edit-clear-symbolic"));
    clearButton->setProperty("isWindowButton", 1);
    clearButton->setProperty("useIconHighlightEffect", 2);
    clearButton->hide();

//    goToButton->setIcon(QIcon::fromTheme("go-next-symbolic"));
//    goToButton->setProperty("useIconHighlightEffect", true);
//    goToButton->setProperty("iconHighlightEffectMode", 1);

//    goToButton->hide();
//    connect(goToButton, &QToolButton::clicked, this, [=](){
//        //开始搜索
//        startSearch();
//    });

    connect(clearButton, &QToolButton::clicked, this, [=](){
        //停止搜索
        edit->clear();
    });
    connect(edit, &QLineEdit::textChanged, this,  [=](const QString &text){
        if(text.isEmpty())
        {
            //goToButton->hide();
            clearButton->hide();
            searchButton->show();
        }
        else
        {
            //goToButton->show();
            clearButton->show();
            searchButton->hide();
        }
    });

    connect(m_list_view, SIGNAL(clicked(const QModelIndex &)), this, SLOT(onTableClicked(const QModelIndex &)));
}

QSize SearchBarContainer::sizeHint() const
{
    return this->topLevelWidget()->sizeHint();
}

void SearchBarContainer::onTableClicked(const QModelIndex &index)
{
    //qDebug() << "onTableClicked:" <<index.data().toInt() <<m_model->rowCount();
    if (index.row() != m_model->rowCount()-1)
    {
        m_search_box->setText(index.data().toString());
        return;
    }

    m_search_box->setText("");
    auto l = m_model->stringList();
    l.clear();
    l.prepend(tr("Clear"));
    m_model->setStringList(l);
}

void SearchBarContainer::startSearch()
{
    auto l = m_model->stringList();
    //fix has empty key words issue
    if (m_search_box->text() != nullptr && m_search_box->text().length() >0
         && ! l.contains(m_search_box->text()))
        l.prepend(m_search_box->text());

    //qDebug() << "SearchBarContainer::startSearch:" <<m_search_box->text();
    m_model->setStringList(l);
    Q_EMIT this->returnPressed();
}

void SearchBarContainer::clearSearchBox()
{
    m_search_box->setText("");
    m_clear_action = true;
    //need stop search action
    m_search_trigger.stop();
}

ToolButtonStyle *ToolButtonStyle::getStyle()
{
    if (!global_instance) {
        global_instance = new ToolButtonStyle;
    }
    return global_instance;
}

void ToolButtonStyle::drawComplexControl(QStyle::ComplexControl control, const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget) const
{
    if (widget &&  widget->objectName() == "toolButton") {
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing);
        QPainterPath path;
        path.addEllipse(widget->rect().adjusted(2, 2, -2, -2));
        painter->setClipPath(path);
        QProxyStyle::drawComplexControl(control, option, painter, widget);
        painter->restore();
    } else {
        QProxyStyle::drawComplexControl(control, option, painter, widget);
    }
}
