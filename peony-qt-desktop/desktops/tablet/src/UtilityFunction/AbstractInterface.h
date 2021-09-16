#ifndef ABSTRACTINTERFACE_H
#define ABSTRACTINTERFACE_H
#include <QObject>
class QWidget;
class AbstractInterface
{
public:
    virtual ~AbstractInterface() {}
    virtual QWidget *createPluginWidget(QWidget *parent,bool type) = 0;
};
#define AbstractInterface_iid "Welcome to use focusmode"
Q_DECLARE_INTERFACE(AbstractInterface,AbstractInterface_iid)
#endif // ABSTRACTINTERFACE_H
