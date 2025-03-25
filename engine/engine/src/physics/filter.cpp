#include "nickel/physics/filter.hpp"
#include "nickel/common/bit_manipulate.hpp"

namespace nickel::physics {

void CollisionGroupFilter::AddCollisionGroup(CollisionGroup group) {
    m_bits |= 1 << static_cast<std::underlying_type_t<CollisionGroup>>(group);
}

void CollisionGroupFilter::RemoveCollisionGroup(CollisionGroup group) {
    m_bits &= ~(1 << static_cast<std::underlying_type_t<CollisionGroup>>(group));
}

void CollisionGroupFilter::AddAllCollisionGroup() {
    m_bits = GenFullBitMast(CollisionGroupContainBits);
}

uint32_t CollisionGroupFilter::GetFilter() const {
    return m_bits;
}

}  // namespace nickel::physics