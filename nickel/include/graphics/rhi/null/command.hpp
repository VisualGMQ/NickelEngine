#pragma once

#include "graphics/rhi/impl/command.hpp"

namespace nickel::rhi::null {

class RenderPassEncoderImpl : public rhi::RenderPassEncoderImpl {
public:
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
    void SetBindGroup(BindGroup, const std::vector<uint32_t>&) override;
    void SetPipeline(RenderPipeline) override;
    void SetPushConstant(ShaderStage stage, void* value, uint32_t offset, uint32_t size) override;

    void End() override;
};

class CommandEncoderImpl : public rhi::CommandEncoderImpl {
public:
    CommandBuffer Finish() override;
    void CopyBufferToBuffer(const Buffer& src, uint64_t srcOffset,
                            const Buffer& dst, uint64_t dstOffset,
                            uint64_t size) override;
    void CopyBufferToTexture(const CommandEncoder::BufTexCopySrc& src,
                             const CommandEncoder::BufTexCopyDst& dst,
                             const Extent3D& copySize) override;

    RenderPassEncoder BeginRenderPass(const RenderPass::Descriptor&) override;
};

class CommandBufferImpl : public rhi::CommandBufferImpl {};

}  // namespace nickel::rhi::null