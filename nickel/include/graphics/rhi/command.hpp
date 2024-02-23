#pragma once
#include "graphics/rhi/buffer.hpp"
#include "graphics/rhi/command.hpp"
#include "graphics/rhi/render_pipeline.hpp"
#include "graphics/rhi/texture.hpp"
#include "graphics/rhi/texture_view.hpp"
#include "graphics/rhi/renderpass.hpp"

namespace nickel::rhi {

class CommandEncoderImpl;
class CommandBufferImpl;
class RenderPassEncoderImpl;

class CommandBuffer final {
public:
    explicit CommandBuffer(CommandBufferImpl*);
    void Destroy();

    auto Impl() const { return impl_; }
    auto Impl() { return impl_; }

private:
    CommandBufferImpl* impl_{};
};


class RenderPassEncoder final {
public:
    RenderPassEncoder(RenderPassEncoderImpl* impl) : impl_{impl} {}

    void Draw(uint32_t vertexCount, uint32_t instanceCount,
              uint32_t firstVertex, uint32_t firstInstance);
    void DrawIndexed(uint32_t indexCount, uint32_t instanceCount,
                     uint32_t firstIndex, uint32_t baseVertex,
                     uint32_t firstInstance);
    void SetVertexBuffer(uint32_t slot, Buffer buffer, uint64_t offset,
                         uint64_t size);
    void SetIndexBuffer(Buffer buffer, IndexType, uint32_t offset,
                        uint32_t size);
    void SetBindGroup(uint32_t index, BindGroup);
    void SetPipeline(RenderPipeline);

    void End();

private:
    RenderPassEncoderImpl* impl_{};
};

class CommandEncoder final {
public:
    struct BufTexCopySrc final {
        Buffer buffer;
        uint64_t offset;
        uint64_t bytesPerRow = 0;   // 0 means whole buffer
        uint64_t rowsPerImage = 0;  // 0 means whole buffer
    };

    struct BufTexCopyDst final {
        TextureAspect aspect = TextureAspect::All;
        uint32_t miplevel = 0;
        // cgmath::Vec3 origin;
        Texture texture;
    };

    explicit CommandEncoder(CommandEncoderImpl*);

    void CopyBufferToBuffer(const Buffer& src, uint64_t srcOffset,
                            const Buffer& dst, uint64_t dstOffset,
                            uint64_t size);
    void CopyBufferToTexture(const CommandEncoder::BufTexCopySrc& src,
                             const CommandEncoder::BufTexCopyDst& dst,
                             const Extent3D& copySize);
    RenderPassEncoder BeginRenderPass(const RenderPass::Descriptor&);
    CommandBuffer Finish();

    void Destroy();

private:
    CommandEncoderImpl* impl_{};
};

}  // namespace nickel::rhi