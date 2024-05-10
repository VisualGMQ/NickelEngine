#include "graphics/rhi/gl4/command.hpp"
#include "graphics/rhi/buffer.hpp"
#include "graphics/rhi/command.hpp"
#include "graphics/rhi/gl4/bind_group.hpp"
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
    CmdExecutor(DeviceImpl& dev, const CommandBufferImpl& buffer)
        : buffer_{buffer}, device_{dev} {}

    void operator()(const CmdCopyBuf2Buf& cmd) const {
        auto src = static_cast<const BufferImpl*>(cmd.src.Impl());
        auto dst = static_cast<const BufferImpl*>(cmd.dst.Impl());
        GL_CALL(glBindBuffer(GL_COPY_READ_BUFFER, src->id));
        GL_CALL(glBindBuffer(GL_COPY_WRITE_BUFFER, dst->id));
        GL_CALL(glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER,
                                    cmd.srcOffset, cmd.dstOffset, cmd.size));
    }

    void operator()(const CmdCopyBuf2Texture& cmd) const {
        auto texture = static_cast<const TextureImpl*>(cmd.dst.texture.Impl());
        texture->Bind();
        GL_CALL(glBindBuffer(
            GL_PIXEL_UNPACK_BUFFER,
            static_cast<const BufferImpl*>(cmd.src.buffer.Impl())->id));
#ifdef NICKEL_HAS_GL4
        if (texture->Type() == GL_TEXTURE_1D) {
            GL_CALL(glTexSubImage1D(texture->Type(), cmd.dst.miplevel,
                                    cmd.dst.origin.x, cmd.copySize.width,
                                    TextureFormat2GL(texture->Format()),
                                    TextureFormat2GLDataType(texture->Format()),
                                    (void*)cmd.src.offset));
        } else if (texture->Type() == GL_TEXTURE_1D_ARRAY ||
                   texture->Type() == GL_TEXTURE_2D) {
            GL_CALL(glTexSubImage2D(texture->Type(), cmd.dst.miplevel,
                                    cmd.dst.origin.x, cmd.dst.origin.y,
                                    cmd.copySize.width, cmd.copySize.height,
                                    TextureFormat2GL(texture->Format()),
                                    TextureFormat2GLDataType(texture->Format()),
                                    (void*)cmd.src.offset));
        } else if (texture->Type() == GL_TEXTURE_3D ||
                   texture->Type() == GL_TEXTURE_2D_ARRAY) {
            GL_CALL(glTexSubImage3D(
                texture->Type(), cmd.dst.miplevel, cmd.dst.origin.x,
                cmd.dst.origin.y, cmd.dst.origin.z, cmd.copySize.width,
                cmd.copySize.height, cmd.copySize.depthOrArrayLayers,
                TextureFormat2GL(texture->Format()),
                TextureFormat2GLDataType(texture->Format()),
                (void*)cmd.src.offset));
        } else {
            LOGE(log_tag::GL, "unsupport texture type");
        }
#else
        if (texture->Type() == GL_TEXTURE_2D) {
            GL_CALL(glTexSubImage2D(texture->Type(), cmd.dst.miplevel,
                                    cmd.dst.origin.x, cmd.dst.origin.y,
                                    cmd.copySize.width, cmd.copySize.height,
                                    TextureFormat2GL(texture->Format()),
                                    TextureFormat2GLDataType(texture->Format()),
                                    (void*)cmd.src.offset));
        } else if (texture->Type() == GL_TEXTURE_3D ||
                   texture->Type() == GL_TEXTURE_2D_ARRAY) {
            GL_CALL(glTexSubImage3D(
                texture->Type(), cmd.dst.miplevel, cmd.dst.origin.x,
                cmd.dst.origin.y, cmd.dst.origin.z, cmd.copySize.width,
                cmd.copySize.height, cmd.copySize.depthOrArrayLayers,
                TextureFormat2GL(texture->Format()),
                TextureFormat2GLDataType(texture->Format()),
                (void*)cmd.src.offset));
        } else {
            LOGE(log_tag::GL, "unsupport texture type");
        }

#endif

        GL_CALL(glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0));
    }

    void operator()(const CmdDraw& cmd) const {
        if (!renderPipeline_) {
            LOGW(log_tag::GL, "Do you forget bind pipeline before drawcall?");
        }

        setVertexBuffer2VAO();

#ifdef NICKEL_HAS_GL4
        GL_CALL(glDrawArraysInstancedBaseInstance(
            Topology2GL(renderPipeline_->Descriptor().primitive.topology),
            cmd.firstVertex, cmd.vertexCount, cmd.instanceCount,
            cmd.firstInstance));
#else
        GL_CALL(glDrawArraysInstanced(Topology2GL(
                    renderPipeline_->Descriptor().primitive.topology),
                cmd.firstVertex, cmd.vertexCount, cmd.instanceCount));
#endif
    }

    void operator()(const CmdDrawIndexed& cmd) const {
        if (!renderPipeline_) {
            LOGW(log_tag::GL, "Do you forget bind pipeline before drawcall?");
        }

        setVertexBuffer2VAO();

        auto& desc = renderPipeline_->Descriptor();

#ifdef NICKEL_HAS_GL4
        GL_CALL(glDrawElementsInstancedBaseVertexBaseInstance(
            Topology2GL(desc.primitive.topology), cmd.indexCount,
            desc.primitive.stripIndexFormat == StripIndexFormat::Uint16
                ? GL_UNSIGNED_SHORT
                : GL_UNSIGNED_INT,
            (void*)((desc.primitive.stripIndexFormat == StripIndexFormat::Uint16
                         ? 2
                         : 4) *
                        cmd.firstIndex +
                    setIndexBufCmd_.offset),
            cmd.instanceCount, cmd.baseVertex, cmd.firstInstance));
#else
        GL_CALL(glDrawElementsInstanced(
            Topology2GL(desc.primitive.topology), cmd.indexCount,
            desc.primitive.stripIndexFormat == StripIndexFormat::Uint16
                ? GL_UNSIGNED_SHORT
                : GL_UNSIGNED_INT,
            (void*)((desc.primitive.stripIndexFormat == StripIndexFormat::Uint16
                         ? 2
                         : 4) *
                        cmd.firstIndex +
                    setIndexBufCmd_.offset),
            cmd.instanceCount));
#endif
    }

    void operator()(const CmdSetVertexBuffer& cmd) {
        setVertexBufferCmds_.emplace_back(cmd);
    }

    void operator()(const CmdSetBindGroup& cmd) const {
        static_cast<const BindGroupImpl*>(cmd.group.Impl())
            ->Apply(*renderPipeline_, cmd.dynamicOffset);
    }

    void operator()(const CmdSetIndexBuffer& cmd) {
        // first we set index buffer and init vao
        if (auto it = device_.vaos.find((size_t)cmd.buffer.Impl());
            it != device_.vaos.end()) {
            vao_ = it->second;
            GL_CALL(glBindVertexArray(it->second));
        } else {
            GLuint newVao;
            GL_CALL(glGenVertexArrays(1, &newVao));
            GL_CALL(glBindVertexArray(newVao));
            vao_ = device_.vaos.emplace((size_t)cmd.buffer.Impl(), newVao)
                       .first->second;
            GL_CALL(glBindBuffer(
                GL_ELEMENT_ARRAY_BUFFER,
                static_cast<const BufferImpl*>(cmd.buffer.Impl())->id));
            GL_CALL(glBindVertexArray(vao_));
        }

        setIndexBufCmd_ = cmd;
    }

    void operator()(const CmdSetRenderPipeline& cmd) {
        renderPipeline_ =
            static_cast<const RenderPipelineImpl*>(cmd.pipeline.Impl());
        renderPipeline_->Apply();
        vao_ = renderPipeline_->GetDefaultVAO();
    }

    void operator()(const CmdPushConstant& cmd) {
#ifdef NICKEL_HAS_GL4

        GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, device_.pushConstantBuf));
        GL_CALL(glBufferSubData(GL_ARRAY_BUFFER, cmd.offset, cmd.size,
                                cmd.data.data()));
