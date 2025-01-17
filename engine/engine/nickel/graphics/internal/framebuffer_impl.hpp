#pragma once
#include "nickel/graphics/framebuffer.hpp"
#include "nickel/graphics/internal/refcountable.hpp"

namespace nickel::graphics {

class DeviceImpl;
class RenderPass;

class FramebufferImpl : public RefCountable {
public:
    FramebufferImpl(DeviceImpl& dev, const Framebuffer::Descriptor&);
    FramebufferImpl(const FramebufferImpl&) = delete;
    FramebufferImpl(FramebufferImpl&&) = delete;
    FramebufferImpl& operator=(const FramebufferImpl&) = delete;
    FramebufferImpl& operator=(FramebufferImpl&&) = delete;

    ~FramebufferImpl();

    void DecRefcount() override;

    VkFramebuffer m_fbo = VK_NULL_HANDLE;
    std::vector<ImageView> m_views;
    
private:
    DeviceImpl& m_device;
};
}  // namespace nickel::graphics