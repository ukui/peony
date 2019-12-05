#include "advanced-location-bar.h"
#include "path-edit.h"
#include "location-bar.h"

#include <QStackedLayout>

using namespace Peony;

AdvancedLocationBar::AdvancedLocationBar(QWidget *parent) : QWidget(parent)
{
    QStackedLayout *layout = new QStackedLayout(this);
    m_layout = layout;

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->setSizeConstraint(QLayout::SetDefaultConstraint);
    m_bar = new Peony::LocationBar(this);
    m_edit = new Peony::PathEdit(this);
    //bar->connect(bar, &Peony::LocationBar::groupChangedRequest, bar, &Peony::LocationBar::setRootUri);
    m_bar->connect(m_bar, &Peony::LocationBar::blankClicked, [=](){
        layout->setCurrentWidget(m_edit);
        m_edit->setFocus();
        m_edit->setUri(m_bar->getCurentUri());
    });

    m_edit->connect(m_edit, &Peony::PathEdit::uriChangeRequest, [=](const QString uri){
        if (m_text == uri) {
            Q_EMIT this->refreshRequest();
            return;
        }
        m_bar->setRootUri(uri);
        layout->setCurrentWidget(m_bar);
        Q_EMIT this->updateWindowLocationRequest(uri);
        m_text = m_edit->text();
    });

    m_bar->connect(m_bar, &LocationBar::groupChangedRequest, [=](const QString &uri){
        if (m_text == uri) {
            Q_EMIT this->refreshRequest();
            return;
        }
        Q_EMIT this->updateWindowLocationRequest(uri);
        m_text = uri;
    });

    m_edit->connect(m_edit, &Peony::PathEdit::editCancelled, [=](){
        layout->setCurrentWidget(m_bar);
    });

    layout->addWidget(m_bar);
    layout->addWidget(m_edit);

    setLayout(layout);
    setFixedHeight(m_edit->height());
}

void AdvancedLocationBar::updateLocation(const QString &uri)
{
    m_bar->setRootUri(uri);
    m_edit->setUri(uri);
    m_text = uri;
}

bool AdvancedLocationBar::isEditing()
{
    return m_edit->isVisible();
}

void AdvancedLocationBar::startEdit()
{
    m_layout->setCurrentWidget(m_edit);
    m_edit->setFocus();
    m_edit->setUri(m_bar->getCurentUri());
}

void AdvancedLocationBar::finishEdit()
{
    Q_EMIT m_edit->returnPressed();
}
