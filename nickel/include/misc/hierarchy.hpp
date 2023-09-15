#pragma once

#include "core/ecs.hpp"
#include "misc/transform.hpp"

namespace nickel {

/**
 * @brief use for transform in hierarchy
 */
struct NodeTransform {
    Transform localTransform;
    Transform globalTransform;
};

/**
 * @brief a componet tag to point out root node in hierarchy
 */
struct RootNode { };

}  // namespace nickel