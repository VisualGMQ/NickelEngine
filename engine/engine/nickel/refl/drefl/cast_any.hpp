#pragma once

#include "nickel/refl/drefl/factory.hpp"
#include "nickel/refl/drefl/any.hpp"

namespace nickel::refl {

template <typename T>
T* TryCast(Any& a) {
    if (TypeInfo<T>() == a.m_type) {
        if (a.m_access == Any::AccessType::Ref || a.m_access == Any::AccessType::Copy) {
            return static_cast<T*>(a.m_payload);
        }
    }
    throw BadAnyAccess{"can't cast mutable type from const reference"};
}

template <typename T>
const T* TryCastConst(const Any& a) {
    if (TypeInfo<T>() == a.m_type) {
        return static_cast<T*>(a.m_payload);
    }
    return nullptr;
}


}