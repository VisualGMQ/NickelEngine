#pragma once

#include "pch.hpp"
#include "rhi/buffer.hpp"
#include "rhi/enums.hpp"

namespace nickel::rhi::vulkan {

class Device;

class Buffer : public rhi::Buffer {
public:
    Buffer(Device* device, uint64_t size, BufferUsageFlags usage,
           std::optional<std::reference_wrapper<std::vector<uint32_t>>> queueIndices);
    Buffer(const Buffer&) = delete;
    Buffer& operator=(const Buffer&) = delete;

    Buffer(Buffer&& o) { swap(*this, o); }

    Buffer& operator=(Buffer&& o) {
        if (this != &o) {
            swap(*this, o);
        }
        return *this;
    }

    ~Buffer();

    auto& Raw() const { return buffer_; }

private:
    vk::Buffer buffer_;
    Device* device_{};

    friend void swap(Buffer& o1, Buffer& o2) noexcept {
        using std::swap;
        swap(o1.buffer_, o2.buffer_);
        swap(o1.device_, o2.device_);
    }
};

class DeviceMemory;

struct MemoryGuard {
    void* memory = nullptr;

    MemoryGuard(DeviceMemory& mem, uint64_t offset, uint64_t size);
    MemoryGuard(DeviceMemory& mem, void* memory);
    MemoryGuard(const MemoryGuard&) = delete;
    MemoryGuard& operator=(const MemoryGuard&) = delete;

    MemoryGuard(MemoryGuard&& o) { swap(*this, o); }

    MemoryGuard& operator=(MemoryGuard&& o) {
        if (this != &o) {
            swap(*this, o);
        }
        return *this;
    }

    ~MemoryGuard();

    void Unmap();

private:
    DeviceMemory* mem_{};

    friend void swap(MemoryGuard& o1, MemoryGuard& o2) {
        using std::swap;
        swap(o1.memory, o2.memory);
        swap(o1.mem_, o2.mem_);
    }
};

class DeviceMemory : public rhi::DeviceMemory {
public:
    friend class MemoryGuard;

    DeviceMemory(Device* device, const Buffer& buffer,
                 MemoryPropertyFlags flags);
    DeviceMemory(const DeviceMemory&) = delete;
    DeviceMemory& operator=(const DeviceMemory&) = delete;

    DeviceMemory(DeviceMemory&& o) { swap(o, *this); }

    DeviceMemory& operator=(DeviceMemory&& o) {
        if (&o != this) {
            swap(*this, o);
        }
        return *this;
    }

    ~DeviceMemory();

    MemoryGuard Map(uint64_t offset, uint64_t size);

private:
    Device* device_{};
    vk::DeviceMemory mem_;

    std::optional<uint32_t> findMemoryType(const vk::MemoryRequirements&,
                                           vk::MemoryPropertyFlags);

    friend void swap(DeviceMemory& o1, DeviceMemory& o2) noexcept {
        using std::swap;
        swap(o1.device_, o2.device_);
        swap(o1.mem_, o2.mem_);
    }

    void* map(uint64_t offset, uint64_t size);
    void unmap();
};

}  // namespace nickel::rhi::vulkan
