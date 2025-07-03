#include "nickel/ecs/table.hpp"

namespace nickel::ecs {

Table::Table() {
    m_component_index_map.fill(invalid_index);
}

}  // namespace nickel::ecs