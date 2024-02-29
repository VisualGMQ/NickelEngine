#pragma once

#include "graphics/rhi/bind_group.hpp"
#include "graphics/rhi/impl/command.hpp"
#include "graphics/rhi/vk/pch.hpp"
#include "graphics/rhi/vk/util.hpp"
#include "graphics/rhi/vk/render_pipeline.hpp"
#include "graphics/rhi/renderpass.hpp"

namespace nickel::rhi::vulkan {

class DeviceImpl;

class RenderPassEncoderImpl : public rhi::RenderPassEncoderImpl {
public:
    RenderPassEncoderImpl(DeviceImpl&, vk::CommandBuffer cmd);

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
    void SetBindGroup(BindGroup, const std::vector<uint32_t>& dynamicOffset) override;
    void SetPipeline(RenderPipeline) override;

    void End() override;

private:
    DeviceImpl& dev_;
    vk::CommandBuffer cmd_;
    RenderPipeline pipeline_;
};

enum class CmdType {
    None,
    RenderPass,
    CopyData,
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

    CmdType Type() const { return type_; }

private:
    vk::CommandPool pool_;
    CommandBuffer* cmdBuf_{};
    vk::CommandBuffer buf_;
    DeviceImpl& dev_;
    CmdType type_ = CmdType::None;
};

class CommandBufferImpl : public rhi::CommandBufferImpl {
public:
    friend class CommandEncoderImpl;

    explicit CommandBufferImpl(vk::CommandBuffer buf) : buf{buf} {}

    vk::CommandBuffer buf;

    CmdType Type() const { return type_; }

private:
    CmdType type_ = CmdType::None;
};

}  // namespace nickel::rhi::vulkan