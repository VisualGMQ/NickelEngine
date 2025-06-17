#pragma once
#include "nickel/ecs/archetype.hpp"
#include "nickel/ecs/internal/sparse_set.hpp"

namespace nickel::ecs {

class World {
public:
private:
    std::shared_ptr<Table> m_root_table;
    // SparseSet<Entity, std::shared_ptr<Record>> m_records;
};

}