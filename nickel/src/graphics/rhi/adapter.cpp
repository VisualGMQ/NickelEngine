#include "graphics/rhi/adapter.hpp"
#include "common/assert.hpp"
#include "graphics/rhi/vk/adapter.hpp"

namespace nickel::rhi {

Adapter::Adapter(void* window, Option option) {
    APIPreference api = option.api;
    if (api == APIPreference::Undefine) {
#ifdef NICKEL_HAS_VULKAN
        api = APIPreference::Vulkan;
#else
        api = APIPreference::GL;
#endif
    }

    switch (api) {
        case APIPreference::Undefine:
            Assert(false, "undefined render api");
            break;
        case APIPreference::GL:
            // TODO: create GL Adapter
            break;
        case APIPreference::Vulkan:
            impl_ = new vulkan::AdapterImpl(window);
            break;
    }
}

GPUSupportFeatures Adapter::Features() {
    return impl_->Features();
}

GPUSupportLimits Adapter::Limits() {
    return impl_->Limits();
}

Device Adapter::RequestDevice() {
    return impl_->RequestDevice();
}

Adapter::Info Adapter::RequestAdapterInfo() {
    return impl_->RequestAdapterInfo();
}

void Adapter::Destroy() {
    delete impl_;
    impl_ = nullptr;
}

}  // namespace nickel::rhi