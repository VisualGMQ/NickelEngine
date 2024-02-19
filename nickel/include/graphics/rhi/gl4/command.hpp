#pragma once

#include "graphics/rhi/impl/command.hpp"

namespace nickel::rhi::gl4 {

class CommandEncoderImpl;

struct CmdCopyBuf2Texture {
    CommandEncoder::BufTexCopySrc src;
    CommandEncoder::BufTexCopyDst dst;
    Extent3D copySize;
};

struct CmdCopyBuf2Buf {
    Buffer src;
    uint64_t srcOffset;
    Buffer dst;
    uint64_t dstOffset;
    uint64_t size;
};

struct CmdDraw {
    uint32_t vertexCount;
    uint32_t instanceCount;
    uint32_t firstVertex;
    uint32_t firstInstance;
};

struct CmdDrawIndexed {
    uint32_t indexCount;
    uint32_t instanceCount;
    uint32_t firstIndex;
    uint32_t baseVertex;
    uint32_t firstInstance;
};

using Command =
    std::variant<CmdCopyBuf2Buf, CmdCopyBuf2Texture, CmdDraw, CmdDrawIndexed>;

class CommandBufferImpl : public rhi::CommandBufferImpl {
public:
    std::vector<Command> cmds;
    RenderPipeline pipeline;
    std::unordered_map<uint32_t, BindGroup> bindGroups;

    void Execute();
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

private:
    CommandBufferImpl buffer_;
};

class RenderPassEncoderImpl : public rhi::RenderPassEncoderImpl {
public:
    explicit RenderPassEncoderImpl(CommandBufferImpl&);

    void Draw(uint32_t vertexCount, uint32_t instanceCount,
              uint32_t firstVertex, uint32_t firstInstance) override;
    void DrawIndexed(uint32_t indexCount, uint32_t instanceCount,
                     uint32_t firstIndex, uint32_t baseVertex,
                     uint32_t firstInstance) override;
    void SetVertexBuffer(uint32_t slot, Buffer buffer, uint64_t offset,
                         uint64_t size) override;
    void SetIndexBuffer(Buffer buffer, IndexType, uint32_t offset,
                        uint32_t size) override;
    void SetBindGroup(uint32_t index, BindGroup) override;
    void SetPipeline(RenderPipeline) override;

    void End() override;

private:
    CommandBufferImpl* buffer_;
};

}  // namespace nickel::rhi::gl4