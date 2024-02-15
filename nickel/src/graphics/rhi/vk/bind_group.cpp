#include "graphics/rhi/vk/bind_group.hpp"
#include "graphics/rhi/vk/buffer.hpp"
#include "graphics/rhi/vk/convert.hpp"
#include "graphics/rhi/vk/device.hpp"
#include "graphics/rhi/vk/sampler.hpp"
#include "graphics/rhi/vk/texture.hpp"
#include "graphics/rhi/vk/texture_view.hpp"

namespace nickel::rhi::vulkan {

struct getDescriptorTypeHelper {
    vk::DescriptorType operator()(const BufferBinding& layout) const {
        switch (layout.type) {
            case BufferType::ReadOnlyStorage:
                if (layout.hasDynamicOffset) {
                    return vk::DescriptorType::eStorageBufferDynamic;
                } else {
                    return vk::DescriptorType::eStorageBuffer;
                }
            case BufferType::Storage:
                if (layout.hasDynamicOffset) {
                    return vk::DescriptorType::eStorageBufferDynamic;
                } else {
                    return vk::DescriptorType::eStorageBuffer;
                }
            case BufferType::Uniform:
                if (layout.hasDynamicOffset) {
                    return vk::DescriptorType::eUniformBufferDynamic;
                } else {
                    return vk::DescriptorType::eUniformBuffer;
                }
        }
    }

    vk::DescriptorType operator()(const SamplerBinding& layout) const {
        return vk::DescriptorType::eSampler;
    }

    vk::DescriptorType operator()(const StorageTextureBinding& layout) const {
        return vk::DescriptorType::eStorageImage;
    }

    vk::DescriptorType operator()(const TextureBinding& layout) const {
        return vk::DescriptorType::eSampledImage;
    }
};

vk::DescriptorType getDescriptorType(const ResourceLayout& layout) {
    return std::visit(getDescriptorTypeHelper{}, layout);
}

BindGroupLayoutImpl::BindGroupLayoutImpl(
    DeviceImpl& dev, const BindGroupLayout::Descriptor& desc)
    : device_{dev.device} {
    std::vector<vk::DescriptorSetLayoutBinding> bindings;
    for (auto& entry : desc.entries) {
        bindings.emplace_back(getBinding(entry));
    }

    vk::DescriptorSetLayoutCreateInfo info;
    info.setBindings(bindings);

    VK_CALL(layout, dev.device.createDescriptorSetLayout(info));
}

vk::DescriptorSetLayoutBinding BindGroupLayoutImpl::getBinding(
    const Entry& entry) {
    vk::DescriptorSetLayoutBinding binding;

    binding.setBinding(entry.binding)
        .setDescriptorCount(entry.arraySize)
        .setStageFlags(ShaderStage2Vk(entry.visibility))
        .setDescriptorType(getDescriptorType(entry.resourceLayout));

    return binding;
}

BindGroupLayoutImpl::~BindGroupLayoutImpl() {
    device_.destroyDescriptorSetLayout(layout);
}

BindGroupImpl::BindGroupImpl(DeviceImpl& dev, const BindGroup::Descriptor& desc)
    : device_{dev}, layout_{desc.layout} {
    createPool(dev.device, dev.swapchain.ImageInfo().imagCount, desc);
    allocSets(dev.device, dev.swapchain.Images().size(), desc);
    writeDescriptors(dev.device, desc);
}

void BindGroupImpl::createPool(vk::Device dev, uint32_t imageCount,
                               const BindGroup::Descriptor& desc) {
    vk::DescriptorPoolCreateInfo info;
    std::vector<vk::DescriptorPoolSize> sizes;
    uint32_t maxCount = 0;
    for (auto& entry : desc.entries) {
        vk::DescriptorPoolSize size;
        size.setType(getDescriptorType(entry.resourceLayout))
            .setDescriptorCount(imageCount);
        sizes.emplace_back(size);
        maxCount += imageCount;
    }
    info.setPoolSizes(sizes).setMaxSets(maxCount);

    VK_CALL(pool, dev.createDescriptorPool(info));
}

void BindGroupImpl::allocSets(vk::Device dev, uint32_t imageCount,
                              const BindGroup::Descriptor& desc) {
    std::vector<vk::DescriptorSetLayout> layouts{
        imageCount,
        static_cast<const BindGroupLayoutImpl*>(desc.layout.Impl())->layout};
    vk::DescriptorSetAllocateInfo info;
    info.setDescriptorSetCount(imageCount)
        .setDescriptorPool(pool)
        .setSetLayouts(layouts);
    VK_CALL(sets, dev.allocateDescriptorSets(info));
}

struct WriteDescriptorHelper final {
    WriteDescriptorHelper(vk::DescriptorSet set, vk::Device dev,
                          const Entry& entry)
        : set_{set}, dev_{dev}, entry_{entry} {}

