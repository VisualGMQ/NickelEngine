#include "graphics/rhi/null/command.hpp"

namespace nickel::rhi::null {

void RenderPassEncoderImpl::Draw(uint32_t vertexCount, uint32_t instanceCount,
                                 uint32_t firstVertex, uint32_t firstInstance) {
}

void RenderPassEncoderImpl::DrawIndexed(uint32_t indexCount,
                                        uint32_t instanceCount,
                                        uint32_t firstIndex,
                                        uint32_t baseVertex,
                                        uint32_t firstInstance) {}

void RenderPassEncoderImpl::SetVertexBuffer(uint32_t slot, Buffer buffer,
                                            uint64_t offset, uint64_t size) {}

void RenderPassEncoderImpl::SetIndexBuffer(Buffer buffer, IndexType,
                                           uint32_t offset, uint32_t size) {}

void RenderPassEncoderImpl::SetBindGroup(BindGroup) {}

void RenderPassEncoderImpl::SetPipeline(RenderPipeline) {}

void RenderPassEncoderImpl::End() {}

CommandBuffer CommandEncoderImpl::Finish() {
    return CommandBuffer{new CommandBufferImpl{}};
}

void CommandEncoderImpl::CopyBufferToBuffer(const Buffer& src,
                                            uint64_t srcOffset,
                                            const Buffer& dst,
                                            uint64_t dstOffset, uint64_t size) {
}

void CommandEncoderImpl::CopyBufferToTexture(
    const CommandEncoder::BufTexCopySrc& src,
    const CommandEncoder::BufTexCopyDst& dst, const Extent3D& copySize) {}

RenderPassEncoder CommandEncoderImpl::BeginRenderPass(
    const RenderPass::Descriptor&) {
    return RenderPassEncoder{new RenderPassEncoderImpl{}};
}

}  // namespace nickel::rhi::null