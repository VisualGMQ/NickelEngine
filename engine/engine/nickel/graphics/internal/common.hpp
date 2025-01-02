#pragma once
#include "nickel/internal/pch.hpp"
#include <optional>

namespace nickel::graphics {

inline std::optional<uint32_t> FindMemoryType(
    VkPhysicalDevice device, const VkMemoryRequirements& requirements,
    VkMemoryPropertyFlags prop) {
    VkPhysicalDeviceMemoryProperties props;
    vkGetPhysicalDeviceMemoryProperties(device, &props);
    for (uint32_t i = 0; i < props.memoryTypeCount; i++) {
        if ((1 << i & requirements.memoryTypeBits) &&
            (props.memoryTypes[i].propertyFlags & prop) == prop) {
            return i;
        }
    }
    return {};
}

}  // namespace nickel::graphics