#pragma once
#include "nickel/ecs/config.hpp"
#include <limits>

namespace nickel::ecs {

enum class Entity : EntityUnderlyingType {};

struct NullEntity final {
    bool operator==(const NullEntity&) const noexcept;
    bool operator!=(const NullEntity&) const noexcept;
    bool operator==(const Entity&) const noexcept;
    bool operator!=(const Entity&) const noexcept;

private:
    // entity is null when all used bits are 1
    static constexpr EntityUnderlyingType null_entity_id =
        (std::numeric_limits<EntityUnderlyingType>::max() & EntityIDMask) |
        (std::numeric_limits<EntityUnderlyingType>::max() & EntityVersionMask);
};

bool operator==(Entity, NullEntity);
bool operator!=(Entity, NullEntity);

inline NullEntity null_entity;

}  // namespace nickel::ecs