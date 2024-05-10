#pragma once

#include "graphics/rhi/impl/bind_group.hpp"

namespace nickel::rhi::null {

class BindGroupImpl : public rhi::BindGroupImpl {
public:
    BindGroupLayout GetLayout() const override;
};

class BindGroupLayoutImpl : public rhi::BindGroupLayoutImpl {};

}  // namespace nickel::rhi::null