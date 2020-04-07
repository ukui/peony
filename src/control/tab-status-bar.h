#ifndef TABSTATUSBAR_H
#define TABSTATUSBAR_H

#include <QStatusBar>

class QLabel;
class QToolBar;
class TabWidget;

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
    QLabel *m_label = nullptr;
    QToolBar *m_styled_toolbar = nullptr;
};

#endif // TABSTATUSBAR_H
