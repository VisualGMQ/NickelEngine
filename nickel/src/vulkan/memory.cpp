#include "vulkan/memory.hpp"

namespace nickel::vulkan {

std::optional<uint32_t> FindMemoryType(vk::PhysicalDevice device,
    const vk::MemoryRequirements& requirements, vk::MemoryPropertyFlags prop) {
    auto properties = device.getMemoryProperties();
    for (uint32_t i = 0; i < properties.memoryTypeCount; i++) {
        if (((1 << i) & requirements.memoryTypeBits) &&
            (properties.memoryTypes[i].propertyFlags & prop) == prop) {
            return i;
        }
    }
    return {};
}

}