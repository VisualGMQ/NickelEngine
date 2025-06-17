#pragma once
#include "nickel/ecs/config.hpp"

namespace nickel::ecs {

enum class ComponentID : ComponentUnderlyingType {};

struct NullComponent {
    bool operator==(NullComponent) const noexcept;
    bool operator!=(NullComponent) const noexcept;
    bool operator==(ComponentID) const noexcept;
    bool operator!=(ComponentID) const noexcept;

    operator ComponentID() const noexcept;

private:
    static constexpr ComponentUnderlyingType null_component_id =
        ComponentIDMask.mask;
};

bool operator==(ComponentID, NullComponent);
bool operator!=(ComponentID, NullComponent);

inline NullComponent null_component;

}  // namespace nickel::ecs