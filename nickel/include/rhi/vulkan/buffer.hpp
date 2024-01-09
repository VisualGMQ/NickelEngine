#pragma once

#include "pch.hpp"
#include "rhi/buffer.hpp"
#include "rhi/enums.hpp"

namespace nickel::rhi::vulkan {

class Device;
class BufferBundle;

class Buffer {
public:
    friend class BufferBundle;

    Buffer(Device* device, uint64_t size, BufferUsageFlags usage,
           std::optional<std::reference_wrapper<std::vector<uint32_t>>>
               queueIndices);
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

    operator bool() const noexcept { return buffer_; }

    auto Usage() const noexcept { return usage_; }

    uint64_t Size() const noexcept { return size_; }

    auto& Raw() const { return buffer_; }

private:
    Device* device_{};
    vk::Buffer buffer_;
    BufferUsageFlags usage_;
    uint64_t size_ = 0;

    Buffer() = default;

    friend void swap(Buffer& o1, Buffer& o2) noexcept {
        using std::swap;
        swap(o1.buffer_, o2.buffer_);
        swap(o1.device_, o2.device_);
        swap(o1.usage_, o2.usage_);
    }
};

class DeviceMemory;

class DeviceMemory {
public:
    friend class BufferBundle;

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

    // MemoryGuard Map(uint64_t offset, uint64_t size);
    void* Map(uint64_t offset, uint64_t size);
    void Unmap();

    auto Property() const noexcept { return prop_; }

    operator bool() const noexcept { return mem_; }

private:
    Device* device_{};
    MemoryPropertyFlags prop_;
    vk::DeviceMemory mem_;

    DeviceMemory() = default;

    std::optional<uint32_t> findMemoryType(const vk::MemoryRequirements&,
                                           vk::MemoryPropertyFlags);

    friend void swap(DeviceMemory& o1, DeviceMemory& o2) noexcept {
        using std::swap;
        swap(o1.device_, o2.device_);
        swap(o1.mem_, o2.mem_);
    }

protected:
};

class BufferBundle : public rhi::BufferBundle {
public:
    BufferBundle(Device*, uint64_t size, BufferUsageFlags usage,
                 MemoryPropertyFlags flags,
                 std::optional<std::reference_wrapper<std::vector<uint32_t>>>
                     queueIndices = {});

    BufferBundle(BufferBundle&& o) { swap(o, *this); }

    BufferBundle& operator=(BufferBundle&& o) {
        if (&o != this) {
            swap(o, *this);
        }
        return *this;
    }

    BufferUsageFlags Usage() const noexcept override { return buffer_.Usage(); }

    uint64_t Size() const noexcept override { return buffer_.Size(); }

    void* Map(uint64_t offset, uint64_t size) override {
        return memory_.Map(offset, size);
    }

    void Unmap() override { return memory_.Unmap(); }

    auto& Buffer() const { return buffer_; }
    auto& DeviceMemory() const { return memory_; }

private:
    Device* device_;
    class Buffer buffer_;
    class DeviceMemory memory_;

    friend void swap(BufferBundle& o1, BufferBundle& o2) noexcept {
        using std::swap;
        swap(o1.device_, o2.device_);
        swap(o1.buffer_, o2.buffer_);
        swap(o1.memory_, o2.memory_);
    }
};

}  // namespace nickel::rhi::vulkan
