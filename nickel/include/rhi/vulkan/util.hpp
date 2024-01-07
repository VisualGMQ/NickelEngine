#pragma once

#include "pch.hpp"

namespace nickel::rhi::vulkan {

template <typename T, typename U>
void RemoveUnexistsElems(std::vector<T>& requires,
                         const std::vector<U>& supports,
                         std::function<bool(const T&, const U&)> isEqual) {
    for (int i = requires.size() - 1; i >= 0; i--) {
        bool found = false;
        auto& require = requires[i];
        for (auto& support : supports) {
            if (isEqual(require, support)) {
                found = true;
            }
        }

        if (!found) {
            requires.erase(requires.begin() + i);
        }
    }
}

/**
 * @brief if flags has enum e, then attach mapped enum on mappedFlags
 */
template <typename Enum, typename MappedEnum,
          typename Underlying = std::enable_if_t<std::is_enum_v<Enum>,
                                                 std::underlying_type_t<Enum>>>
void AttachMappedEnum(Underlying flags, Enum e, Underlying& mappedFlags,  MappedEnum mapped) {
    if (flags & static_cast<Underlying>(e)) {
        mappedFlags |= static_cast<Underlying>(mapped);
    }
}

}  // namespace nickel::rhi::vulkan