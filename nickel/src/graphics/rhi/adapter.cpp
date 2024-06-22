#include "graphics/rhi/adapter.hpp"
#include "common/assert.hpp"
#ifdef NICKEL_HAS_VULKAN
#include "graphics/rhi/vk/adapter.hpp"
#endif
#ifdef NICKEL_HAS_GLES3
#include "graphics/rhi/gl/adapter.hpp"
#endif
#include "graphics/rhi/null/adapter.hpp"

namespace nickel::rhi {

APIPreference GetSupportRenderAPI(APIPreference desired) {
    APIPreference api = desired;

    if (api == APIPreference::Undefine) {
#ifdef NICKEL_HAS_VULKAN
        api = APIPreference::Vulkan;
#elif defined(NICKEL_HAS_GLES3)
        api = APIPreference::GL;
#endif
    }

    if (api == APIPreference::Vulkan) {
#ifndef NICKEL_HAS_VULKAN
        api = APIPreference::GL;
#endif
    }

    return api;
}

Adapter::Adapter(void* window, Option option) {
    auto api = GetSupportRenderAPI(option.api);

    switch (api) {
        case APIPreference::Undefine:
            Assert(false, "undefined render api");
            break;
        case APIPreference::GL:
#ifdef NICKEL_HAS_GLES3
            impl_ = new gl::AdapterImpl((SDL_Window*)window);
#endif
            break;
        case APIPreference::Vulkan:
#ifdef NICKEL_HAS_VULKAN
            impl_ = new vulkan::AdapterImpl(window);
            break;
#endif
        case APIPreference::Null:
            impl_ = new null::AdapterImpl{};
            break;
    }
}

GPUSupportFeatures Adapter::Features() {
    return impl_->Features();
}

const GPUSupportLimits& Adapter::Limits() const {
    return impl_->Limits();
}

Device Adapter::RequestDevice() {
    return impl_->RequestDevice();
}

Adapter::Info Adapter::RequestAdapterInfo() const {
    return impl_->RequestAdapterInfo();
}

void Adapter::Destroy() {
    if (impl_) {
        delete impl_;
        impl_ = nullptr;
    }
}

}  // namespace nickel::rhi