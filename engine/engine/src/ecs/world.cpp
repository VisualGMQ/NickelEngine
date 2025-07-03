#include "nickel/ecs/world.hpp"

#include "nickel/ecs/internal/compomnent_operations.hpp"

namespace nickel::ecs {

World::World() {
    m_root_table = TableHandle{m_table_allocator.Allocate(), m_table_allocator};
}

ComponentRecord* World::GetComponentRecord(ComponentID id) {
    auto cid = ComponentGetID(id);
    if (cid < LowComponentID) {
        if (m_lo_component_records[cid].m_id != null_id) {
            return &m_lo_component_records[cid];
        }
    } else {
        if (auto it = m_hi_component_records.find(id); it != m_hi_component_records.end()) {
            return &it->second;
        }
    }
    return nullptr;
}

}  // namespace nickel::ecs