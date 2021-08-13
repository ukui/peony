//
// Created by hxf on 2021/8/10.
//

#ifndef PEONY_TABLET_MODE_FACTORY_H
#define PEONY_TABLET_MODE_FACTORY_H

#include "desktop-factory-i-face.h"

namespace Peony {

class TabletModeFactory : public QObject, public DesktopFactoryIFace
{
    Q_OBJECT
public:
    static TabletModeFactory *getInstance(QObject *parent = nullptr);

    ~TabletModeFactory() override;

    DesktopType getDesktopType() override;

    QString getFactoryName() override;

    bool isEnable() override;

    DesktopWidgetBase *createDesktop(QWidget *parent) override;

    DesktopWidgetBase *createNewDesktop(QWidget *parent) override;

    bool closeFactory() override;

private:
    explicit TabletModeFactory(QObject *parent = nullptr);
};

}

#endif //PEONY_TABLET_MODE_FACTORY_H
