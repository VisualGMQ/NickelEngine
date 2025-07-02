#pragma once
#include "nickel/ecs/table.hpp"

namespace nickel::ecs {

struct Record {
    TableHandle m_table;
    size_t m_row{};    /// entity row index of table
    size_t m_dense{};  /// index in sparse set dense array of entity
};

using RecordHandle = Handle<Record, BlockMemoryAllocator<Record>>;

}