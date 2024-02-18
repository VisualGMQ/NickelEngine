#pragma once
#include "graphics/rhi/impl/bind_group.hpp"

namespace nickel::rhi::gl4 {

class BindGroupLayoutImpl: public rhi::BindGroupLayoutImpl {
public:
    explicit BindGroupLayoutImpl(const BindGroupLayout::Descriptor&);
    const BindGroupLayout::Descriptor& Descriptor() const;

private:
    BindGroupLayout::Descriptor desc_;
};

class BindGroupImpl: public rhi::BindGroupImpl {
public:
    explicit BindGroupImpl(BindGroupLayout);
    BindGroupLayout GetLayout() const override;

private:
    BindGroupLayout layout_;
};

}