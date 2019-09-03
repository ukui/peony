#ifndef DEFAULTPREVIEWPAGE_H
#define DEFAULTPREVIEWPAGE_H

#include "peony-core_global.h"
#include <QWidget>

namespace Peony {

class PEONYCORESHARED_EXPORT DefaultPreviewPage : public QWidget
{
    Q_OBJECT
public:
    explicit DefaultPreviewPage(const QString &uri, QWidget *parent = nullptr);
};

}

#endif // DEFAULTPREVIEWPAGE_H
