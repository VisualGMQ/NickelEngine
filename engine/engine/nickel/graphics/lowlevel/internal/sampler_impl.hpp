#pragma once
#include "nickel/graphics/lowlevel/sampler.hpp"
#include "nickel/internal/pch.hpp"
#include "nickel/common/memory/refcountable.hpp"

namespace nickel::graphics {

class DeviceImpl;

class SamplerImpl : public RefCountable {
public:
    explicit SamplerImpl(DeviceImpl& dev, const Sampler::Descriptor&);
    SamplerImpl(const SamplerImpl&) = delete;
    SamplerImpl(SamplerImpl&&) = delete;
    SamplerImpl& operator=(const SamplerImpl&) = delete;
    SamplerImpl& operator=(SamplerImpl&&) = delete;

    ~SamplerImpl();

    void DecRefcount() override;
    VkSampler m_sampler = VK_NULL_HANDLE;

private:
    DeviceImpl& m_dev;
};

}  // namespace nickel::graphics