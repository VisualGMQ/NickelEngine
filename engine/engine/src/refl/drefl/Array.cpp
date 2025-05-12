#include "nickel/refl/drefl/array.hpp"
#include "nickel/refl/drefl/any.hpp"
#include "nickel/refl/drefl/operation_traits.hpp"

namespace nickel::refl {

Any Array::Get(size_t idx, Any& array) const noexcept {
    enum Any::AccessType access = Any::AccessType::Ref;
    if (array.IsConstRef()) {
        access = Any::AccessType::ConstRef;
    }
    return {access, m_operations->get(idx, array.Payload(), false),
            m_elem_operations, m_elem_type};
}

Any Array::GetConst(size_t idx, const Any& array) const noexcept {
    enum Any::AccessType access = Any::AccessType::Ref;
    return {Any::AccessType::ConstRef,
            m_operations->get(idx, (void*)array.Payload(), true),
            m_elem_operations, m_elem_type};
}

bool Array::PushBack(const Any& elem, Any& array) const noexcept {
    if (elem.TypeInfo() != m_elem_type) {
        return false;
    }

    return m_operations->push_back(elem.Payload(), array.Payload());
}

bool Array::PopBack(Any& array) const noexcept {
    if (array.IsConstRef()) {
        return false;
    } else {
        return m_operations->pop_back(array.Payload());
    }
}

Any Array::Back(Any& array) const noexcept {
    void* elem = m_operations->back(array.Payload(), false);

    if (!elem) {
        return {Any::AccessType::Null, nullptr, nullptr, array.TypeInfo()};
    } else {
        auto access = array.AccessType() == Any::AccessType::ConstRef
                          ? Any::AccessType::ConstRef
                          : Any::AccessType::Ref;
        return {access, elem, m_elem_operations, m_elem_type};
    }
}

Any Array::BackConst(const Any& array) const noexcept {
    void* elem = m_operations->back((void*)array.Payload(), true);

    if (!elem) {
        return {Any::AccessType::Null, nullptr, nullptr, array.TypeInfo()};
    } else {
        return {Any::AccessType::ConstRef, elem, m_elem_operations, m_elem_type};
    }
}

bool Array::Resize(size_t size, Any& array) const noexcept {
    if (array.IsConstRef()) {
        return false;
    }
    return m_operations->resize(size, array.Payload());
}

size_t Array::Size(const Any& array) const noexcept {
    return m_operations->size(array.Payload());
}

size_t Array::Capacity(const Any& array) const noexcept {
    return m_operations->capacity(array.Payload());
}

bool Array::Insert(size_t idx, const Any& elem, Any& array) const noexcept {
    if (array.IsConstRef() || elem.TypeInfo() != m_elem_type) {
        return false;
    }

    return m_operations->insert(idx, elem.Payload(), array.Payload());
}

}  // namespace nickel::refl