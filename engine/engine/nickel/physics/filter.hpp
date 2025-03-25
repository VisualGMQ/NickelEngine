#pragma once
#include "nickel/common/flags.hpp"
#include "nickel/physics/collision_group.hpp"
#include "nickel/physics/enums.hpp"

namespace nickel::physics {

struct CollisionGroupFilter {
    void AddCollisionGroup(CollisionGroup);
    void RemoveCollisionGroup(CollisionGroup);
    void AddAllCollisionGroup();
    uint32_t GetFilter() const;

private:
    uint32_t m_bits{};
};

struct QueryFilterData {
    CollisionGroupFilter m_filter;
    Flags<QueryFlag> m_flags;
};

}  // namespace nickel::physics