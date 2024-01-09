#include "rhi/gl/framebuffer.hpp"
#include "rhi/gl/enum_convert.hpp"
#include "rhi/gl/glcall.hpp"


namespace nickel::rhi::gl {

GLenum getTextureTarget(int& idx, ImageUsageFlags usage) {
    if (static_cast<uint32_t>(ImageUsageFlagBits::ColorAttachment) & usage) {
        return GL_COLOR_ATTACHMENT0 + (idx++);
    } else if (static_cast<uint32_t>(
                   ImageUsageFlagBits::DepthStencilAttachment) &
               usage) {
        return GL_DEPTH_STENCIL_ATTACHMENT;
    }
    return {};
}

Framebuffer::Framebuffer(const std::vector<ImageView>& imageViews) {
    GL_CALL(glGenFramebuffers(1, &id_));
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, id_));
    int colorIdx = 0;
    for (auto& view : imageViews) {
        auto target = getTextureTarget(colorIdx, view.GetImage().Usage());
        switch (view.ViewType()) {
            case ImageViewType::e1D:
                GL_CALL(glFramebufferTexture1D(
                    GL_FRAMEBUFFER, target, ImageViewType2GL(view.ViewType()),
                    view.GetImage().Raw(), 0));
            case ImageViewType::e2D:
                GL_CALL(glFramebufferTexture2D(
                    GL_FRAMEBUFFER, target, ImageViewType2GL(view.ViewType()),
                    view.GetImage().Raw(), 0));
            case ImageViewType::e3D:
                GL_CALL(glFramebufferTexture3D(
                    GL_FRAMEBUFFER, target, ImageViewType2GL(view.ViewType()),
                    view.GetImage().Raw(), 0, 0));
            case ImageViewType::Cube:
            case ImageViewType::e1DArray:
            case ImageViewType::e2DArray:
            case ImageViewType::CubeArray:
                // currently I don't known how to handle these
                Assert(false, "unsupport texture type in opengl");
        }
    }
}

Framebuffer::~Framebuffer() {
    GL_CALL(glDeleteFramebuffers(1, &id_));
}

}  // namespace nickel::rhi::gl