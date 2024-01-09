#pragma once

#include "pch.hpp"
#include "rhi/commands.hpp"
#include "rhi/enums.hpp"
#include "rhi/vulkan/framebuffer.hpp"
#include "rhi/vulkan/renderpass.hpp"

namespace nickel::rhi::vulkan {

inline vk::RenderPassBeginInfo RenderPassBeginInfo2Vk(
    const RenderPassBeginInfo& info) {
    vk::RenderPassBeginInfo beginInfo;
    beginInfo
        .setRenderPass(
            static_cast<const vulkan::RenderPass&>(info.renderpass).Raw())
        .setFramebuffer(
            static_cast<const vulkan::Framebuffer&>(info.framebuffer).Raw())
        .setRenderArea(vk::Rect2D(
            vk::Offset2D(info.renderArea.position.x,
                         info.renderArea.position.y),
            vk::Extent2D(info.renderArea.size.w, info.renderArea.size.h)));
    beginInfo.setPClearValues((vk::ClearValue*)info.clearValues.data())
        .setClearValueCount(info.clearValues.size());
    return beginInfo;
}

class Device;

class CommandBuffer : public rhi::CommandBuffer {
public:
    explicit CommandBuffer(vk::CommandBuffer cmd);

    void BeginRecord(CommandBufferUsage usage) override;
    void BindPipeline(PipelineBindPoint bind,
                      const rhi::Pipeline& pipeline) override;
    void Draw(uint32_t vertexCount, uint32_t instanceCount,
              uint32_t firstVertex, uint32_t firstInstance) override;
    void BindVertexBuffer(uint32_t firstBinding,
                          const std::vector<BufferBundle*>&,
                          const std::vector<uint64_t>& offsets) override;
    void EndRecord() override;
    void Reset() override;

private:
    vk::CommandBuffer cmd_;

    // inner struct to save convert datas
    struct bindBuffer final {
        std::vector<vk::Buffer> buffers;
    };

    std::vector<bindBuffer> bindBuffers_;
};

class CommandPool : public rhi::CommandPool {
public:
    CommandPool(Device* device, CommandPoolCreateFlag flag,
                uint32_t queueIndex);
    CommandPool(const CommandPool&) = delete;
    CommandPool& operator=(const CommandPool&) = delete;

    CommandPool(CommandPool&& o) { swap(*this, o); }

    CommandPool& operator=(CommandPool&& o) {
        if (&o != this) {
            swap(o, *this);
        }
        return *this;
    }

    ~CommandPool();

    std::vector<CommandBuffer> Allocate(CommandBufferLevel, uint32_t count);
    void Reset();

private:
    Device* device_{};
    vk::CommandPool pool_;

    friend void swap(CommandPool& o1, CommandPool& o2) noexcept {
        using std::swap;
        swap(o1.device_, o2.device_);
        swap(o1.pool_, o2.pool_);
    }
};

}  // namespace nickel::rhi::vulkan