#include "status-bar.h"
#include "fm-window.h"

#include <QLabel>
#include <QPainter>
#include <QApplication>
#include <QStyle>

using namespace Peony;

StatusBar::StatusBar(FMWindow *window, QWidget *parent) : QStatusBar(parent)
{
    setContentsMargins(0, 0, 0, 0);
    setStyleSheet("padding: 0;");
    setSizeGripEnabled(false);
    setMinimumHeight(30);

    m_window = window;
    m_label = new QLabel(this);
    m_label->setContentsMargins(0, 0, 0, 0);
    m_label->setWordWrap(false);
    m_label->setAlignment(Qt::AlignCenter);
    addWidget(m_label, 1);

    //setStyleSheet("align: center;");
    //showMessage(tr("Status Bar"));
}

void StatusBar::update()
{
    if (!m_window)
        return;

    auto selections = m_window->getCurrentSelections();
    if (!selections.isEmpty()) {
        m_label->setText(tr("%1 files selected ").arg(selections.count()));
        //showMessage(tr("%1 files selected ").arg(selections.count()));
    } else {
        m_label->setText(m_window->getCurrentUri());
        //showMessage(m_window->getCurrentUri());
    }
}

void StatusBar::paintEvent(QPaintEvent *e)
{
    //I do not want status bar draw the inserted widget's
    //'border', so I use painter overwrite it.
    QStatusBar::paintEvent(e);
    QPainter p(this);
    QPalette palette;
    auto rect = this->rect();
    rect.adjust(1, 2, 1, 1);
    p.fillRect(rect, palette.brush(QPalette::Background));
}
