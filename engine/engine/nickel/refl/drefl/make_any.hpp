#pragma once

#include "nickel/refl/drefl/factory.hpp"
#include "nickel/refl/drefl/any.hpp"

namespace nickel::refl {

template <typename T>
Any AnyMakeConstRef(const T& value) noexcept {
    using type = remove_cvref_t<T>;
    return {Any::AccessType::ConstRef, (void*)&value,
            &TypeOperationTraits<type>::GetOperations(),
            TypeInfo<remove_cvref_t<T>>()};
}

template <typename T>
Any AnyMakeRef(T& value) noexcept {
    using type = remove_cvref_t<T>;
    return {Any::AccessType::Ref, (void*)&value,
            &TypeOperationTraits<type>::GetOperations(),
            TypeInfo<remove_cvref_t<T>>()};
}

template <typename T>
Any AnyMakeCopy(T&& value) noexcept(
    std::is_rvalue_reference_v<T&&>
        ? std::is_nothrow_move_constructible_v<remove_cvref_t<T>>
        : std::is_nothrow_copy_constructible_v<remove_cvref_t<T>>) {
    using type = remove_cvref_t<T>;

    void* elem = nullptr;
    try {
        if constexpr (std::is_enum_v<T>) {
            elem = new long{static_cast<long>(value)};
        } else {
            elem = new type{std::forward<T>(value)};
        }
    } catch (const std::bad_alloc&) {
        LOGE("make copy failed! due to allocate memory failed!");
        elem = nullptr;
    } catch (...) {
        throw;
    }
    return {Any::AccessType::Copy, elem, &TypeOperationTraits<type>::GetOperations(),
            TypeInfo<remove_cvref_t<T>>()};
}

}  // namespace nickel::refl