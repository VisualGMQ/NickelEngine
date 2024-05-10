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
    CommandBuffer() = default;
    explicit CommandBuffer(CommandBufferImpl*);

    auto Impl() const { return impl_; }
    auto Impl() { return impl_; }

    operator bool() const {
        return impl_;
    }

private:
    CommandBufferImpl* impl_{};
};


class RenderPassEncoder final {
public:
    RenderPassEncoder() = default;
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
    void SetBindGroup(BindGroup);
    void SetBindGroup(BindGroup, const std::vector<uint32_t>& dynamicOffset);
    void SetPipeline(RenderPipeline);
    void SetPushConstant(ShaderStage stage, const void* value, uint32_t offset, uint32_t size);
    void SetViewport(float x, float y, float width, float height);

    void End();

private:
    RenderPassEncoderImpl* impl_{};
};

class CommandEncoder final {
public:
    struct BufTexCopySrc final {
        Buffer buffer;
        uint64_t offset;
        uint64_t rowLength = 0;   // 0 means whole buffer, in texel
        uint64_t rowsPerImage = 0;  // 0 means whole buffer
    };

    struct BufTexCopyDst final {
        Flags<TextureAspect> aspect = TextureAspect::ColorOnly;
        uint32_t miplevel = 0;
        Offset3D origin;
        Texture texture;
    };

    CommandEncoder() = default;
    explicit CommandEncoder(CommandEncoderImpl*);
    CommandEncoder(CommandEncoder&& o) noexcept { swap(o, *this); }
    CommandEncoder(const CommandEncoder& o) = default;
    CommandEncoder& operator=(const CommandEncoder& o) = default;

    CommandEncoder& operator=(CommandEncoder&& o) noexcept {
        if (&o != this) {
            swap(o, *this);
        }
        return *this;
    }

    void CopyBufferToBuffer(const Buffer& src, uint64_t srcOffset,
                            const Buffer& dst, uint64_t dstOffset,
                            uint64_t size);
    void CopyBufferToTexture(const CommandEncoder::BufTexCopySrc& src,
                             const CommandEncoder::BufTexCopyDst& dst,
                             const Extent3D& copySize);
    [[nodiscard]] RenderPassEncoder BeginRenderPass(const RenderPass::Descriptor&);
    [[nodiscard]] CommandBuffer Finish();

    void Destroy();

   CommandEncoderImpl* Impl() const;

private:
    CommandEncoderImpl* impl_{};

    friend void swap(CommandEncoder& o1, CommandEncoder& o2) noexcept {
        using std::swap;

        swap(o1.impl_, o2.impl_);
    }
};

}  // namespace nickel::rhi