#include "nickel/ecs/id.hpp"

namespace nickel::ecs {

using id_underlying_type = std::underlying_type_t<ID>;

constexpr id_underlying_type null =
    std::numeric_limits<id_underlying_type>::max();

constexpr bool operator==(NullID, NullID) noexcept {
    return true;
}

constexpr bool operator!=(NullID, NullID) noexcept {
    return false;
}

constexpr bool operator==(NullID, ID o) noexcept {
    return static_cast<id_underlying_type>(o) == null;
}

constexpr bool operator==(ID o, NullID null) noexcept {
    return null == o;
}

constexpr bool operator!=(NullID null, ID o) noexcept {
    return !(null == o);
}

constexpr bool operator!=(ID o, NullID null) noexcept {
    return null != o;
}

constexpr NullID::operator ID() const {
    return static_cast<ID>(null);
}

}  // namespace nickel::ecs