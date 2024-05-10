#pragma once

#include "graphics/rhi/impl/sampler.hpp"
#include "graphics/rhi/vk/util.hpp"
#include "graphics/rhi/vk/pch.hpp"
#include "graphics/rhi/sampler.hpp"

namespace nickel::rhi::vulkan {

class DeviceImpl;

class SamplerImpl: public rhi::SamplerImpl {
public:
    explicit SamplerImpl(DeviceImpl& dev, const Sampler::Descriptor&);
    ~SamplerImpl();

    vk::Sampler sampler;

private:
    vk::Device dev_;
};

}