#else
        auto id = renderPipeline_->GetShaderID();
        auto index =
            glGetUniformBlockIndex(id, "PushConstant");
        if (index == GL_INVALID_INDEX) {
            LOGE(log_tag::GL, "push constant uniform buffer not exists");
        }
        GL_CALL(glUniformBlockBinding(id, index, 16));
        GL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, device_.pushConstantBuf));
        GL_CALL(glBindBufferRange(GL_UNIFORM_BUFFER, 16, device_.pushConstantBuf,
                                  cmd.offset, cmd.size));

#endif
    }

    void operator()(const CmdBeginRenderPass& cmd) {
        std::vector<uint32_t> attachments;
        for (auto& colorAtt : cmd.desc.colorAttachments) {
            attachments.emplace_back(
                static_cast<const TextureViewImpl*>(colorAtt.view.Impl())->id);
        }
        if (cmd.desc.depthStencilAttachment) {
            attachments.emplace_back(
                static_cast<const TextureViewImpl*>(
                    cmd.desc.depthStencilAttachment->view.Impl())
                    ->id);
        }

        Framebuffer framebuffer;

        for (auto fbo : device_.framebuffers) {
            if (static_cast<const FramebufferImpl*>(fbo.Impl())
                    ->GetAttachmentIDs() == attachments) {
                framebuffer = fbo;
                break;
            }
        }

        if (!framebuffer) {
            framebuffer = device_.framebuffers.emplace_back(
                new FramebufferImpl(cmd.desc));
        }

        static_cast<FramebufferImpl*>(framebuffer.Impl())->Bind();
        for (int i = 0; i < cmd.desc.colorAttachments.size(); i++) {
            auto attachment = cmd.desc.colorAttachments[i];

            if (attachment.loadOp == AttachmentLoadOp::Clear) {
                GLenum index = GL_COLOR_ATTACHMENT0 + i;
                GL_CALL(glDrawBuffers(1, &index));
                GL_CALL(
                    glClearBufferfv(GL_COLOR, 0, attachment.clearValue.data()));
            }
        }

        if (cmd.desc.depthStencilAttachment) {
            auto attachment = cmd.desc.depthStencilAttachment.value();
            if (attachment.depthLoadOp == AttachmentLoadOp::Clear) {
                GL_CALL(
                    glClearBufferfv(GL_DEPTH, 0, &attachment.depthClearValue));
            }
            if (attachment.stencilLoadOp == AttachmentLoadOp::Clear) {
                int value = attachment.stencilClearValue;
                GL_CALL(glClearBufferiv(GL_STENCIL, 0, &value));
            }
        }
    }

