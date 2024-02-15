#include "graphics/rhi/shader.hpp"
#include "graphics/rhi/adapter.hpp"
#include "graphics/rhi/impl/device.hpp"
#include "graphics/rhi/impl/shader.hpp"
#include "graphics/rhi/vk/device.hpp"
#include "graphics/rhi/vk/shader.hpp"
#include "graphics/rhi/null/shader.hpp"


namespace nickel::rhi {

ShaderModule::ShaderModule(APIPreference api, DeviceImpl& device,
                           const Descriptor& desc) {
    switch (api) {
        case APIPreference::Undefine:
            break;
        case APIPreference::GL:
            break;
        case APIPreference::Vulkan:
            impl_ = new vulkan::ShaderModuleImpl(
                static_cast<vulkan::DeviceImpl&>(device).device, desc.code);
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