    void operator()(const BufferBinding& binding) const {
        vk::WriteDescriptorSet writeInfo;
        vk::DescriptorBufferInfo bufferInfo;

        auto buffer = static_cast<BufferImpl*>(binding.buffer.Impl());
        bufferInfo.setBuffer(buffer->buffer)
            .setOffset(0)
            .setRange(binding.minBindingSize ? binding.minBindingSize.value()
                                             : buffer->Size());
        writeInfo.setBufferInfo(bufferInfo)
            .setDescriptorCount(1)
            .setDescriptorType(vk::DescriptorType::eUniformBuffer)
            .setDstBinding(entry_.binding)
            .setDstArrayElement(0)
            .setDstSet(set_);
        dev_.updateDescriptorSets(writeInfo, {});
    }

    void operator()(const SamplerBinding& binding) const {
        vk::WriteDescriptorSet writeInfo;
        vk::DescriptorImageInfo imageInfo;

        imageInfo
            .setSampler(static_cast<const SamplerImpl*>(binding.sampler.Impl())
                            ->sampler)
            .setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal);
        writeInfo.setImageInfo(imageInfo)
            .setDescriptorCount(1)
            .setDescriptorType(vk::DescriptorType::eSampler)
            .setDstBinding(entry_.binding)
            .setDstArrayElement(0)
            .setDstSet(set_);
        dev_.updateDescriptorSets(writeInfo, {});
    }

    void operator()(const StorageTextureBinding& binding) const {
        vk::WriteDescriptorSet writeInfo;
        vk::DescriptorImageInfo imageInfo;

        imageInfo
            .setImageView(
                static_cast<const TextureViewImpl*>(binding.view.Impl())
                    ->GetView())
            .setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal);
        writeInfo.setImageInfo(imageInfo)
            .setDescriptorCount(1)
            .setDescriptorType(vk::DescriptorType::eStorageImage)
            .setDstBinding(entry_.binding)
            .setDstArrayElement(0)
            .setDstSet(set_);
        dev_.updateDescriptorSets(writeInfo, {});
    }

    void operator()(const TextureBinding& binding) const {
        vk::WriteDescriptorSet writeInfo;
        vk::DescriptorImageInfo imageInfo;

        imageInfo
            .setImageView(
                static_cast<const TextureViewImpl*>(binding.view.Impl())
                    ->GetView())
            .setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal);
        writeInfo.setImageInfo(imageInfo)
            .setDescriptorCount(1)
            .setDescriptorType(vk::DescriptorType::eSampledImage)
            .setDstBinding(entry_.binding)
            .setDstArrayElement(0)
            .setDstSet(set_);
        dev_.updateDescriptorSets(writeInfo, {});
    }

private:
    vk::DescriptorSet set_;
    vk::Device dev_;
    const Entry& entry_;
};

void BindGroupImpl::writeDescriptors(vk::Device dev,
                                     const BindGroup::Descriptor& desc) {
    uint32_t imgCount = device_.swapchain.ImageInfo().imagCount;
    for (int i = 0; i < imgCount; i++) {
        auto set = sets[i];
        for (int j = 0; j < desc.entries.size(); j++) {
            auto& entry = desc.entries[j];

            WriteDescriptorHelper helper(set, dev, entry);
            std::visit(helper, entry.resourceLayout);
        }
    }
}

BindGroupImpl::~BindGroupImpl() {
    device_.device.resetDescriptorPool(pool);
    device_.device.destroyDescriptorPool(pool);
}

}  // namespace nickel::rhi::vulkan