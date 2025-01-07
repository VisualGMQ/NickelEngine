#pragma once
#include "nickel/common/math/math.hpp"
#include "nickel/graphics/render_pass.hpp"
#include "nickel/common/dllexport.hpp"

namespace nickel::graphics {
class ImageView;

class FramebufferImpl;

class NICKEL_API Framebuffer {
public:
    struct Descriptor {
        std::vector<ImageView> m_views;
        Vec3 m_extent;
        RenderPass m_render_pass;
    };

    Framebuffer() = default;
    explicit Framebuffer(FramebufferImpl*);
    Framebuffer(const Framebuffer&);
    Framebuffer(Framebuffer&&) noexcept;
    Framebuffer& operator=(const Framebuffer&) noexcept;
    Framebuffer& operator=(Framebuffer&&) noexcept;
    ~Framebuffer();

    const FramebufferImpl& Impl() const noexcept;
    FramebufferImpl& Impl() noexcept;
    
    operator bool() const noexcept;
    void Release();

private:
    FramebufferImpl* m_impl{};
};

}  // namespace nickel::graphics