#ifndef TABSTATUSBAR_H
#define TABSTATUSBAR_H

#include <QStatusBar>

class QLabel;
class QToolBar;
class TabWidget;
class ElidedLabel;

class TabStatusBar : public QStatusBar
{
    Q_OBJECT
public:
    explicit TabStatusBar(TabWidget *tab, QWidget *parent = nullptr);
    ~TabStatusBar() override;

public Q_SLOTS:
    void update();
    void update(const QString &message);

protected:
    void paintEvent(QPaintEvent *e) override;

private:
    TabWidget *m_tab = nullptr;
    ElidedLabel *m_label = nullptr;
    QToolBar *m_styled_toolbar = nullptr;
};

class ElidedLabel : public QWidget
{
    Q_OBJECT
public:
    explicit ElidedLabel(QWidget *parent);

    void setText(const QString &text);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QString m_text;
};

#endif // TABSTATUSBAR_H
