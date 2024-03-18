#pragma once

#include "graphics/rhi/renderpass.hpp"
#include "graphics/rhi/texture_view.hpp"
#include <vector>


namespace nickel::rhi {

class FramebufferImpl;

class Framebuffer final {
public:
    Framebuffer() = default;
    Framebuffer(FramebufferImpl*);
    void Destroy();

    operator bool() const { return impl_; }

    auto Impl() const { return impl_; }
    auto Impl() { return impl_; }

private:
    FramebufferImpl* impl_{};
};

}  // namespace nickel::rhi