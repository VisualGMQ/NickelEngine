#include "graphics/rhi/vk/queue.hpp"
#include "graphics/rhi/vk/command.hpp"
#include "graphics/rhi/vk/device.hpp"

namespace nickel::rhi::vulkan {

QueueImpl::QueueImpl(DeviceImpl& dev, vk::Queue queue)
    : dev_{dev}, queue{queue} {}

void QueueImpl::Submit(const std::vector<CommandBuffer>& cmds) {
    std::vector<vk::CommandBuffer> bufs;
    for (auto& cmd : cmds) {
        bufs.push_back(
            static_cast<const vulkan::CommandBufferImpl*>(cmd.Impl())->buf);
    }

    vk::SubmitInfo info;
    vk::PipelineStageFlags waitDstStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    info.setCommandBuffers(bufs)
        .setWaitSemaphores(dev_.imageAvaliableSems[dev_.curFrame])
        .setSignalSemaphores(dev_.renderFinishSems[dev_.curFrame])
        .setWaitDstStageMask(waitDstStage);
    VK_CALL_NO_VALUE(queue.submit(info, dev_.fences[dev_.curFrame]));
}

}  // namespace nickel::rhi::vulkan