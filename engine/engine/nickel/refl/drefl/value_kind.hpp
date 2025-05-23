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

}  // namespace nickel::refl