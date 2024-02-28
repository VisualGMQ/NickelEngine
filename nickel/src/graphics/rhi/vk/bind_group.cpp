#include "graphics/rhi/vk/bind_group.hpp"
#include "graphics/rhi/vk/buffer.hpp"
#include "graphics/rhi/vk/convert.hpp"
#include "graphics/rhi/vk/device.hpp"
#include "graphics/rhi/vk/sampler.hpp"
#include "graphics/rhi/vk/texture_view.hpp"
#include "graphics/rhi/defs.hpp"

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
        return vk::DescriptorType::eCombinedImageSampler;
    }

    vk::DescriptorType operator()(const StorageTextureBinding& layout) const {
        return vk::DescriptorType::eStorageImage;
    }

    vk::DescriptorType operator()(const TextureBinding& layout) const {
        return vk::DescriptorType::eSampledImage;
    }
};

vk::DescriptorType BindingType2Vk(BindingType type) {
    switch (type) {
        case BindingType::Buffer:
            return vk::DescriptorType::eUniformBuffer;
        case BindingType::Sampler:
            return vk::DescriptorType::eCombinedImageSampler;
        case BindingType::Texture:
            return vk::DescriptorType::eSampledImage;
        case BindingType::StorageTexture:
            return vk::DescriptorType::eStorageImage;
    }
}

vk::DescriptorType getDescriptorType(const ResourceEntry& entry) {
    return std::visit(getDescriptorTypeHelper{}, entry.entry);
}

BindGroupLayoutImpl::BindGroupLayoutImpl(
    DeviceImpl& dev, const BindGroupLayout::Descriptor& desc)
    : device_{dev}, desc_{desc} {
    std::vector<vk::DescriptorSetLayoutBinding> bindings;
    for (auto& entry : desc.entries) {
        bindings.emplace_back(getBinding(entry));
    }

    vk::DescriptorSetLayoutCreateInfo info;
    info.setBindings(bindings);

    VK_CALL(layout, dev.device.createDescriptorSetLayout(info));

    createPool(dev.device, dev.swapchain.imageInfo.imagCount, desc);

    // query physics device to determine this const
    constexpr uint32_t shaderUniformMaxCount = 16;

    allocSets(dev.device,
              shaderUniformMaxCount * dev.swapchain.ImageInfo().imagCount *
                  MaxDrawCallPerCmdBuf,
              desc);
}

vk::DescriptorSetLayoutBinding BindGroupLayoutImpl::getBinding(
    const Entry& entry) {
    vk::DescriptorSetLayoutBinding binding;

    binding.setBinding(entry.binding)
        .setDescriptorCount(entry.arraySize)
        .setStageFlags(ShaderStage2Vk(entry.visibility))
        .setDescriptorType(BindingType2Vk(entry.type));

    return binding;
}


void BindGroupLayoutImpl::createPool(vk::Device dev, uint32_t count, const BindGroupLayout::Descriptor& desc) {
    vk::DescriptorPoolCreateInfo info;
    std::vector<vk::DescriptorPoolSize> sizes;
    uint32_t maxCount = 0;
    for (auto& entry : desc.entries) {
        vk::DescriptorPoolSize size;
        size.setType(BindingType2Vk(entry.type))
            .setDescriptorCount(count);
        sizes.emplace_back(size);
        maxCount += count;
    }
    info.setPoolSizes(sizes).setMaxSets(maxCount);

    VK_CALL(pool, dev.createDescriptorPool(info));
}

void BindGroupLayoutImpl::allocSets(vk::Device dev, uint32_t count,
                              const BindGroupLayout::Descriptor& desc) {
    std::vector<vk::DescriptorSetLayout> layouts{
        count, layout};
    vk::DescriptorSetAllocateInfo info;
    info.setDescriptorSetCount(count)
        .setDescriptorPool(pool)
        .setSetLayouts(layouts);
    VK_CALL(sets, dev.allocateDescriptorSets(info));
}

BindGroupLayoutImpl::~BindGroupLayoutImpl() {
    device_.device.freeDescriptorSets(pool, sets);
    device_.device.destroyDescriptorPool(pool);
    device_.device.destroyDescriptorSetLayout(layout);
}

BindGroupImpl::BindGroupImpl(DeviceImpl& dev, const BindGroup::Descriptor& desc)
    : device_{dev}, layout_{desc.layout}, desc_{desc} {
    // move sets to here;
}

struct WriteDescriptorHelper final {
    WriteDescriptorHelper(vk::DescriptorSet set, vk::Device dev,
                          const Entry& entry)
        : set_{set}, dev_{dev}, entry_{entry} {}

    void operator()(const BufferBinding& binding) const {
        vk::WriteDescriptorSet writeInfo;
        vk::DescriptorBufferInfo bufferInfo;

        auto buffer = static_cast<const BufferImpl*>(binding.buffer.Impl());
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
            .setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
            .setImageView(
                static_cast<const TextureViewImpl*>(binding.view.Impl())
                    ->GetView());
        writeInfo.setImageInfo(imageInfo)
            .setDescriptorCount(1)
            .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
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

void BindGroupImpl::WriteDescriptors() {
    auto& layoutDesc =
        static_cast<const BindGroupLayoutImpl*>(desc_.layout.Impl())
            ->Descriptor();

    for (auto set : sets) {
        for (int i = 0; i < layoutDesc.entries.size(); i++) {
            WriteDescriptorHelper helper(set, device_.device, layoutDesc.entries[i]);
            std::visit(helper, desc_.entries[i].entry);
        }
    }
}

}  // namespace nickel::rhi::vulkan