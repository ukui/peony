#ifndef ICONVIEWEDITOR_H
#define ICONVIEWEDITOR_H

#include <QTextEdit>

#include "peony-core_global.h"

class QLabel;

namespace Peony {

namespace DirectoryView {

class PEONYCORESHARED_EXPORT IconViewEditor : public QTextEdit
{
    Q_OBJECT
public:
    explicit IconViewEditor(QWidget *parent = nullptr);

public Q_SLOTS:
    void minimalAdjust();

protected:
    void paintEvent(QPaintEvent *e) override;
};

}

}

#endif // ICONVIEWEDITOR_H
