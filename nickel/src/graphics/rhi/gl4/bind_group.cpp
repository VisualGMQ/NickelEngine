#include "graphics/rhi/gl4/bind_group.hpp"

namespace nickel::rhi::gl4 {

BindGroupLayoutImpl::BindGroupLayoutImpl(
    const BindGroupLayout::Descriptor& desc)
    : desc_{desc} {}

const BindGroupLayout::Descriptor& BindGroupLayoutImpl::Descriptor() const {
    return desc_;
}

BindGroupImpl::BindGroupImpl(BindGroupLayout layout) : layout_{layout} {}

BindGroupLayout BindGroupImpl::GetLayout() const {
    return layout_;
}

}  // namespace nickel::rhi::gl4