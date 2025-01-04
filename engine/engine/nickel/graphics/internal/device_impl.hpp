#pragma once
#include "nickel/graphics/device.hpp"
#include "nickel/internal/pch.hpp"

namespace nickel::graphics {

class DeviceImpl {
public:
    struct QueueFamilyIndices {
        std::optional<uint32_t> m_graphics_index;
        std::optional<uint32_t> m_present_index;

        explicit operator bool() const {
            return m_graphics_index && m_present_index;
        }

        std::set<uint32_t> GetUniqueIndices() const {
            return {m_graphics_index.value(), m_present_index.value()};
        }

        bool HasSeparateQueue() const {
            return m_graphics_index.value() != m_present_index.value();
        }

        std::vector<uint32_t> GetIndices() const {
            std::vector<uint32_t> indices;
            if (!m_graphics_index.has_value() || !m_present_index.has_value()) {
                return {};
            }

            if (m_graphics_index.has_value() == m_present_index.has_value()) {
                indices.push_back(m_graphics_index.value());
            } else {
                indices.push_back(m_graphics_index.value());
                indices.push_back(m_present_index.value());
            }
            return indices;
        }
    };

    struct ImageInfo {
        SVector<uint32_t, 2> extent;
        uint32_t imagCount;
        VkSurfaceFormatKHR format;
    };

    DeviceImpl(const AdapterImpl&, const SVector<uint32_t, 2>& window_size);
    ~DeviceImpl();

    const ImageInfo& GetSwapchainImageInfo() const noexcept;

    VkDevice m_device;
    VkQueue m_present_queue;
    VkQueue m_graphics_queue;
    VkSwapchainKHR m_swapchain;
    std::vector<VkImageView> m_image_views;
    QueueFamilyIndices m_queue_indices;

private:
    ImageInfo m_image_info;

    QueueFamilyIndices chooseQueue(VkPhysicalDevice phyDevice,
                                   VkSurfaceKHR surface);

    void createSwapchain(VkPhysicalDevice phyDev, VkSurfaceKHR surface,
                         const SVector<uint32_t, 2>& window_size);
    ImageInfo queryImageInfo(VkPhysicalDevice, const SVector<uint32_t, 2>&,
                             VkSurfaceKHR);
    VkPresentModeKHR queryPresentMode(VkPhysicalDevice, VkSurfaceKHR);

    void getAndCreateImageViews();
};

}  // namespace nickel::graphics