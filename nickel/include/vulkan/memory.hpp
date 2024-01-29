#pragma once

#include "pch.hpp"

namespace nickel::vulkan {

std::optional<uint32_t> FindMemoryType(vk::PhysicalDevice,
    const vk::MemoryRequirements& requirements, vk::MemoryPropertyFlags prop);
}