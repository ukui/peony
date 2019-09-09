#ifndef ICONVIEWEDITOR_H
#define ICONVIEWEDITOR_H

#include <QTextEdit>

class IconViewEditor : public QTextEdit
{
    Q_OBJECT
public:
    explicit IconViewEditor(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *e) override;
};

#endif // ICONVIEWEDITOR_H
