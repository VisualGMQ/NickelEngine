#pragma once
#include "graphics/rhi/command.hpp"
#include "graphics/rhi/framebuffer.hpp"
#include "graphics/rhi/renderpass.hpp"

namespace nickel::rhi {

class RenderPassEncoderImpl {
public:
    virtual ~RenderPassEncoderImpl() = default;

    virtual void Draw(uint32_t vertexCount, uint32_t instanceCount,
                      uint32_t firstVertex, uint32_t firstInstance) = 0;
    virtual void DrawIndexed(uint32_t indexCount, uint32_t instanceCount,
                             uint32_t firstIndex, uint32_t baseVertex,
                             uint32_t firstInstance) = 0;
    virtual void SetVertexBuffer(uint32_t slot, Buffer buffer, uint64_t offset,
                                 uint64_t size) = 0;
    virtual void SetIndexBuffer(Buffer buffer, IndexType, uint32_t offset,
                                uint32_t size) = 0;
    virtual void SetBindGroup(BindGroup) = 0;
    virtual void SetBindGroup(BindGroup, const std::vector<uint32_t>& dynamicOffset) = 0;
    virtual void SetPipeline(RenderPipeline) = 0;

    virtual void End() = 0;

private:
};

class CommandEncoderImpl {
public:
    virtual ~CommandEncoderImpl() = default;

    virtual CommandBuffer Finish() = 0;
    virtual void CopyBufferToBuffer(const Buffer& src, uint64_t srcOffset,
                                    const Buffer& dst, uint64_t dstOffset,
                                    uint64_t size) = 0;
    virtual void CopyBufferToTexture(const CommandEncoder::BufTexCopySrc& src,
                                     const CommandEncoder::BufTexCopyDst& dst,
                                     const Extent3D& copySize) = 0;

    virtual RenderPassEncoder BeginRenderPass(
        const RenderPass::Descriptor&) = 0;
};

class CommandBufferImpl {
public:
    virtual ~CommandBufferImpl() = default;
};

}  // namespace nickel::rhi