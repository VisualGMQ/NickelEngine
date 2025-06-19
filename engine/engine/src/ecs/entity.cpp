#include "nickel/ecs/entity.hpp"

namespace nickel::ecs {

bool NullEntity::operator==(const NullEntity&) const noexcept {
    return true;
}

bool NullEntity::operator!=(const NullEntity&) const noexcept {
    return false;
}

bool NullEntity::operator==(const Entity& e) const noexcept {
    auto id = static_cast<EntityUnderlyingType>(e);
    return (id & null_entity_id) == null_entity_id;
}

bool NullEntity::operator!=(const Entity& e) const noexcept {
    return !(*this == e);
}

NullEntity::operator Entity() const noexcept {
    return static_cast<Entity>(null_entity_id);
}

bool operator==(Entity e, NullEntity n) {
    return n == e;
}

bool operator!=(Entity entity, NullEntity null_entity) {
    return !(entity == null_entity);
}

}  // namespace nickel::ecs