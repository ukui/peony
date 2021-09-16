//
// Created by hxf on 2021/8/6.
//

#ifndef DESKTOP_MODE_FACTORY_H
#define DESKTOP_MODE_FACTORY_H

#include "desktop-factory-i-face.h"

namespace Peony {

class DesktopModeFactory : public QObject, public DesktopFactoryIFace
{
Q_OBJECT
public:
    static DesktopModeFactory *getInstance(QObject *parent = nullptr);

    ~DesktopModeFactory() override;

    DesktopType getDesktopType() override;

    QString getFactoryName() override;

    bool isEnable() override;

    DesktopWidgetBase *createDesktop(QWidget *parent = nullptr) override;

    DesktopWidgetBase *createNewDesktop(QWidget *parent) override;

    bool closeFactory() override;

private:
    explicit DesktopModeFactory(QObject *parent = nullptr);

};

}

#endif //DESKTOP_MODE_FACTORY_H
