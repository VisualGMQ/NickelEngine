#include "graphics/rhi/gl4/command.hpp"
#include "graphics/rhi/command.hpp"
#include "graphics/rhi/gl4/buffer.hpp"
#include "graphics/rhi/gl4/convert.hpp"
#include "graphics/rhi/gl4/device.hpp"
#include "graphics/rhi/gl4/framebuffer.hpp"
#include "graphics/rhi/gl4/glcall.hpp"
#include "graphics/rhi/gl4/render_pipeline.hpp"
#include "graphics/rhi/gl4/renderpass.hpp"
#include "graphics/rhi/gl4/texture.hpp"
#include "graphics/rhi/gl4/texture_view.hpp"

namespace nickel::rhi::gl4 {

struct CmdExecutor final {
    CmdExecutor(const CommandBufferImpl& buffer) : buffer_{buffer} {
        // TODO: apply renderpass & framebuffer
    }

    void operator()(const CmdCopyBuf2Buf& cmd) const {
        auto src = static_cast<const BufferImpl*>(cmd.src.Impl());
        auto dst = static_cast<const BufferImpl*>(cmd.dst.Impl());
        GL_CALL(glBindBuffer(GL_COPY_READ_BUFFER, src->id));
        GL_CALL(glBindBuffer(GL_COPY_WRITE_BUFFER, dst->id));
        GL_CALL(glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER,
                                    cmd.srcOffset, cmd.dstOffset, cmd.size));
    }

    void operator()(const CmdCopyBuf2Texture& cmd) const {
        GL_CALL(glBindBuffer(
            GL_PIXEL_UNPACK_BUFFER,
            static_cast<const BufferImpl*>(cmd.src.buffer.Impl())->id));
        auto texture = static_cast<const TextureImpl*>(cmd.dst.texture.Impl());
        texture->Bind();
        GL_CALL(glTexImage2D(texture->Type(), 0,
                             TextureFormat2GL(texture->Format()),
                             texture->Extent().width, texture->Extent().height,
                             0, TextureFormat2GL(texture->Format()),
                             GL_UNSIGNED_BYTE, &cmd.src.offset));
        GL_CALL(glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0));
    }

    void operator()(const CmdDraw& cmd) const {
        if (!buffer_.pipeline) {
            LOGW(log_tag::GL, "Do you forget bind pipeline before drawcall?");
        }

        GL_CALL(glDrawArraysInstancedBaseInstance(
            Topology2GL(
                static_cast<const RenderPipelineImpl*>(buffer_.pipeline.Impl())
                    ->Descriptor()
                    .primitive.topology),
            cmd.firstVertex, cmd.vertexCount, cmd.instanceCount,
            cmd.firstInstance));
    }

    void operator()(const CmdDrawIndexed& cmd) const {
        if (!buffer_.pipeline) {
            LOGW(log_tag::GL, "Do you forget bind pipeline before drawcall?");
        }

        auto& desc =
            static_cast<const RenderPipelineImpl*>(buffer_.pipeline.Impl())
                ->Descriptor();

        GL_CALL(glDrawElementsInstancedBaseVertexBaseInstance(
            Topology2GL(desc.primitive.topology), cmd.indexCount,
            desc.primitive.stripIndexFormat == StripIndexFormat::Uint16
                ? GL_UNSIGNED_SHORT
                : GL_UNSIGNED_INT,
            &cmd.firstIndex, cmd.instanceCount, cmd.baseVertex,
            cmd.firstInstance));
    }

private:
    const CommandBufferImpl& buffer_;
};

void CommandBufferImpl::Execute() {
    CmdExecutor executor{*this};
    for (auto& cmd : cmds) {
        std::visit(executor, cmd);
    }
}

CommandBuffer CommandEncoderImpl::Finish() {
    return CommandBuffer{&buffer_};
}

CommandEncoderImpl::CommandEncoderImpl(DeviceImpl& impl) : device_{&impl} {}

