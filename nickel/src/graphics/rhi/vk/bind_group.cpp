#include "graphics/rhi/vk/bind_group.hpp"
#include "graphics/rhi/defs.hpp"
#include "graphics/rhi/vk/buffer.hpp"
#include "graphics/rhi/vk/convert.hpp"
#include "graphics/rhi/vk/device.hpp"
#include "graphics/rhi/vk/queue.hpp"
#include "graphics/rhi/vk/sampler.hpp"
#include "graphics/rhi/vk/texture.hpp"
#include "graphics/rhi/vk/texture_view.hpp"
#include "graphics/rhi/vk/util.hpp"

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

vk::DescriptorType getDescriptorType(const BindingPoint& entry) {
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

    uint32_t count = dev.swapchain.ImageInfo().imagCount * MaxDrawCallPerCmdBuf;
    createPool(dev.device, count, desc);
    allocSets(dev.device, count, desc);

    ids_.resize(MaxDrawCallPerCmdBuf);
}

vk::DescriptorSetLayoutBinding BindGroupLayoutImpl::getBinding(
    const Entry& entry) {
    vk::DescriptorSetLayoutBinding binding;

    binding.setBinding(entry.binding.binding)
        .setDescriptorCount(entry.arraySize)
        .setStageFlags(ShaderStage2Vk(entry.visibility))
        .setDescriptorType(getDescriptorType(entry.binding));

    return binding;
}

void BindGroupLayoutImpl::createPool(vk::Device dev, uint32_t count,
                                     const BindGroupLayout::Descriptor& desc) {
    vk::DescriptorPoolCreateInfo info;
    std::vector<vk::DescriptorPoolSize> sizes;
    uint32_t maxCount = 0;
    for (auto& entry : desc.entries) {
        vk::DescriptorPoolSize size;
        size.setType(getDescriptorType(entry.binding))
            .setDescriptorCount(count);
        sizes.emplace_back(size);
        maxCount += count;
    }
    info.setPoolSizes(sizes).setMaxSets(maxCount);

    VK_CALL(pool, dev.createDescriptorPool(info));
}

void BindGroupLayoutImpl::allocSets(vk::Device dev, uint32_t count,
                                    const BindGroupLayout::Descriptor& desc) {
    std::vector<vk::DescriptorSetLayout> layouts{count, layout};
    vk::DescriptorSetAllocateInfo info;
    info.setDescriptorSetCount(count).setDescriptorPool(pool).setSetLayouts(
        layouts);
    ids_.resize(count / MaxDrawCallPerCmdBuf);
    for (int i = 0; i < ids_.size(); i++) {
        ids_[i] = i;
    }
    VK_CALL(sets, dev.allocateDescriptorSets(info));
}

std::vector<vk::DescriptorSet> BindGroupLayoutImpl::RequireSets(uint32_t id) {
    std::vector<vk::DescriptorSet> returnSets;
    auto imgCount = device_.swapchain.imageInfo.imagCount;
    for (int i = 0; i < imgCount; i++) {
        returnSets.emplace_back(sets[id * imgCount + i]);
    }
    return returnSets;
}

uint32_t BindGroupLayoutImpl::RequireBindGroupID() {
    for (uint32_t i = 0; i < ids_.size(); i++) {
        if (!ids_[i]) {
            ids_[i] = true;
            return i;
        }
    }

    return -1;
}

void BindGroupLayoutImpl::DestoryID(uint32_t id) {
    ids_[id] = false;
}

BindGroupLayoutImpl::~BindGroupLayoutImpl() {
    device_.device.destroyDescriptorPool(pool);
    device_.device.destroyDescriptorSetLayout(layout);
}

BindGroupImpl::BindGroupImpl(DeviceImpl& dev, const BindGroup::Descriptor& desc)
    : device_{dev}, desc_{desc} {
    auto layout = static_cast<BindGroupLayoutImpl*>(desc.layout.Impl());
    id_ = layout->RequireBindGroupID();
    sets = layout->RequireSets(id_);

    auto& layoutDesc =
        static_cast<const BindGroupLayoutImpl*>(desc.layout.Impl())
            ->Descriptor();
    desc_.layout = desc.layout;
    for (auto& layoutEntry : layoutDesc.entries) {
        desc_.entries.emplace_back(layoutEntry.binding);
        for (auto& groupEntry : desc.entries) {
            if (groupEntry.binding == layoutEntry.binding.binding) {
                desc_.entries.back() = groupEntry;
                break;
            }
        }
    }

    writeDescriptors();
}

BindGroupImpl::~BindGroupImpl() {
    static_cast<BindGroupLayoutImpl*>(desc_.layout.Impl())->DestoryID(id_);
}

struct WriteDescriptorHelper final {
    WriteDescriptorHelper(vk::DescriptorSet set, DeviceImpl& dev,
                          const BindingPoint& binding)
        : set_{set}, dev_{dev}, binding_{binding} {}

