#pragma once

#include "graphics/rhi/impl/buffer.hpp"
#include "graphics/rhi/vk/pch.hpp"
#include "graphics/rhi/vk/util.hpp"


namespace nickel::rhi::vulkan {

class DeviceImpl;

class BufferImpl : public rhi::BufferImpl {
public:
    BufferImpl(DeviceImpl&, vk::PhysicalDevice, const Buffer::Descriptor&);
    ~BufferImpl();

    enum Buffer::MapState MapState() const override;
    uint64_t Size() const override;
    void Unmap() override;
    void MapAsync(Flags<Buffer::Mode>, uint64_t offset, uint64_t size) override;
    void* GetMappedRange() override;
    void* GetMappedRange(uint64_t offset) override;
    void* GetMappedRange(uint64_t offset, uint64_t size) override;
    void Flush() override;
    void Flush(uint64_t offset, uint64_t size) override;
    bool IsMappingCoherence() const override;

    vk::Buffer buffer;
    vk::DeviceMemory mem;

private:
    uint64_t size_{};
    enum Buffer::MapState mapState_ = Buffer::MapState::Unmapped;
    vk::Device device_;
    void* map_{};
    uint64_t mappedOffset_{};
    uint64_t mappedSize_{};
    bool isMappingCoherence_{};

    vk::MemoryPropertyFlags getMemoryProperty(vk::PhysicalDevice phyDevice, const Buffer::Descriptor&);
    void createBuffer(uint64_t size, vk::BufferUsageFlags usage,
                      const std::vector<uint32_t>& indices, bool hostVisible);
    void allocateMem(vk::PhysicalDevice phyDevice,
                     vk::MemoryPropertyFlags flags);
};

}  // namespace nickel::rhi::vulkan