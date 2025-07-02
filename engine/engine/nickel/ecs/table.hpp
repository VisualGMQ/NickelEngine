#pragma once
#include "nickel/common/handle.hpp"
#include "nickel/common/memory/memory.hpp"
#include "nickel/ecs/id.hpp"

namespace nickel::ecs {

using TableID = ID;

struct Table;

using TableHandle = Handle<Table, BlockMemoryAllocator<Table>>;

struct Table {
    struct Column {
        std::vector<void*> m_components;
    };

    std::vector<ComponentID>
        m_component_types;           /// component id of types(column header)
    std::vector<Entity> m_entities;  /// stored entities(row header)
    std::vector<Column> m_columns;   /// columns stored components

    struct Edge {
        TableHandle m_from;
        TableHandle m_to;
    };

    struct GraphNode {
        std::unordered_map<ComponentID, Edge> m_add_edges;
        std::unordered_map<ComponentID, Edge> m_remove_edges;
    };
};

}  // namespace nickel::ecs