#include "graphics/rhi/framebuffer.hpp"
#ifdef NICKEL_HAS_VULKAN
#include "graphics/rhi/vk/framebuffer.hpp"
#endif
#ifdef NICKEL_HAS_GLES3
#include "graphics/rhi/gl/framebuffer.hpp"
#endif

namespace nickel::rhi {

Framebuffer::Framebuffer(FramebufferImpl* impl) : impl_{impl} {}

void Framebuffer::Destroy() {
    if (impl_) {
        delete impl_;
        impl_ = nullptr;
    }
}

}