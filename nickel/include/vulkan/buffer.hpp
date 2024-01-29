#pragma once

#include "pch.hpp"
#include "vulkan/memory.hpp"

namespace nickel::vulkan {

class Device;

class Buffer final {
public:
    Buffer(Device*, uint64_t size, vk::BufferUsageFlags usage,
           vk::MemoryPropertyFlags flags,
           const std::set<uint32_t>& queueIndices);
    ~Buffer();

    Buffer(Buffer&& o) { swap(o, *this); }

    Buffer& operator=(Buffer&& o) {
        if (&o != this) {
            swap(o, *this);
        }
        return *this;
    }

    vk::BufferUsageFlags Usage() const noexcept { return usage_; }

    uint64_t Size() const noexcept { return size_; }

    void* Map(uint64_t offset, uint64_t size);
    void Unmap();

    vk::Buffer GetBuffer() const { return buffer_; }

    vk::DeviceMemory GetDeviceMemory() const { return mem_; }

private:
    Device* device_{};
    uint64_t size_ = 0;
    vk::BufferUsageFlags usage_;
    vk::MemoryPropertyFlags prop_;
    vk::Buffer buffer_;
    vk::DeviceMemory mem_;

    void createBuffer(uint64_t size, vk::BufferUsageFlags usage,
                      const std::vector<uint32_t>& queueIndices);

    void allocateMem(vk::Buffer buffer, vk::MemoryPropertyFlags flags);

    friend void swap(Buffer& o1, Buffer& o2) noexcept {
        using std::swap;
        swap(o1.device_, o2.device_);
        swap(o1.buffer_, o2.buffer_);
        swap(o1.usage_, o2.usage_);
        swap(o1.size_, o2.size_);
        swap(o1.prop_, o2.prop_);
        swap(o1.mem_, o2.mem_);
    }
};

}  // namespace nickel::vulkan