#include "graphics/rhi/gl/framebuffer.hpp"
#include "graphics/rhi/gl/glcall.hpp"
#include "graphics/rhi/gl/texture.hpp"
#include "graphics/rhi/gl/texture_view.hpp"

namespace nickel::rhi::gl {

FramebufferImpl::FramebufferImpl(const RenderPass::Descriptor& renderPassDesc) {
    GL_CALL(glGenFramebuffers(1, &id));
    Bind();
    for (int i = 0; i < renderPassDesc.colorAttachments.size(); i++) {
        auto& att = renderPassDesc.colorAttachments[i];
        auto view = static_cast<const TextureViewImpl*>(
            static_cast<const TextureViewImpl*>(att.view.Impl()));
        auto texture = static_cast<const TextureImpl*>(view->Texture().Impl());
#ifdef NICKEL_HAS_GL
        if (texture->Type() == GL_TEXTURE_1D ||
            texture->Type() == GL_TEXTURE_2D ||
            texture->Type() == GL_TEXTURE_3D) {
            GL_CALL(glFramebufferTexture(
                GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, view->id, 0));
        }
#else
        if (texture->Type() == GL_TEXTURE_2D) {
            GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i,
                                    GL_TEXTURE_2D, view->id, 0));
        } else if (texture->Type() == GL_TEXTURE_3D) {
            auto layer = view->Texture().Extent().depthOrArrayLayers;
            for (int j = 0; j < layer; j++) {
                GL_CALL(glFramebufferTextureLayer(
                    GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i + j, view->id, 0, j));
            }
        }
#endif
        attachments_.emplace_back(texture->id);
    }
    if (renderPassDesc.colorAttachments.empty()) {
        GLenum buf = GL_NONE;
        GL_CALL(glDrawBuffers(1, &buf));
        GL_CALL(glReadBuffer(GL_NONE));
    }
    if (renderPassDesc.depthStencilAttachment) {
        auto view = static_cast<const TextureViewImpl*>(
            static_cast<const TextureViewImpl*>(
                renderPassDesc.depthStencilAttachment->view.Impl()));
        auto fmt = view->Format();
        GLenum target = GL_DEPTH_STENCIL_ATTACHMENT;
        if (fmt == TextureFormat::DEPTH24_PLUS_STENCIL8 ||
            fmt == TextureFormat::DEPTH32_FLOAT_STENCIL8) {
            target = GL_DEPTH_STENCIL_ATTACHMENT;
        } else if (fmt == TextureFormat::DEPTH32_FLOAT ||
                   fmt == TextureFormat::DEPTH16_UNORM ||
                   fmt == TextureFormat::DEPTH24_PLUS) {
            target = GL_DEPTH_ATTACHMENT;
        } else {
            target = GL_STENCIL_ATTACHMENT;
        }
#ifdef NICKEL_HAS_GL
        GL_CALL(glFramebufferTexture(GL_FRAMEBUFFER, target, view->id, 0));
#else
        GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, target, GL_TEXTURE_2D,
                                       view->id, 0));
#endif
        attachments_.emplace_back(view->id);
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

}  // namespace nickel::rhi::gl