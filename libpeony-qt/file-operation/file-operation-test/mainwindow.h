#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "file-operation.h"
#include "gerror-wrapper.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

public Q_SLOTS:
    QVariant handleError(const QString &srcUri,
                         const QString &destDirUri,
                         const Peony::GErrorWrapperPtr &err);
};

#endif // MAINWINDOW_H
