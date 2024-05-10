#pragma once

#include <cstdint>

namespace nickel::rhi {

inline uint32_t AlignTo(uint32_t value, uint32_t alignment) {
    return (value + alignment - 1) & ~(alignment - 1);
}

}