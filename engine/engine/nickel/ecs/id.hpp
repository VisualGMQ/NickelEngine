#pragma once
#include "nickel/ecs/config.hpp"

namespace nickel::ecs {

enum class ID : IDUnderlyingType {};

struct NullID {
    constexpr operator ID() const;
};

constexpr bool operator==(NullID, NullID) noexcept;
constexpr bool operator!=(NullID, NullID) noexcept;
constexpr bool operator==(NullID, ID o) noexcept;
constexpr bool operator==(ID o, NullID null) noexcept;
constexpr bool operator!=(NullID null, ID o) noexcept;
constexpr bool operator!=(ID o, NullID null) noexcept;

using ComponentID = ID;
using Entity = ID;

constexpr NullID null_id;

}  // namespace nickel::ecs