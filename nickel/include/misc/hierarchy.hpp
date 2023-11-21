#pragma once

#include "pch.hpp"
#include "gecs/entity/entity.hpp"
#include "misc/transform.hpp"

namespace nickel {

/**
 * @brief a componet tag to save parent entity
 */
struct Parent {
    gecs::entity entity;
};

/**
 * @brief a componet tag to save child entities
 */
struct Child {
    std::vector<gecs::entity> entities;
};

void UpdateGlobalTransform(
    gecs::querier<gecs::mut<GlobalTransform>, gecs::mut<Transform>, Parent>
        querier,
    gecs::registry reg);

}  // namespace nickel