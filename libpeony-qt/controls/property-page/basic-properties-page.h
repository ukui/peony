#ifndef BASICPROPERTIESPAGE_H
#define BASICPROPERTIESPAGE_H

#include <QWidget>
#include "peony-core_global.h"

class QVBoxLayout;

namespace Peony {

class BasicPropertiesPage : public QWidget
{
    Q_OBJECT
public:
    explicit BasicPropertiesPage(const QString &uri, QWidget *parent = nullptr);

protected:
    void addSeparator();

private:
    QVBoxLayout *m_layout = nullptr;
};

}

#endif // BASICPROPERTIESPAGE_H
