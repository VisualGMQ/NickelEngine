#include "graphics/rhi/gl4/framebuffer.hpp"
#include "graphics/rhi/gl4/glcall.hpp"
#include "graphics/rhi/gl4/texture.hpp"
#include "graphics/rhi/gl4/texture_view.hpp"

namespace nickel::rhi::gl4 {

FramebufferImpl::FramebufferImpl(const Framebuffer::Descriptor& desc,
                                 const RenderPass::Descriptor& renderPassDesc) {
    GL_CALL(glGenFramebuffers(1, &id));
    Bind();
    for (int i = 0; i < renderPassDesc.colorAttachments.size(); i++) {
        auto& att = renderPassDesc.colorAttachments[i];
        auto texture = static_cast<const TextureImpl*>(
            static_cast<const TextureViewImpl*>(att.view.Impl())->Texture().Impl());
        if (texture->Type() == GL_TEXTURE_1D ||
            texture->Type() == GL_TEXTURE_2D ||
            texture->Type() == GL_TEXTURE_3D) {
            GL_CALL(glFramebufferTexture(
                GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, texture->id, 0));
        }
        attachments.emplace_back(texture->id);
    }
    if (renderPassDesc.depthStencilAttachment) {
        auto texture = static_cast<const TextureImpl*>(
            static_cast<const TextureViewImpl*>(
                renderPassDesc.depthStencilAttachment->view.Impl())
                ->Texture()
                .Impl());
        auto fmt = texture->Format();
        if (fmt == TextureFormat::DEPTH24_PLUS_STENCIL8 ||
            fmt == TextureFormat::DEPTH32_FLOAT_STENCIL8) {
            GL_CALL(glFramebufferTexture(
                GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, texture->id, 0));
        } else if (fmt == TextureFormat::DEPTH32_FLOAT ||
                   fmt == TextureFormat::DEPTH16_UNORM ||
                   fmt == TextureFormat::DEPTH24_PLUS) {
            GL_CALL(glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                                         texture->id, 0));
        } else {
            GL_CALL(glFramebufferTexture(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT,
                                         texture->id, 0));
        }
        attachments.emplace_back(texture->id);
    }

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        LOGE(log_tag::GL, "framebuffer in complete!");
    }

    Unbind();
}

FramebufferImpl::~FramebufferImpl() {
    GL_CALL(glDeleteFramebuffers(1, &id));
}

void FramebufferImpl::Bind() const {
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, id));
}

void FramebufferImpl::Unbind() const {
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

}  // namespace nickel::rhi::gl4