#ifndef PREVIEWOPTIONTOOLBAR_H
#define PREVIEWOPTIONTOOLBAR_H

#include <QToolBar>
#include "peony-core_global.h"

namespace Peony {

class PEONYCORESHARED_EXPORT PreviewOptionToolBar : public QToolBar
{
    Q_OBJECT
public:
    explicit PreviewOptionToolBar(QWidget *parent = nullptr);

};

}

#endif // PREVIEWOPTIONTOOLBAR_H
