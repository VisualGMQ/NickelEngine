#include "graphics/rhi/shader.hpp"
#include "graphics/rhi/adapter.hpp"
#include "graphics/rhi/impl/device.hpp"
#include "graphics/rhi/impl/shader.hpp"
#ifdef NICKEL_HAS_VULKAN
#include "graphics/rhi/vk/device.hpp"
#include "graphics/rhi/vk/shader.hpp"
#endif
#include "graphics/rhi/gl4/shader.hpp"
#include "graphics/rhi/null/shader.hpp"

namespace nickel::rhi {

ShaderModule::ShaderModule(APIPreference api, DeviceImpl& device,
                           const Descriptor& desc) {
    switch (api) {
        case APIPreference::Undefine:
            break;
        case APIPreference::GL:
            impl_ = new gl4::ShaderModuleImpl(desc);
            break;
        case APIPreference::Vulkan:
#ifdef NICKEL_HAS_VULKAN
            impl_ = new vulkan::ShaderModuleImpl(
                static_cast<vulkan::DeviceImpl&>(device).device, desc.code);
#endif
            break;
        case APIPreference::Null:
            impl_ = new null::ShaderModuleImpl{};
            break;
    }
}

void ShaderModule::Destroy(DeviceImpl& dev) {
    if (impl_) {
        delete impl_;
        impl_ = nullptr;
    }
}

}  // namespace nickel::rhi