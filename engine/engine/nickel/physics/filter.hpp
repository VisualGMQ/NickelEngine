#pragma once
#include "nickel/common/flags.hpp"
#include "nickel/physics/enums.hpp"

namespace nickel::physics {

struct FilterData {
    uint32_t m_word0{}, m_word1{}, m_word2{}, m_word3{};
};

struct QueryFilterData {
    FilterData m_filter;
    Flags<QueryFlag> m_flags;
};


}  // namespace nickel::physics