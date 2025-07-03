#pragma once
#include "nickel/common/handle.hpp"
#include "nickel/common/memory/memory.hpp"
#include "nickel/ecs/id.hpp"

namespace nickel::ecs {

struct Table;

using TableHandle = Handle<Table, BlockMemoryAllocator<Table>>;

struct HeterogeneousListBase {};

template <typename T>
struct HeterogeneousList : public HeterogeneousListBase {
    std::vector<std::array<std::byte, sizeof(T)>> m_datas;
};

struct Table {
    struct Column {
        std::unique_ptr<HeterogeneousListBase> m_datas;
    };

    static constexpr size_t invalid_index = std::numeric_limits<size_t>::max();

    std::vector<ComponentID>
        m_component_types;           /// component id of types(column header)
    std::vector<Entity> m_entities;  /// stored entities(row header)
    std::array<size_t, LowComponentID>
        m_component_index_map;      /** quick component index accessor for lower
                                       component ID*/
    std::vector<Column> m_columns;  /// columns stored components

    struct Edge {
        TableHandle m_from;
        TableHandle m_to;
    };

    struct GraphNode {
        std::unordered_map<ComponentID, Edge> m_add_edges;
        std::unordered_map<ComponentID, Edge> m_remove_edges;
    };

    Table();
};

}  // namespace nickel::ecs