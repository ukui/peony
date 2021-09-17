//
// Created by hxf on 2021/8/31.
//

#ifndef PEONY_STUDY_CENTER_FACTORY_H
#define PEONY_STUDY_CENTER_FACTORY_H

#include "desktop-factory-i-face.h"

namespace Peony {

class StudyCenterFactory : public QObject, public DesktopFactoryIFace
{
public:
    static StudyCenterFactory *getInstance(QObject *parent = nullptr);

    ~StudyCenterFactory() override;

    DesktopType getDesktopType() override;

    QString getFactoryName() override;

    bool isEnable() override;

    DesktopWidgetBase *createDesktop(QWidget *parent) override;

    DesktopWidgetBase *createNewDesktop(QWidget *parent) override;

    bool closeFactory() override;

private:
    explicit StudyCenterFactory(QObject *parent = nullptr);
};

}

#endif //PEONY_STUDY_CENTER_FACTORY_H
