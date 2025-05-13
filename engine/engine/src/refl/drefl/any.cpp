#include "nickel/refl/drefl/any.hpp"

namespace nickel::refl {

Any::Any(Any&& o)
    : m_access(o.m_access), m_payload(o.m_payload), m_operations(o.m_operations), m_type(o.m_type) {
    o.m_access = AccessType::Null;
    o.m_payload = nullptr;
}

Any::Any(const Any& o) {
    switch (o.m_access) {
        case AccessType::Null:
            m_access = AccessType::Null;
            m_payload = nullptr;
            m_operations = &TypeOperations::null;
            break;
        case AccessType::ConstRef:
        case AccessType::Ref:
            m_payload = o.m_payload;
            m_access = o.m_access;
            m_operations = o.m_operations;
            break;
        case AccessType::Copy:
            m_payload = o.m_operations->copy_construct(o.m_payload);
            m_access = o.m_access;
            m_operations = o.m_operations;
            break;
    }
    m_type = o.m_type;
}

Any& Any::operator=(const Any& o) {
    if (m_access == AccessType::Copy) {
        m_operations->destroy(m_payload);
        m_payload = nullptr;
    }

    m_type = o.m_type;

    switch (o.m_access) {
        case AccessType::Null:
            m_access = AccessType::Null;
            m_payload = nullptr;
            m_operations = &TypeOperations::null;
            break;
        case AccessType::ConstRef:
        case AccessType::Ref:
            m_access = o.m_access;
            m_payload = o.m_payload;
            m_operations = o.m_operations;
            break;
        case AccessType::Copy:
            m_access = o.m_access;
            m_payload = o.m_operations->copy_construct(o.m_payload);
            m_operations = o.m_operations;
            break;
    }

    return *this;
}

Any& Any::operator=(Any&& o) {
    if (&o != this) {
        m_payload = o.m_payload;
        m_access = o.m_access;
        m_operations = o.m_operations;
        m_type = o.m_type;

        o.m_payload = nullptr;
        o.m_access = AccessType::Null;
        o.m_operations = &TypeOperations::null;
        o.m_type = nullptr;
    }
    return *this;
}

Any::~Any() {
    if (m_access == AccessType::Copy && m_operations) {
        m_operations->destroy(m_payload);
    }
}

}  // namespace nickel::refl