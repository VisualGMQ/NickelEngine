#pragma once
#include "nickel/graphics/render_pass.hpp"
#include "nickel/graphics/internal/refcountable.hpp"

namespace nickel::graphics {

class DeviceImpl;

class RenderPassImpl : public RefCountable {
public:
    RenderPassImpl(DeviceImpl&, const RenderPass::Descriptor&);
    ~RenderPassImpl();

    VkRenderPass m_render_pass;

private:
    DeviceImpl& m_dev;
};

}