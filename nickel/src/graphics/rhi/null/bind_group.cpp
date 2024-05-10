#include "graphics/rhi/null/bind_group.hpp"
#include "graphics/rhi/bind_group.hpp"
#include "graphics/rhi/null/device.hpp"


namespace nickel::rhi::null {

BindGroupLayout BindGroupImpl::GetLayout() const {
    DeviceImpl impl;
    return BindGroupLayout{APIPreference::Null, impl, {}};
}

}  // namespace nickel::rhi::null