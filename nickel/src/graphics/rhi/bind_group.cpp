#include "graphics/rhi/bind_group.hpp"
#include "graphics/rhi/null/bind_group.hpp"
#include "graphics/rhi/vk/bind_group.hpp"
#include "graphics/rhi/vk/device.hpp"

namespace nickel::rhi {

BindGroupLayout::BindGroupLayout(APIPreference api, DeviceImpl& dev,
                                 const Descriptor& desc) {
    switch (api) {
        case APIPreference::Undefine:
            break;
        case APIPreference::GL:
            break;
        case APIPreference::Vulkan:
            impl_ = new vulkan::BindGroupLayoutImpl(
                static_cast<vulkan::DeviceImpl&>(dev), desc);
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
            break;
        case APIPreference::Vulkan:
            impl_ = new vulkan::BindGroupImpl(
                static_cast<vulkan::DeviceImpl&>(dev), desc);
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