#include "basic-properties-page.h"

#include <QVBoxLayout>
#include <QFrame>
#include <QLabel>

using namespace Peony;

BasicPropertiesPage::BasicPropertiesPage(const QString &uri, QWidget *parent) : QWidget(parent)
{
    //FIXME: complete the content
    QVBoxLayout *layout = new QVBoxLayout(this);
    m_layout = layout;
    layout->setMargin(0);
    layout->setSpacing(0);

    setLayout(layout);

    //use hline shape QFrame?
    QLabel *label = new QLabel("FIXME:\n"
                               "display name:\n"
                               "location\n"
                               "uri"+uri, this);
    layout->addWidget(label);
    addSeparator();

    QLabel *l2 = new QLabel("FIXME:\n"
                            "type\n"
                            "open\n"
                            "size\n", this);
    layout->addWidget(l2);
    addSeparator();

    QLabel *l3 = new QLabel("FIXME:\n"
                            "created\n"
                            "modified\n"
                            "accessed", this);
    layout->addWidget(l3);
    addSeparator();

    QLabel *l4 = new QLabel("FIXME:\n"
                            "read write", this);
    layout->addWidget(l4);
}

void BasicPropertiesPage::addSeparator()
{
    QFrame *separator = new QFrame(this);
    separator->setFrameShape(QFrame::HLine);
    m_layout->addWidget(separator);
}
