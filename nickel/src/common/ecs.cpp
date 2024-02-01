#include "common/ecs.hpp"

namespace nickel {

gecs::world& ECS::World() {
    return world_;
}

const gecs::world& ECS::World() const {
    return world_;
}

}