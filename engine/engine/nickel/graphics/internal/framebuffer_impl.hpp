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
    std::vector<ImageView> m_views;
    
private:
    DeviceImpl& m_device;
};
}  // namespace nickel::graphics