#ifndef OPERATIONMENU_H
#define OPERATIONMENU_H

#include <QMenu>

class MainWindow;

class OperationMenu : public QMenu
{
    Q_OBJECT
public:
    explicit OperationMenu(MainWindow *window, QWidget *parent = nullptr);

public Q_SLOTS:
    void updateMenu();

private:
    MainWindow *m_window;
};

#endif // OPERATIONMENU_H
