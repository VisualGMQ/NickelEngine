#include "graphics/rhi/command.hpp"
#ifdef NICKEL_HAS_VULKAN
#include "graphics/rhi/vk/command.hpp"
#endif
#include "graphics/rhi/gl4/command.hpp"
#include "graphics/rhi/null/command.hpp"

namespace nickel::rhi {

CommandEncoder::CommandEncoder(CommandEncoderImpl* impl) : impl_{impl} {}

CommandBuffer::CommandBuffer(CommandBufferImpl* impl) : impl_{impl} {}

void CommandEncoder::CopyBufferToBuffer(const Buffer& src, uint64_t srcOffset,
                                        const Buffer& dst, uint64_t dstOffset,
                                        uint64_t size) {
    impl_->CopyBufferToBuffer(src, srcOffset, dst, dstOffset, size);
}

void CommandEncoder::CopyBufferToTexture(
    const CommandEncoder::BufTexCopySrc& src,
    const CommandEncoder::BufTexCopyDst& dst, const Extent3D& copySize) {
    impl_->CopyBufferToTexture(src, dst, copySize);
}

RenderPassEncoder CommandEncoder::BeginRenderPass(
    const RenderPass::Descriptor& desc) {
    return impl_->BeginRenderPass(desc);
}

CommandBuffer CommandEncoder::Finish() {
    return impl_->Finish();
}

void CommandEncoder::Destroy() {
    if (impl_) {
        delete impl_;
        impl_ = nullptr;
    }
}

void RenderPassEncoder::Draw(uint32_t vertexCount, uint32_t instanceCount,
                             uint32_t firstVertex, uint32_t firstInstance) {
    impl_->Draw(vertexCount, instanceCount, firstVertex, firstInstance);
}

void RenderPassEncoder::DrawIndexed(uint32_t indexCount, uint32_t instanceCount,
                                    uint32_t firstIndex, uint32_t baseVertex,
                                    uint32_t firstInstance) {
    impl_->DrawIndexed(indexCount, instanceCount, firstIndex, baseVertex,
                       firstInstance);
}

void RenderPassEncoder::SetVertexBuffer(uint32_t slot, Buffer buffer,
                                        uint64_t offset, uint64_t size) {
    impl_->SetVertexBuffer(slot, buffer, offset, size);
}

void RenderPassEncoder::SetIndexBuffer(Buffer buffer, IndexType type,
                                       uint32_t offset, uint32_t size) {
    impl_->SetIndexBuffer(buffer, type, offset, size);
}

void RenderPassEncoder::SetBindGroup(BindGroup group) {
    impl_->SetBindGroup(group);
}

void RenderPassEncoder::SetBindGroup(BindGroup group, const std::vector<uint32_t>& dynamicOffset) {
    impl_->SetBindGroup(group, dynamicOffset);
}

void RenderPassEncoder::SetPipeline(RenderPipeline pipeline) {
    impl_->SetPipeline(pipeline);
}

void RenderPassEncoder::SetPushConstant(ShaderStage stage, void* value, uint32_t offset, uint32_t size) {
    impl_->SetPushConstant(stage, value, offset, size);
}

void RenderPassEncoder::End() {
    impl_->End();
    if (impl_) {
        delete impl_;
        impl_ = nullptr;
    }
}

}  // namespace nickel::rhi