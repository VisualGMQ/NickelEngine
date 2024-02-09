#include "graphics/rhi/framebuffer.hpp"
#include "graphics/rhi/vk/framebuffer.hpp"

namespace nickel::rhi {

Framebuffer::Framebuffer(FramebufferImpl* impl) : impl_{impl} {}

void Framebuffer::Destroy() {
    if (impl_) {
        delete impl_;
        impl_ = nullptr;
    }
}

}