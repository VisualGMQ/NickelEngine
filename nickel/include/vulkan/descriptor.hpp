#pragma once

#include "pch.hpp"

namespace nickel::vulkan {

class Device;

class DescriptorPool final {
public:
    DescriptorPool(Device* device,
                   const std::vector<vk::DescriptorPoolSize>& sizes,
                   uint32_t maxSets);
    DescriptorPool(const DescriptorPool&) = delete;
    DescriptorPool& operator=(const DescriptorPool&) = delete;

    DescriptorPool(DescriptorPool&& o) { swap(o, *this); }

    DescriptorPool& operator=(DescriptorPool&& o) {
        if (&o != this) {
            swap(o, *this);
        }
        return *this;
    }

    ~DescriptorPool();

    operator vk::DescriptorPool() { return pool_; }

    operator vk::DescriptorPool() const { return pool_; }

    std::vector<vk::DescriptorSet> AllocSet(
        uint32_t count, const std::vector<vk::DescriptorSetLayout>& layouts);

private:
    Device* device_{};
    vk::DescriptorPool pool_;

    friend void swap(DescriptorPool& o1, DescriptorPool& o2) noexcept {
        using std::swap;

        swap(o1.device_, o2.device_);
        swap(o1.pool_, o2.pool_);
    }
};

class DescriptorSetLayout final {
public:
    DescriptorSetLayout(
        Device*, const std::vector<vk::DescriptorSetLayoutBinding>& bindings);
    DescriptorSetLayout(const DescriptorSetLayout&) = delete;
    DescriptorSetLayout& operator=(const DescriptorSetLayout&) = delete;

    DescriptorSetLayout(DescriptorSetLayout&& o) { swap(o, *this); }

    DescriptorSetLayout& operator=(DescriptorSetLayout&& o) {
        if (&o != this) {
            swap(o, *this);
        }
        return *this;
    }

    ~DescriptorSetLayout();

    operator vk::DescriptorSetLayout() { return layout_; }

    operator vk::DescriptorSetLayout() const { return layout_; }

    std::vector<vk::DescriptorSet> AllocSet(
        uint32_t count, const std::vector<vk::DescriptorSetLayout>& layouts);

private:
    Device* device_{};
    vk::DescriptorSetLayout layout_;

    friend void swap(DescriptorSetLayout& o1,
                     DescriptorSetLayout& o2) noexcept {
        using std::swap;

        swap(o1.device_, o2.device_);
        swap(o1.layout_, o2.layout_);
    }
};

}  // namespace nickel::vulkan