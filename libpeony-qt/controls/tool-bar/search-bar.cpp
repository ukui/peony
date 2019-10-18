#include "search-bar.h"
#include <QStringListModel>
#include <QCompleter>
#include <QAction>
#include <QIcon>

#include <QDebug>

using namespace Peony;

SearchBar::SearchBar(QWidget *parent) : QLineEdit(parent)
{
    setTextMargins(5, 0, 0, 0);
    setFixedWidth(175);

    setToolTip(tr("Input the search key of files you would like to find."));

    m_model = new QStringListModel(this);
    QCompleter *completer = new QCompleter(this);
    completer->setModel(m_model);
    completer->setMaxVisibleItems(10);
    //completer->setCompletionPrefix(" ");
    completer->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
    this->setCompleter(completer);

    setLayoutDirection(Qt::LeftToRight);
    setPlaceholderText(tr("Input search key..."));
    QAction *searchAction = addAction(QIcon::fromTheme("edit-find-symbolic"), QLineEdit::TrailingPosition);
    //NOTE: we should not add a short cut for line edit,
    //because it might have some bad effect for other controls.
    //use returnPressed signal trigger the action instead.
    //searchAction->setShortcut(Qt::Key_Return);
    connect(this, &QLineEdit::returnPressed, searchAction, &QAction::trigger);

    connect(this, &QLineEdit::textChanged, this, &SearchBar::searchKeyChanged);
    connect(searchAction, &QAction::triggered, [=](){
        if (!this->text().isEmpty()) {
            auto l = m_model->stringList();
            if (!l.contains(this->text()))
                l.prepend(this->text());

            m_model->setStringList(l);
            qDebug()<<m_model->stringList();
            Q_EMIT this->searchRequest(this->text());
            this->clear();
            this->clearFocus();
        }
    });
}

void SearchBar::focusInEvent(QFocusEvent *e)
{
    blockSignals(false);
    QLineEdit::focusInEvent(e);
    this->completer()->complete();
}

void SearchBar::focusOutEvent(QFocusEvent *e)
{
    blockSignals(true);
    QLineEdit::focusOutEvent(e);
    //this->clear();
}