private:
    DeviceImpl& device_;
    const CommandBufferImpl& buffer_;
    const RenderPipelineImpl* renderPipeline_{};
    GLuint vao_;
    CmdSetIndexBuffer setIndexBufCmd_;
    std::vector<CmdSetVertexBuffer> setVertexBufferCmds_;

    void setVertexBuffer2VAO() const {
        for (auto& cmd : setVertexBufferCmds_) {
            auto& vertexState = renderPipeline_->Descriptor().vertex;
            auto buffer = vertexState.buffers.at(cmd.slot);
            GL_CALL(glBindBuffer(
                GL_ARRAY_BUFFER,
                static_cast<const BufferImpl*>(cmd.buffer.Impl())->id));
            for (auto& attr : buffer.attributes) {
                GL_CALL(glVertexAttribPointer(
                    attr.shaderLocation,
                    GetVertexFormatComponentCount(attr.format),
                    GetVertexFormatGLType(attr.format),
                    IsNormalizedVertexFormat(attr.format), buffer.arrayStride,
                    (void*)(attr.offset + cmd.offset)));
                GL_CALL(glEnableVertexAttribArray(attr.shaderLocation));
            }
        }
    }
};

void CommandBufferImpl::Execute(DeviceImpl& device) const {
    CmdExecutor executor{device, *this};
    for (auto& cmd : cmds) {
        std::visit(executor, cmd.cmd);
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
    buffer_.cmds.push_back({
        CmdType::CopyBuf2Buf,
        CmdCopyBuf2Buf{src, srcOffset, dst, dstOffset, size}
    });
}

void CommandEncoderImpl::CopyBufferToTexture(
    const CommandEncoder::BufTexCopySrc& src,
    const CommandEncoder::BufTexCopyDst& dst, const Extent3D& copySize) {
    buffer_.cmds.push_back({
        CmdType::CopyBuf2Texture,
        CmdCopyBuf2Texture{dst.texture.Dimension(), src, dst, copySize}
    });
}

RenderPassEncoder CommandEncoderImpl::BeginRenderPass(
    const RenderPass::Descriptor& desc) {
    CmdBeginRenderPass cmd;
    cmd.desc = desc;
    buffer_.cmds.emplace_back(
        Command{CmdType::BeginRenderPass, std::move(cmd)});

    return RenderPassEncoder{new RenderPassEncoderImpl(*device_, buffer_)};
}

RenderPassEncoderImpl::RenderPassEncoderImpl(DeviceImpl& device,
                                             CommandBufferImpl& buf)
    : device_{device}, buffer_{&buf} {}

void RenderPassEncoderImpl::Draw(uint32_t vertexCount, uint32_t instanceCount,
                                 uint32_t firstVertex, uint32_t firstInstance) {
    buffer_->cmds.push_back({
        CmdType::Draw,
        CmdDraw{vertexCount, instanceCount, firstVertex, firstInstance}
    });
}

void RenderPassEncoderImpl::DrawIndexed(uint32_t indexCount,
                                        uint32_t instanceCount,
                                        uint32_t firstIndex,
                                        uint32_t baseVertex,
                                        uint32_t firstInstance) {
    buffer_->cmds.push_back({
        CmdType::DrawIndexed,
        CmdDrawIndexed{indexCount, instanceCount, firstIndex, baseVertex,
                       firstInstance}
    });
}

void RenderPassEncoderImpl::SetVertexBuffer(uint32_t slot, Buffer buffer,
                                            uint64_t offset, uint64_t size) {
    CmdSetVertexBuffer cmd;
    cmd.slot = slot;
    cmd.buffer = buffer;
    cmd.offset = offset;
    cmd.size = size;
    buffer_->cmds.push_back(Command{CmdType::SetVertexBuffer, std::move(cmd)});
}

void RenderPassEncoderImpl::SetIndexBuffer(Buffer buffer, IndexType type,
                                           uint32_t offset, uint32_t size) {
    CmdSetIndexBuffer cmd;
    cmd.buffer = buffer;
    cmd.offset = offset;
    cmd.size = size;
    cmd.indexType = type;
    buffer_->cmds.push_back({CmdType::SetIndexBuffer, std::move(cmd)});
}

void RenderPassEncoderImpl::SetBindGroup(BindGroup group) {
    CmdSetBindGroup cmd;
    cmd.group = group;
    buffer_->cmds.push_back({CmdType::SetBindGroup, std::move(cmd)});
}

void RenderPassEncoderImpl::SetBindGroup(
    BindGroup group, const std::vector<uint32_t>& dynamicOffset) {
    CmdSetBindGroup cmd;
    cmd.group = group;
    cmd.dynamicOffset = dynamicOffset;
    buffer_->cmds.push_back({CmdType::SetBindGroup, std::move(cmd)});
}

void RenderPassEncoderImpl::SetPipeline(RenderPipeline pipeline) {
    CmdSetRenderPipeline cmd;
    cmd.pipeline = pipeline;
    buffer_->cmds.push_back({CmdType::SetRenderPipeline, cmd});
}

void RenderPassEncoderImpl::SetPushConstant(ShaderStage stage,
                                            const void* value, uint32_t offset,
                                            uint32_t size) {
    CmdPushConstant cmd;
    cmd.stage = stage;
    cmd.offset = offset;
    cmd.size = size;
    cmd.data.resize(size);
    memcpy(cmd.data.data(), value, size);
    auto maxSize = device_.adapter->Limits().maxPushConstantSize;
    if (offset + size >= maxSize) {
        LOGE(log_tag::GL, "push constant out of range: require ", offset + size,
             ", has ", maxSize);
    }

    buffer_->cmds.push_back({CmdType::PushConstant, cmd});
}

void RenderPassEncoderImpl::SetViewport(float x, float y, float width,
                                        float height) {
    GL_CALL(glViewport(x, y, width, height));
    GL_CALL(glScissor(x, y, width, height));
}

void RenderPassEncoderImpl::End() {}

}  // namespace nickel::rhi::gl4