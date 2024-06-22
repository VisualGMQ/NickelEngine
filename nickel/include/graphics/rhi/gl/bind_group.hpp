#pragma once
#include "graphics/rhi/impl/bind_group.hpp"

namespace nickel::rhi::gl {

class RenderPipelineImpl;

class BindGroupLayoutImpl: public rhi::BindGroupLayoutImpl {
public:
    explicit BindGroupLayoutImpl(const BindGroupLayout::Descriptor&);
    const BindGroupLayout::Descriptor& Descriptor() const;

private:
    BindGroupLayout::Descriptor desc_;
};

class BindGroupImpl: public rhi::BindGroupImpl {
public:
    explicit BindGroupImpl(const BindGroup::Descriptor& desc);
    BindGroupLayout GetLayout() const override;

    auto& Descriptor() const { return desc_; }

    void Apply(const RenderPipelineImpl&, const std::vector<uint32_t>& dynamicOffset) const;

private:
    BindGroup::Descriptor desc_;
};

}