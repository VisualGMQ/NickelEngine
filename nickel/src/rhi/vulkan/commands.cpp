#include "rhi/vulkan/commands.hpp"
#include "rhi/vulkan/device.hpp"
#include "rhi/vulkan/enum_convert.hpp"
#include "rhi/vulkan/pipeline.hpp"
#include "rhi/vulkan/buffer.hpp"

namespace nickel::rhi::vulkan {

CommandBuffer::CommandBuffer(vk::CommandBuffer cmd): cmd_{cmd} { }

void CommandBuffer::BeginRecord(CommandBufferUsage usage) {
    vk::CommandBufferBeginInfo info;
    info.setFlags(CommandBufferUsage2Vk(usage));
    cmd_.begin(info);
}

void CommandBuffer::BindPipeline(PipelineBindPoint bind, const rhi::Pipeline& pipeline) {
    const vk::Pipeline* vkPipeline = nullptr;
    switch (pipeline.Type()) {
        case Pipeline::Type::Graphics:
            vkPipeline = &static_cast<const vulkan::GraphicsPipeline&>(pipeline).Raw();
            break;
        case Pipeline::Type::Compute:
            // TODO: add compute pipeline
            // vkPipeline = &static_cast<const ComputePipeline&>(pipeline).Raw();
            break;
    }
    cmd_.bindPipeline(PipelineBindPoint2Vk(bind), *vkPipeline);
}

void CommandBuffer::Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance)  {
    cmd_.draw(vertexCount, instanceCount, firstVertex, firstInstance);
}

void CommandBuffer::BindVertexBuffer(uint32_t firstBinding,
                                     const std::vector<rhi::BufferBundle*>& buffers,
                                     const std::vector<uint64_t>& offsets) {
    std::vector<vk::Buffer> bufs;
    for (auto& buffer : buffers) {
        bufs.emplace_back(static_cast<const vulkan::BufferBundle*>(buffer)->Buffer().Raw());
    }
    bindBuffers_.emplace_back(bindBuffer{std::move(bufs)});
    cmd_.bindVertexBuffers(firstBinding, (uint32_t)buffers.size(),
                           bindBuffers_.back().buffers.data(),
                           offsets.data());
}

void CommandBuffer::EndRecord() {
    cmd_.end();
}

void CommandBuffer::Reset() {
    cmd_.reset();

    // inner state clear
    bindBuffers_.clear();
}

CommandPool::CommandPool(Device* device, CommandPoolCreateFlag flag,
                         uint32_t queueIndex)
    : device_{device} {
    vk::CommandPoolCreateInfo createInfo;
    createInfo.setQueueFamilyIndex(queueIndex)
        .setFlags(CommandPoolCreateFlag2Vk(flag));
    pool_ = device->Raw().createCommandPool(createInfo);
    if (!pool_) {
        LOGE(log_tag::Vulkan, "create command pool");
    }
}

CommandPool::~CommandPool() {
    if (device_ && pool_) {
        device_->Raw().destroyCommandPool(pool_);
    }
}

std::vector<CommandBuffer> CommandPool::Allocate(CommandBufferLevel level, uint32_t count) {
    vk::CommandBufferAllocateInfo allocInfo;
    allocInfo.setCommandPool(pool_)
        .setCommandBufferCount(count)
        .setLevel(CommandBufferLevel2Vk(level));
    auto cmds = device_->Raw().allocateCommandBuffers(allocInfo);
    std::vector<CommandBuffer> buffers;
    for (auto& cmd : cmds) {
        buffers.emplace_back(cmd);
    }

    return buffers;
}

void CommandPool::Reset() {
    device_->Raw().resetCommandPool(pool_);
}

}  // namespace nickel::rhi::vulkan