    void operator()(const BufferBinding& binding) const {
        if (!binding.buffer) {
            return;
        }

        vk::WriteDescriptorSet writeInfo;
        vk::DescriptorBufferInfo bufferInfo;

        auto buffer = static_cast<const BufferImpl*>(binding.buffer.Impl());
        bufferInfo.setBuffer(buffer->buffer)
            .setOffset(0)
            .setRange(binding.minBindingSize ? binding.minBindingSize.value()
                                             : buffer->Size());
        writeInfo.setBufferInfo(bufferInfo)
            .setDescriptorCount(1)
            .setDescriptorType(binding.hasDynamicOffset
                                   ? vk::DescriptorType::eUniformBufferDynamic
                                   : vk::DescriptorType::eUniformBuffer)
            .setDstBinding(binding_.binding)
            .setDstArrayElement(0)
            .setDstSet(set_);
        dev_.device.updateDescriptorSets(writeInfo, {});
    }

    void operator()(const SamplerBinding& binding) const {
        if (!binding.sampler) {
            return;
        }

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
            .setDstBinding(binding_.binding)
            .setDstArrayElement(0)
            .setDstSet(set_);
        dev_.device.updateDescriptorSets(writeInfo, {});
    }

    void operator()(const StorageTextureBinding& binding) const {
        if (!binding.view) {
            return;
        }

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
            .setDstBinding(binding_.binding)
            .setDstArrayElement(0)
            .setDstSet(set_);
        dev_.device.updateDescriptorSets(writeInfo, {});
    }

    void operator()(const TextureBinding& binding) const {
        if (!binding.view) {
            return;
        }

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
            .setDstBinding(binding_.binding)
            .setDstArrayElement(0)
            .setDstSet(set_);
        dev_.device.updateDescriptorSets(writeInfo, {});
    }

private:
    vk::DescriptorSet set_;
    DeviceImpl& dev_;
    const BindingPoint& binding_;
};


void BindGroupImpl::writeDescriptors() {
    auto& layoutDesc =
        static_cast<const BindGroupLayoutImpl*>(desc_.layout.Impl())
            ->Descriptor();
    for (auto set : sets) {
        for (auto& entry : desc_.entries) {
            WriteDescriptorHelper helper{set, device_, entry};
            std::visit(helper, entry.entry);
        }
    }
}

class LayoutTransformHelper final {
public:
    explicit LayoutTransformHelper(DeviceImpl& dev) : dev_{dev} {}

    void operator()(const BufferBinding& binding) {
        // TODO: maybe we need layout transform
    }

    void operator()(const SamplerBinding& binding) {
        vk::ImageLayout dstLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
        auto texture = static_cast<TextureImpl*>(binding.view.Texture().Impl());
        auto view = static_cast<TextureViewImpl*>(binding.view.Impl());

        if (texture->layout == dstLayout) {
            return ;
        }

        if (dstLayout != texture->layout) {
            auto aspect = DetermineTextureAspect(TextureAspect::All,
                                                 binding.view.Format());
            vk::ImageMemoryBarrier barrier;
            vk::ImageSubresourceRange range;
            range.setAspectMask(aspect)
                .setBaseArrayLayer(0)
                .setLayerCount(texture->Extent().depthOrArrayLayers)
                .setBaseMipLevel(0)
                .setLevelCount(1);

            vk::ImageLayout dstLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
            barrier.setOldLayout(texture->layout)
                .setNewLayout(dstLayout)
                .setSrcAccessMask(vk::AccessFlagBits::eTransferWrite)
                .setDstAccessMask(vk::AccessFlagBits::eShaderRead)
                .setImage(texture->GetImage())
                .setSubresourceRange(range);

            vk::CommandBufferAllocateInfo allocInfo;
            allocInfo.setCommandBufferCount(1)
                .setCommandPool(dev_.cmdPool)
                .setLevel(vk::CommandBufferLevel::ePrimary);
            std::vector<vk::CommandBuffer> cmds;
            VK_CALL(cmds, dev_.device.allocateCommandBuffers(allocInfo));
            vk::CommandBufferBeginInfo beginInfo;
            beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
            VK_CALL_NO_VALUE(cmds[0].begin(beginInfo));
            cmds[0].pipelineBarrier(vk::PipelineStageFlagBits::eTransfer,
                                    vk::PipelineStageFlagBits::eFragmentShader,
                                    vk::DependencyFlagBits::eByRegion, {}, {},
                                    barrier);
            VK_CALL_NO_VALUE(cmds[0].end());
            vk::SubmitInfo info;
            info.setCommandBuffers(cmds);
            VK_CALL_NO_VALUE(static_cast<QueueImpl*>(dev_.graphicsQueue->Impl())
                                 ->queue.submit(info));
            dev_.WaitIdle();
            dev_.device.freeCommandBuffers(dev_.cmdPool, cmds[0]);

            texture->layout = dstLayout;
        }
    }

    void operator()(const StorageTextureBinding& binding) {}

    void operator()(const TextureBinding& binding) {}

private:
    DeviceImpl& dev_;
};

void BindGroupImpl::Transformlayouts() {
    auto& layoutDesc =
        static_cast<const BindGroupLayoutImpl*>(desc_.layout.Impl())
            ->Descriptor();
    for (auto& entry : desc_.entries) {
        LayoutTransformHelper helper{device_};
        std::visit(helper, entry.entry);
    }
}

}  // namespace nickel::rhi::vulkan