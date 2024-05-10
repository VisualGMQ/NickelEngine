#pragma once

#include "graphics/rhi/bind_group.hpp"

namespace nickel::rhi {

class BindGroupLayoutImpl {
public:
    virtual ~BindGroupLayoutImpl() = default;
};

class BindGroupImpl {
public:
    virtual ~BindGroupImpl() = default;
    virtual BindGroupLayout GetLayout() const = 0;
};

}  // namespace nickel::rhi