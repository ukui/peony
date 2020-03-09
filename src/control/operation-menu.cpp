#include "operation-menu.h"

OperationMenu::OperationMenu(MainWindow *window, QWidget *parent) : QMenu(parent)
{
    m_window = window;

    connect(this, &QMenu::aboutToShow, this, &OperationMenu::updateMenu);
}

void OperationMenu::updateMenu()
{

}
