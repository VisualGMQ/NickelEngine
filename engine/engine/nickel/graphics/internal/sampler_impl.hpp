#pragma once
#include "nickel/graphics/sampler.hpp"
#include "nickel/internal/pch.hpp"
#include "refcountable.hpp"

namespace nickel::graphics {

class DeviceImpl;

class SamplerImpl : public RefCountable {
public:
    explicit SamplerImpl(DeviceImpl& dev, const Sampler::Descriptor&);
    ~SamplerImpl();

    VkSampler m_sampler;

private:
    VkDevice m_dev;
};

}  // namespace nickel::graphics