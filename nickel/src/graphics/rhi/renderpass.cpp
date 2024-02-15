#include "graphics/rhi/renderpass.hpp"
#include "graphics/rhi/vk/device.hpp"
#include "graphics/rhi/vk/renderpass.hpp"
#include "graphics/rhi/null/renderpass.hpp"


namespace nickel::rhi {

RenderPass::RenderPass(APIPreference api, DeviceImpl& dev,
                       const Descriptor& desc)
    : desc_{desc} {
    switch (api) {
        case APIPreference::Undefine:
            break;
        case APIPreference::GL:
            break;
        case APIPreference::Vulkan:
            impl_ = new vulkan::RenderPassImpl(
                static_cast<vulkan::DeviceImpl&>(dev), desc);
            break;
        case APIPreference::Null:
            impl_ = new null::RenderPassImpl{};
            break;
    }
}

void RenderPass::Destroy() {
    if (impl_) {
        delete impl_;
        impl_ = nullptr;
    }
}

}  // namespace nickel::rhi