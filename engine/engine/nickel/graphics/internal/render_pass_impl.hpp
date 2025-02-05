#pragma once
#include "nickel/graphics/render_pass.hpp"
#include "nickel/common/memory/refcountable.hpp"

namespace nickel::graphics {

class DeviceImpl;

class RenderPassImpl : public RefCountable {
public:
    RenderPassImpl(DeviceImpl&, const RenderPass::Descriptor&);
    RenderPassImpl(const RenderPassImpl&) = delete;
    RenderPassImpl(RenderPassImpl&&) = delete;
    RenderPassImpl& operator=(const RenderPassImpl&) = delete;
    RenderPassImpl& operator=(RenderPassImpl&&) = delete;

    ~RenderPassImpl();

    void DecRefcount() override;

    VkRenderPass m_render_pass = VK_NULL_HANDLE;

private:
    DeviceImpl& m_dev;
};

}