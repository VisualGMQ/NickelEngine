#pragma once

#include "core/cgmath.hpp"
#include "rhi/framebuffer.hpp"
#include "rhi/pipeline.hpp"
#include "rhi/renderpass.hpp"
#include "rhi/buffer.hpp"


namespace nickel::rhi {

// NOTE: correspond to VkClearValue(will explicit convert to VkClearValue)
struct ClearValue final {
    std::array<float, 4> colorValue = {0, 0, 0, 1};
    float depth = 0;
    uint32_t stencil = 0;
};

// commands
struct RenderPassBeginInfo final {
    const RenderPass& renderpass;
    const Framebuffer& framebuffer;
    cgmath::Rect renderArea;
    std::vector<ClearValue> clearValues;
};

class CommandBuffer {
public:
    virtual void BeginRecord(CommandBufferUsage) = 0;
    virtual void BindPipeline(PipelineBindPoint bind,
                              const Pipeline& pipeline) = 0;
    virtual void Draw(uint32_t vertexCount, uint32_t instanceCount,
                      uint32_t firstVertex, uint32_t firstInstance) = 0;
    virtual void BindVertexBuffer(uint32_t firstBinding,
                                  const std::vector<Buffer*>&,
                                  const std::vector<uint64_t>& offsets) = 0;
    virtual void EndRecord() = 0;
    virtual void Reset() = 0;
    virtual ~CommandBuffer() = default;
};

class CommandPool {
public:
    virtual ~CommandPool() = default;
};

}  // namespace nickel::rhi