#include "mainwindow.h"

#include <QLineEdit>
#include <QLayout>
#include <memory>

#include "file-info.h"
#include "file-item.h"
#include "file-item-model.h"
#include <QTreeView>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QIcon::setThemeName("ukui-icon-theme");
    QLineEdit *line = new QLineEdit("computer:///", this);
    layout()->addWidget(line);
    connect(line, &QLineEdit::returnPressed, [=](){
        Peony::FileItemModel *model = new Peony::FileItemModel(this);
        Peony::FileItem *item = new Peony::FileItem(Peony::FileInfo::fromUri(line->text()), nullptr, model);
        model->setRootItem(item);
        QTreeView *v = new QTreeView();
        v->setModel(model);
        v->show();
    });
}

MainWindow::~MainWindow()
{

}
