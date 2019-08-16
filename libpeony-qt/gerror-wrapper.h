#ifndef GIOERROR_H
#define GIOERROR_H

#include "peony-core_global.h"
#include <gio/gio.h>
#include <memory>

#include <QMetaType>

namespace Peony {

class PEONYCORESHARED_EXPORT GErrorWrapper
{
public:
    GErrorWrapper();//do not use this constructor.
    GErrorWrapper(GError *err);
    ~GErrorWrapper();
    int code();
    QString message();
    QString domain();

    static std::shared_ptr<GErrorWrapper> wrapFrom(GError *err);

private:
    GError *m_err = nullptr;
};

typedef std::shared_ptr<GErrorWrapper> GErrorWrapperPtr;

}

Q_DECLARE_METATYPE(Peony::GErrorWrapper)
Q_DECLARE_METATYPE(Peony::GErrorWrapperPtr)

#endif // GIOERROR_H
