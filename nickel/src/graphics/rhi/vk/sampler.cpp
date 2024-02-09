#include "graphics/rhi/vk/sampler.hpp"
#include "graphics/rhi/vk/convert.hpp"
#include "graphics/rhi/vk/device.hpp"

namespace nickel::rhi::vulkan {

SamplerImpl::SamplerImpl(DeviceImpl& dev, const Sampler::Descriptor& desc): dev_{dev.device} {
    vk::SamplerCreateInfo info;
    info.setAddressModeU(SamplerAddressMode2Vk(desc.u))
        .setAddressModeV(SamplerAddressMode2Vk(desc.v))
        .setAddressModeW(SamplerAddressMode2Vk(desc.w))
        .setCompareEnable(desc.compare.has_value())
        .setAnisotropyEnable(desc.maxAnisotropy.has_value())
        .setMagFilter(Filter2Vk(desc.mag))
        .setMinFilter(Filter2Vk(desc.min))
        .setMinLod(desc.lodMinClamp)
        .setMaxLod(desc.lodMaxClamp)
        .setUnnormalizedCoordinates(false)
        .setMipmapMode(desc.mipmapFilter == Filter::Linear
                           ? vk::SamplerMipmapMode::eLinear
                           : vk::SamplerMipmapMode::eNearest);

    if (desc.compare.has_value()) {
        info.setCompareOp(CompareOp2Vk(desc.compare.value()));
    }
    if (desc.maxAnisotropy.has_value()) {
        info.setMaxAnisotropy(desc.maxAnisotropy.value());
    }

    VK_CALL(sampler, dev.device.createSampler(info));
}

SamplerImpl::~SamplerImpl() {
    if (sampler) {
        dev_.destroySampler(sampler);
    }
}

}