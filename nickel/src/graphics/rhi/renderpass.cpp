#include "graphics/rhi/renderpass.hpp"
#ifdef NICKEL_HAS_VULKAN
#include "graphics/rhi/vk/device.hpp"
#include "graphics/rhi/vk/renderpass.hpp"
#endif
#include "graphics/rhi/gl4/renderpass.hpp"
#include "graphics/rhi/null/renderpass.hpp"


namespace nickel::rhi {

RenderPass::RenderPass(APIPreference api, DeviceImpl& dev,
                       const Descriptor& desc) {
    switch (api) {
        case APIPreference::Undefine:
            break;
        case APIPreference::GL:
            impl_ = new gl4::RenderPassImpl(desc);
            break;
        case APIPreference::Vulkan:
#ifdef NICKEL_HAS_VULKAN
            impl_ = new vulkan::RenderPassImpl(
                static_cast<vulkan::DeviceImpl&>(dev), desc);
#endif
            break;
        case APIPreference::Null:
            impl_ = new null::RenderPassImpl{};
            break;
    }
}

const RenderPass::Descriptor& RenderPass::GetDescriptor() const {
    return impl_->GetDescriptor();
}

void RenderPass::Destroy() {
    if (impl_) {
        delete impl_;
        impl_ = nullptr;
    }
}

}  // namespace nickel::rhi