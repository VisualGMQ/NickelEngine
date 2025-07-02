#pragma once
#include "nickel/common/memory/memory.hpp"
#include "nickel/ecs/internal/entity_operations.hpp"
#include "nickel/ecs/internal/sparse_set.hpp"
#include "nickel/ecs/record.hpp"
#include "nickel/ecs/table.hpp"

namespace nickel::ecs {

struct EntitySparseSetPolicy {
    using key_type = Entity;
    using value_type = RecordHandle;

    size_t GetIndexFromKey(const key_type& key) const noexcept {
        return EntityGetID(key);
    }

    size_t GetIndexFromValue(const value_type& value) const noexcept {
        return value->m_dense;
    }

    Entity GetInvalidKey() const noexcept { return null_id; }

    value_type GetInvalidValue() const noexcept { return {}; }

    void RecordDenseIndex(value_type& value, size_t idx) noexcept {
        value->m_dense = idx;
    }

    void ReuseKey(key_type& key) noexcept {
        // TODO: make next generation key
    }
};

class World {
public:
private:
    TableHandle m_root_table;
    SparseSet<Entity, RecordHandle, EntitySparseSetPolicy> m_records;
    BlockMemoryAllocator<Table> m_table_allocator;
    BlockMemoryAllocator<Record> m_record_allocator;
};

}