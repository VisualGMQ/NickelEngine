#include "graphics/rhi/shader.hpp"
#include "graphics/rhi/adapter.hpp"
#include "graphics/rhi/impl/device.hpp"
#include "graphics/rhi/impl/shader.hpp"
#ifdef NICKEL_HAS_VULKAN
#include "graphics/rhi/vk/device.hpp"
#include "graphics/rhi/vk/shader.hpp"
#endif
#ifdef NICKEL_HAS_GLES3
#include "graphics/rhi/gl/shader.hpp"
#endif
#include "graphics/rhi/null/shader.hpp"

namespace nickel::rhi {

ShaderModule::ShaderModule(APIPreference api, DeviceImpl& device,
                           const Descriptor& desc) {
    switch (api) {
        case APIPreference::Undefine:
            break;
        case APIPreference::GL:
        #ifdef NICKEL_HAS_GLES3
            impl_ = std::shared_ptr<ShaderModuleImpl>(new gl::ShaderModuleImpl(desc));
#endif
            break;
        case APIPreference::Vulkan:
#ifdef NICKEL_HAS_VULKAN
            impl_ =
                std::shared_ptr<ShaderModuleImpl>(new vulkan::ShaderModuleImpl(
                    static_cast<vulkan::DeviceImpl&>(device).device,
                    desc.code));
#endif
            break;
        case APIPreference::Null:
            impl_ =
                std::shared_ptr<ShaderModuleImpl>(new null::ShaderModuleImpl{});
            break;
    }
}

void ShaderModule::Destroy() {
    impl_.reset();
}

}  // namespace nickel::rhi