#pragma once
#include "nickel/ecs/entity.hpp"
#include "nickel/ecs/component.hpp"
#include <unordered_map>
#include <unordered_set>

namespace nickel::ecs {

struct Table final {
    struct AddRemoveEdge {
        Table* m_add{}; 
        Table* m_remove{};
        ComponentID m_component = null_component;
    } m_edge;

    std::vector<Entity> m_entities;
    std::vector<void*> m_components;
    std::vector<AddRemoveEdge> m_edges;
};

// element in sparse set
struct Record {
    size_t m_dense;
    std::shared_ptr<Table> m_archetype;
    uint32_t m_row;
};

}