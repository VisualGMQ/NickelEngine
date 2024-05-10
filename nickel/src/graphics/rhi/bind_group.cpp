#include "graphics/rhi/bind_group.hpp"
#include "graphics/rhi/null/bind_group.hpp"
#ifdef NICKEL_HAS_VULKAN
#include "graphics/rhi/vk/bind_group.hpp"
#include "graphics/rhi/vk/device.hpp"
#endif
#include "graphics/rhi/gl4/device.hpp"
#include "graphics/rhi/null/device.hpp"


namespace nickel::rhi {

BindGroupLayout::BindGroupLayout(APIPreference api, DeviceImpl& dev,
                                 const Descriptor& desc) {
    switch (api) {
        case APIPreference::Undefine:
            break;
        case APIPreference::GL:
            impl_ = new gl4::BindGroupLayoutImpl(desc);
            break;
        case APIPreference::Vulkan:
#ifdef NICKEL_HAS_VULKAN
            impl_ = new vulkan::BindGroupLayoutImpl(
                static_cast<vulkan::DeviceImpl&>(dev), desc);
#endif
            break;
        case APIPreference::Null:
            impl_ = new null::BindGroupLayoutImpl{};
            break;
    }
}

void BindGroupLayout::Destroy() {
    if (impl_) {
        delete impl_;
        impl_ = nullptr;
    }
}

BindGroup::BindGroup(APIPreference api, DeviceImpl& dev,
                     const Descriptor& desc) {
    switch (api) {
        case APIPreference::Undefine:
            break;
        case APIPreference::GL:
            impl_ = new gl4::BindGroupImpl(desc);
            break;
        case APIPreference::Vulkan:
#ifdef NICKEL_HAS_VULKAN
            impl_ = new vulkan::BindGroupImpl(
                static_cast<vulkan::DeviceImpl&>(dev), desc);
#endif
            break;
        case APIPreference::Null:
            impl_ = new null::BindGroupImpl{};
            break;
    }
}

BindGroupLayout BindGroup::GetLayout() const {
    return impl_->GetLayout();
}

void BindGroup::Destroy() {
    if (impl_) {
        delete impl_;
        impl_ = nullptr;
    }
}

}  // namespace nickel::rhi