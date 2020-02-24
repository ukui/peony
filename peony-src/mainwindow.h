#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    bool eventFilter(QObject *obj, QEvent *e);

protected:
    void paintEvent(QPaintEvent *e);

private:
    Ui::MainWindow *ui;

    QPoint m_last_move_mouse_released_pos;
};

#endif // MAINWINDOW_H
