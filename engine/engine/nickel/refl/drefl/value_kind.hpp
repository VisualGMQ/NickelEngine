#pragma once

#include <string_view>
#include <string>
#include <type_traits>
#include "nickel/refl/util/misc.hpp"

namespace nickel::refl {

enum class ValueKind {
    None,
    Boolean,
    Numeric,
    String,
    Enum,
    Class,
    Property,
    Pointer,
    Array,
    Optional,
};

template <typename T>
ValueKind GetKindFromType() {
    if constexpr (is_std_array_v<T> || is_vector_v<T> ||
                  std::is_array_v<T>) {
        return ValueKind::Array;
    }
    if constexpr (is_optional_v<T>) {
        return ValueKind::Optional;
    }
    if constexpr (std::is_pointer_v<T>) {
        return ValueKind::Pointer;
    }
    if constexpr (std::is_fundamental_v<T>) {
        return ValueKind::Numeric;
    }
    if constexpr (std::is_same_v<T, std::string> ||
                  std::is_same_v<T, std::string_view>) {
        return ValueKind::String;
    }
    if constexpr (std::is_enum_v<T>) {
        return ValueKind::Enum;
    }
    if constexpr (std::is_class_v<T>) {
        return ValueKind::Class;
    }

    return ValueKind::None;
}

}  // namespace nickel::refl