#include "nickel/ecs/component.hpp"

namespace nickel::ecs {

bool NullComponent::operator==(NullComponent) const noexcept {
    return true;
}

bool NullComponent::operator!=(NullComponent) const noexcept {
    return false;
}

bool NullComponent::operator==(ComponentID c) const noexcept {
    return (static_cast<ComponentUnderlyingType>(c) & null_component_id) ==
           null_component_id;
}

bool NullComponent::operator!=(ComponentID c) const noexcept {
    return !(*this == c);
}

bool operator==(ComponentID e, NullComponent n) {
    return n == e;
}

bool operator!=(ComponentID component_id, NullComponent null_component) {
    return !(component_id == null_component);
}

}  // namespace nickel::ecs