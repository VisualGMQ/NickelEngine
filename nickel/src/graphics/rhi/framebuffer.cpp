#include "graphics/rhi/framebuffer.hpp"
#ifdef NICKEL_HAS_VULKAN
#include "graphics/rhi/vk/framebuffer.hpp"
#endif
#include "graphics/rhi/gl4/framebuffer.hpp"

namespace nickel::rhi {

Framebuffer::Framebuffer(FramebufferImpl* impl) : impl_{impl} {}

void Framebuffer::Destroy() {
    if (impl_) {
        delete impl_;
        impl_ = nullptr;
    }
}

}