void CommandEncoderImpl::CopyBufferToBuffer(const Buffer& src,
                                            uint64_t srcOffset,
                                            const Buffer& dst,
                                            uint64_t dstOffset, uint64_t size) {
    buffer_.cmds.push_back(
        CmdCopyBuf2Buf{src, srcOffset, dst, dstOffset, size});
}

void CommandEncoderImpl::CopyBufferToTexture(
    const CommandEncoder::BufTexCopySrc& src,
    const CommandEncoder::BufTexCopyDst& dst, const Extent3D& copySize) {
    buffer_.cmds.push_back(CmdCopyBuf2Texture{src, dst, copySize});
}

RenderPassEncoder CommandEncoderImpl::BeginRenderPass(
    const RenderPass::Descriptor& desc) {
    std::vector<uint32_t> attachments;
    for (auto& colorAtt : desc.colorAttachments) {
        attachments.emplace_back(
            static_cast<const TextureViewImpl*>(colorAtt.view.Impl())
                ->texture->id);
    }
    if (desc.depthStencilAttachment) {
        attachments.emplace_back(static_cast<const TextureViewImpl*>(
                                     desc.depthStencilAttachment->view.Impl())
                                     ->texture->id);
    }

    for (auto fbo : device_->framebuffers) {
        if (static_cast<const FramebufferImpl*>(fbo.Impl())
                ->GetAttachmentIDs() == attachments) {
            buffer_.framebuffer = fbo;
        }
    }

    if (!buffer_.framebuffer) {
        Framebuffer::Descriptor fboDesc;
        for (auto att : desc.colorAttachments) {
            fboDesc.views.emplace_back(att.view);
        }
        if (desc.depthStencilAttachment) {
            fboDesc.views.emplace_back(desc.depthStencilAttachment->view);
        }
        buffer_.framebuffer =
            device_->framebuffers.emplace_back(new FramebufferImpl(fboDesc));
        fboDesc.extent = desc.colorAttachments.at(0).view.Texture().Extent();
    }

    buffer_.renderPass = desc;

    return RenderPassEncoder{new RenderPassEncoderImpl(buffer_)};
}

RenderPassEncoderImpl::RenderPassEncoderImpl(CommandBufferImpl& buf)
    : buffer_{&buf} {}

void RenderPassEncoderImpl::Draw(uint32_t vertexCount, uint32_t instanceCount,
                                 uint32_t firstVertex, uint32_t firstInstance) {
    buffer_->cmds.push_back(
        CmdDraw{vertexCount, instanceCount, firstVertex, firstInstance});
}

void RenderPassEncoderImpl::DrawIndexed(uint32_t indexCount,
                                        uint32_t instanceCount,
                                        uint32_t firstIndex,
                                        uint32_t baseVertex,
                                        uint32_t firstInstance) {
    buffer_->cmds.push_back(CmdDrawIndexed{
        indexCount, instanceCount, firstIndex, baseVertex, firstInstance});
}

void RenderPassEncoderImpl::SetVertexBuffer(uint32_t slot, Buffer buffer,
                                            uint64_t offset, uint64_t size) {
    auto buf = static_cast<const BufferImpl*>(buffer.Impl());
    GL_CALL(glBindBufferRange(GL_ARRAY_BUFFER, slot, buf->id, offset, size));
}

void RenderPassEncoderImpl::SetIndexBuffer(Buffer buffer, IndexType,
                                           uint32_t offset, uint32_t size) {
    auto buf = static_cast<const BufferImpl*>(buffer.Impl());
    GL_CALL(
        glBindBufferRange(GL_ELEMENT_ARRAY_BUFFER, 0, buf->id, offset, size));
}

void RenderPassEncoderImpl::SetBindGroup(uint32_t index, BindGroup group) {
    buffer_->bindGroups[index] = group;
}

void RenderPassEncoderImpl::SetPipeline(RenderPipeline pipeline) {
    buffer_->pipeline = pipeline;
}

void RenderPassEncoderImpl::End() {}

}  // namespace nickel::rhi::gl4