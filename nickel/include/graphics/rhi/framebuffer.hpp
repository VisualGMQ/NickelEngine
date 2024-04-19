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

    Framebuffer(Framebuffer&& o) noexcept { swap(o, *this); }
    Framebuffer(const Framebuffer& o) = default;
    Framebuffer& operator=(const Framebuffer& o) = default;

    Framebuffer& operator=(Framebuffer&& o) noexcept {
        if (&o != this) swap(o, *this);
        return *this;
    }

    void Destroy();

    operator bool() const { return impl_; }

    auto Impl() const { return impl_; }

    auto Impl() { return impl_; }

private:
    FramebufferImpl* impl_{};

    friend void swap(Framebuffer& o1, Framebuffer& o2) noexcept {
        using std::swap;

        swap(o1.impl_, o2.impl_);
    }
};

}  // namespace nickel::rhi