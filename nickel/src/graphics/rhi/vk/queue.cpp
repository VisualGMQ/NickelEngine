#include "graphics/rhi/vk/queue.hpp"
#include "graphics/rhi/vk/command.hpp"
#include "graphics/rhi/vk/device.hpp"

namespace nickel::rhi::vulkan {

QueueImpl::QueueImpl(DeviceImpl& dev, vk::Queue queue)
    : dev_{dev}, queue{queue} {}

void QueueImpl::Submit(const std::vector<CommandBuffer>& cmds) {
    auto cmdType = static_cast<const CommandBufferImpl*>(cmds[0].Impl())->Type();

    std::vector<vk::CommandBuffer> bufs;
    for (auto& cmd : cmds) {
        bufs.push_back(
            static_cast<const vulkan::CommandBufferImpl*>(cmd.Impl())->buf);
    }

    if (cmdType == CmdType::RenderPass) {
        vk::SubmitInfo info;
        vk::PipelineStageFlags waitDstStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;
        info.setCommandBuffers(bufs)
            .setWaitSemaphores(dev_.imageAvaliableSems[dev_.curFrame])
            .setSignalSemaphores(dev_.renderFinishSems[dev_.curFrame])
            .setWaitDstStageMask(waitDstStage);
        VK_CALL_NO_VALUE(queue.submit(info, dev_.fences[dev_.curFrame]));
    } else if (cmdType == CmdType::CopyData) {
        vk::SubmitInfo info;
        info.setCommandBuffers(bufs);
        VK_CALL_NO_VALUE(queue.submit(info));
        // TODO: may use semaphore to accelerate?
        dev_.WaitIdle();
    }
}

}  // namespace nickel::rhi::vulkan