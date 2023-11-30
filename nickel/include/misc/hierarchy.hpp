#pragma once

#include "pch.hpp"
#include "gecs/entity/entity.hpp"
#include "misc/transform.hpp"

namespace nickel {

/**
 * @brief [component] save parent entity
 */
struct Parent {
    gecs::entity entity;
};

/**
 * @brief [component] save child entities
 */
struct Child {
    std::vector<gecs::entity> entities;
};

void UpdateGlobalTransform(
    gecs::querier<gecs::mut<GlobalTransform>, gecs::mut<Transform>, Child,
                  gecs::without<Parent>>,
    gecs::querier<gecs::mut<GlobalTransform>, gecs::mut<Transform>,
                  gecs::without<Parent>>,
    gecs::registry);

}  // namespace nickel