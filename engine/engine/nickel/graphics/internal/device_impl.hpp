#pragma once
#include "nickel/graphics/adapter.hpp"
#include "nickel/graphics/device.hpp"
#include "nickel/internal/pch.hpp"

namespace nickel::graphics {

class Device::Impl {
public:
    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsIndex;
        std::optional<uint32_t> presentIndex;

        explicit operator bool() const { return graphicsIndex && presentIndex; }

        std::set<uint32_t> GetUniqueIndices() const {
            return {graphicsIndex.value(), presentIndex.value()};
        }

        bool HasSeperateQueue() const {
            return graphicsIndex.value() != presentIndex.value();
        }
    };

    Impl(const Adapter::Impl&);
    ~Impl();

private:
    VkDevice m_device;
    VkQueue m_present_queue;
    VkQueue m_graphics_queue;
    QueueFamilyIndices m_queue_indices;

    QueueFamilyIndices chooseQueue(VkPhysicalDevice phyDevice,
                                   VkSurfaceKHR surface);
};

}  // namespace nickel::graphics