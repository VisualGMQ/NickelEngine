#pragma once

#include "graphics/rhi/bind_group.hpp"
#include "graphics/rhi/impl/command.hpp"
#include "graphics/rhi/renderpass.hpp"
#include "graphics/rhi/vk/pch.hpp"
#include "graphics/rhi/vk/render_pipeline.hpp"
#include "graphics/rhi/vk/util.hpp"
#include "graphics/rhi/vk/renderpass.hpp"


namespace nickel::rhi::vulkan {

class DeviceImpl;
class RenderPassImpl;
class CommandEncoderImpl;

struct CmdCopyBuffer2Buffer final {
    Buffer src;
    uint64_t srcOffset;
    Buffer dst;
    uint64_t dstOffset;
    uint64_t size;
};

struct CmdCopyBufferToTexture final {
    CommandEncoder::BufTexCopySrc src;
    CommandEncoder::BufTexCopyDst dst;
    Extent3D copySize;
};

struct CmdDraw final {
    bool isIndexed;
    uint32_t elemCount;
    uint32_t instanceCount;
    uint32_t firstVertex;
    uint32_t firstIndex;
    uint32_t firstInstance;
};

struct CmdSetVertexBuffer final {
    uint32_t slot;
    Buffer buffer;
    uint64_t offset;
};

struct CmdSetIndexBuffer final {
    Buffer buffer;
    IndexType type;
    uint32_t offset;
};

struct CmdSetBindGroup final {
    BindGroup bindgroup;
    std::vector<uint32_t> dynamicOffsets;
};

struct CmdSetPipeline final {
    RenderPipeline pipeline;
};

struct CmdSetPushConstant final {
    ShaderStage stage;
    std::array<uint8_t, 512> value;
    uint32_t size;
    uint32_t offset;
};

struct CmdSetViewport final {
    float x, y, width, height;
};


struct CmdBeginRenderPass final {
    RenderPass::Descriptor desc;

    RenderPassInfo analyzedRenderPassInfo;
    std::vector<vk::ClearValue> clearValues;
    vk::Rect2D renderArea;
    std::vector<TextureView> views;
};

struct CmdImageBarrier final {
    vk::ImageMemoryBarrier imageMemBarrier;
    vk::PipelineStageFlags srcStage;
    vk::PipelineStageFlags dstStage;
};

struct CmdNextSubpass final {};

struct CmdEndRenderPass final {};

using Command =
    std::variant<CmdCopyBuffer2Buffer, CmdCopyBufferToTexture, CmdDraw,
                 CmdSetVertexBuffer, CmdSetIndexBuffer, CmdSetBindGroup,
                 CmdSetPipeline, CmdSetPushConstant, CmdSetViewport,
                 CmdBeginRenderPass, CmdEndRenderPass, CmdImageBarrier,
                 CmdNextSubpass>;


class CommandBufferImpl : public rhi::CommandBufferImpl {
public:
    explicit CommandBufferImpl(vk::CommandBuffer buf)
        : buf{buf} {}
    CommandBufferImpl() = default;

    vk::CommandBuffer buf;
    bool needWaitImageAvaliFence = false;
};

class RenderPassEncoderImpl : public rhi::RenderPassEncoderImpl {
public:
    explicit RenderPassEncoderImpl(CommandEncoderImpl&);

    void Draw(uint32_t vertexCount, uint32_t instanceCount,
              uint32_t firstVertex, uint32_t firstInstance) override;
    void DrawIndexed(uint32_t indexCount, uint32_t instanceCount,
                     uint32_t firstIndex, uint32_t baseVertex,
                     uint32_t firstInstance) override;
    void SetVertexBuffer(uint32_t slot, Buffer buffer, uint64_t offset,
                         uint64_t size) override;
    void SetIndexBuffer(Buffer buffer, IndexType, uint32_t offset,
                        uint32_t size) override;
    void SetBindGroup(BindGroup) override;
    void SetBindGroup(BindGroup,
                      const std::vector<uint32_t>& dynamicOffset) override;
    void SetPipeline(RenderPipeline) override;
    void SetPushConstant(ShaderStage stage, const void* value, uint32_t offset,
                         uint32_t size) override;
    void SetViewport(float x, float y, float width, float height) override;

    void End() override;

private:
    CommandEncoderImpl& encoder_;
};

class CommandEncoderImpl : public rhi::CommandEncoderImpl {
public:
    explicit CommandEncoderImpl(DeviceImpl&, vk::CommandPool);
    ~CommandEncoderImpl();

    void CopyBufferToBuffer(const Buffer& src, uint64_t srcOffset,
                            const Buffer& dst, uint64_t dstOffset,
                            uint64_t size) override;
    void CopyBufferToTexture(const CommandEncoder::BufTexCopySrc& src,
                             const CommandEncoder::BufTexCopyDst& dst,
                             const Extent3D& copySize) override;
    RenderPassEncoder BeginRenderPass(const RenderPass::Descriptor&) override;

    CommandBuffer Finish() override;

    vk::CommandPool pool;
    std::vector<Command> cmds;
    CommandBufferImpl cmdBuf;

private:
    DeviceImpl& dev_;
};

struct LayoutTransition final {
    vk::ImageLayout& oldLayout;
    vk::ImageLayout newLayout;
};

}  // namespace nickel::rhi::vulkan