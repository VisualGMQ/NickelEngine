#pragma once
#include "nickel/graphics/framebuffer.hpp"
#include "nickel/graphics/internal/refcountable.hpp"

namespace nickel::graphics {

class DeviceImpl;
class RenderPass;

class FramebufferImpl : public RefCountable {
public:
    FramebufferImpl(DeviceImpl& dev, const Framebuffer::Descriptor&);

    ~FramebufferImpl();

    VkFramebuffer m_fbo;

private:
    DeviceImpl& m_device;
    std::vector<ImageView> m_views;
};
}  // namespace nickel::graphics