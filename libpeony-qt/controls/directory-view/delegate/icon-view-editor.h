#ifndef ICONVIEWEDITOR_H
#define ICONVIEWEDITOR_H

#include <QTextEdit>

#include "peony-core_global.h"

class QLabel;
class QLineEdit;

namespace Peony {

namespace DirectoryView {

class PEONYCORESHARED_EXPORT IconViewEditor : public QTextEdit
{
    Q_OBJECT
public:
    explicit IconViewEditor(QWidget *parent = nullptr);
    ~IconViewEditor() override;

Q_SIGNALS:
    void returnPressed();

public Q_SLOTS:
    void minimalAdjust();

protected:
    void paintEvent(QPaintEvent *e) override;
    void keyPressEvent(QKeyEvent *e) override;

private:
    QLineEdit *m_styled_edit;
};

}

}

#endif // ICONVIEWEDITOR_H
