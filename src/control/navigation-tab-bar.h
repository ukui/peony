#ifndef NAVIGATIONTABBAR_H
#define NAVIGATIONTABBAR_H

#include <QTabBar>

class QToolButton;

class NavigationTabBar : public QTabBar
{
    Q_OBJECT
public:
    explicit NavigationTabBar(QWidget *parent = nullptr);

Q_SIGNALS:
    void pageAdded(const QString &uri);
    void closeWindowRequest();

public Q_SLOTS:
    void addPage(const QString &uri = nullptr, bool jumpToNewTab = false);
    void addPages(const QStringList &uri);

protected:
    void tabRemoved(int index) override;
    void tabInserted(int index) override;
    void relayoutFloatButton(bool insterted);

private:
    QToolButton *m_float_button;
};

#endif // NAVIGATIONTABBAR_H
