#pragma once
#include "nickel/common/math/math.hpp"
#include "render_pass.hpp"

namespace nickel::graphics {
class ImageView;

class FramebufferImpl;

class Framebuffer {
public:
    struct Descriptor {
        std::vector<ImageView> m_views;
        Vec3 m_extent;
        RenderPass m_render_pass;
    };

    explicit Framebuffer(FramebufferImpl*);
    Framebuffer(const Framebuffer&);
    Framebuffer(Framebuffer&&) noexcept;
    Framebuffer& operator=(const Framebuffer&) noexcept;
    Framebuffer& operator=(Framebuffer&&) noexcept;
    ~Framebuffer();

    const FramebufferImpl& Impl() const noexcept;
    FramebufferImpl& Impl() noexcept;

private:
    FramebufferImpl* m_impl;
};

}  // namespace nickel::graphics