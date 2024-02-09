#include "graphics/rhi/sampler.hpp"
#include "graphics/rhi/vk/device.hpp"
#include "graphics/rhi/vk/sampler.hpp"


namespace nickel::rhi {

Sampler::Sampler(APIPreference api, rhi::DeviceImpl& dev,
                 const Descriptor& desc) {
    switch (api) {
        case APIPreference::Undefine:
            break;
        case APIPreference::GL:
            break;
        case APIPreference::Vulkan: {
            impl_ = new vulkan::SamplerImpl(
                static_cast<vulkan::DeviceImpl&>(dev), desc);
            break;
        }
    }
}

void Sampler::Destroy() {
    delete impl_;
    impl_ = nullptr;
}

}  // namespace nickel::rhi