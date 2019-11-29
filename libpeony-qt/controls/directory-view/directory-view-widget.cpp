#include "directory-view-widget.h"

#include <QApplication>

using namespace Peony;

DirectoryViewWidget::DirectoryViewWidget(QWidget *parent) : QWidget (parent)
{
    setAttribute(Qt::WA_DeleteOnClose);

    connect(qApp, &QApplication::paletteChanged, this, [=](){
        this->repaintView();
    });
}
