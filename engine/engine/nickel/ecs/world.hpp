#pragma once
#include "nickel/ecs/internal/entity_operations.hpp"
#include "nickel/ecs/archetype.hpp"
#include "nickel/ecs/internal/sparse_set.hpp"

namespace nickel::ecs {

struct EntitySparseSetPolicy {
    using key_type = Entity;
    using value_type = std::shared_ptr<Record>;

    size_t GetIndexFromKey(const key_type& key) const noexcept {
        return EntityGetID(key);
    }

    size_t GetIndexFromValue(const value_type& value) const noexcept {
        return value->m_dense;
    }

    Entity GetInvalidKey() const noexcept { return null_entity; }

    value_type GetInvalidValue() const noexcept { return nullptr; }

    void RecordDenseIndex(value_type& value, size_t idx) noexcept {
        value->m_dense = idx;
    }

    void ReuseKey(key_type& key) noexcept {
        // make next generation key
    }
};

class World {
public:
private:
    std::shared_ptr<Table> m_root_table;
    SparseSet<Entity, std::shared_ptr<Record>, EntitySparseSetPolicy> m_records;
};

}