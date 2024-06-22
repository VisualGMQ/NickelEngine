#include "graphics/rhi/sampler.hpp"
#ifdef NICKEL_HAS_VULKAN
#include "graphics/rhi/vk/device.hpp"
#include "graphics/rhi/vk/sampler.hpp"
#endif
#ifdef NICKEL_HAS_GLES3
#include "graphics/rhi/gl/sampler.hpp"
#endif
#include "graphics/rhi/null/sampler.hpp"

namespace nickel::rhi {

Sampler::Sampler(APIPreference api, rhi::DeviceImpl& dev,
                 const Descriptor& desc) {
    switch (api) {
        case APIPreference::Undefine:
            break;
        case APIPreference::GL:
#ifdef NICKEL_HAS_GLES3
            impl_ = new gl::SamplerImpl(desc);
#endif
            break;
        case APIPreference::Vulkan: {
#ifdef NICKEL_HAS_VULKAN
            impl_ = new vulkan::SamplerImpl(
                static_cast<vulkan::DeviceImpl&>(dev), desc);
#endif
            break;
        }
        case APIPreference::Null:
            impl_ = new null::SamplerImpl{};
            break;
    }
}

void Sampler::Destroy() {
    if (impl_) {
        delete impl_;
        impl_ = nullptr;
    }
}

}  // namespace nickel::rhi