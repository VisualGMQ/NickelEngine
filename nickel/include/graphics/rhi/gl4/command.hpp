#pragma once

#include "graphics/rhi/impl/command.hpp"
#include "graphics/rhi/gl4/glpch.hpp"

namespace nickel::rhi::gl4 {

class DeviceImpl;
class CommandEncoderImpl;

struct CmdCopyBuf2Texture final {
    CommandEncoder::BufTexCopySrc src;
    CommandEncoder::BufTexCopyDst dst;
    Extent3D copySize;
};

struct CmdCopyBuf2Buf final {
    Buffer src;
    uint64_t srcOffset;
    Buffer dst;
    uint64_t dstOffset;
    uint64_t size;
};

struct CmdDraw final {
    uint32_t vertexCount;
    uint32_t instanceCount;
    uint32_t firstVertex;
    uint32_t firstInstance;
};

struct CmdDrawIndexed final {
    uint32_t indexCount;
    uint32_t instanceCount;
    uint32_t firstIndex;
    uint32_t baseVertex;
    uint32_t firstInstance;
};

struct CmdSetVertexBuffer final {
    uint32_t slot;
    Buffer buffer;
    uint64_t offset;
    uint64_t size;
};

struct CmdSetBindGroup final {
    BindGroup group;
};


using Command =
    std::variant<CmdCopyBuf2Buf, CmdCopyBuf2Texture, CmdDraw, CmdDrawIndexed,
                 CmdSetVertexBuffer, CmdSetBindGroup>;

class CommandBufferImpl : public rhi::CommandBufferImpl {
public:
    std::vector<Command> cmds;
    RenderPipeline pipeline;
    std::unordered_map<uint32_t, BindGroup> bindGroups;
    std::optional<RenderPass::Descriptor> renderPass;
    Framebuffer framebuffer;
    Buffer indicesBuffer;

    void Execute(DeviceImpl&) const;
};

class CommandEncoderImpl : public rhi::CommandEncoderImpl {
public:
    explicit CommandEncoderImpl(DeviceImpl&);

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
    DeviceImpl* device_;
};

class RenderPassEncoderImpl : public rhi::RenderPassEncoderImpl {
public:
    explicit RenderPassEncoderImpl(DeviceImpl& device, CommandBufferImpl&);

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
    void SetPipeline(RenderPipeline) override;

    void End() override;

private:
    DeviceImpl& device_;
    CommandBufferImpl* buffer_;
};

}  // namespace nickel::rhi::gl4