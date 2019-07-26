#include "gerror-wrapper.h"
#include <QString>

using namespace Peony;

GErrorWrapper::GErrorWrapper(GError *err)
{
    m_err = err;
}

GErrorWrapper::~GErrorWrapper()
{
    if (m_err)
        g_error_free(m_err);
}

int GErrorWrapper::code()
{
    if (!m_err)
        return -1;
    return m_err->code;
}

QString GErrorWrapper::message()
{
    if (!m_err)
        return nullptr;
    return m_err->message;
}

QString GErrorWrapper::domain()
{
    if (!m_err)
        return nullptr;
    return g_quark_to_string(m_err->domain);
}

std::shared_ptr<GErrorWrapper> GErrorWrapper::wrapFrom(GError *err)
{
    return std::make_shared<GErrorWrapper>(err);
}
