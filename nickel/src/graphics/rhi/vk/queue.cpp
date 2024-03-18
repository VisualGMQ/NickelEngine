#include "graphics/rhi/vk/queue.hpp"
#include "graphics/rhi/vk/command.hpp"
#include "graphics/rhi/vk/device.hpp"

namespace nickel::rhi::vulkan {

QueueImpl::QueueImpl(DeviceImpl& dev, vk::Queue queue)
    : dev_{dev}, queue{queue} {}

void QueueImpl::Submit(const std::vector<CommandBuffer>& cmds) {
    std::vector<vk::CommandBuffer> bufs;
    uint32_t renderPassCmdCount = 0;
    for (auto& cmd : cmds) {
        bufs.push_back(
            static_cast<const vulkan::CommandBufferImpl*>(cmd.Impl())->buf);

        auto type = static_cast<const CommandBufferImpl*>(cmd.Impl())->Type();
        dev_.cmdCounter.Add(type);
        if (type == CmdType::RenderPass) {
            renderPassCmdCount++;
        }
    }

    if (renderPassCmdCount > 0) {
        auto fence = dev_.fences[dev_.curFrame];
        VK_CALL_NO_VALUE(dev_.device.waitForFences(fence, true, UINT64_MAX));
        dev_.device.resetFences(fence);

        vk::SubmitInfo info;
        vk::PipelineStageFlags waitDstStage =
            vk::PipelineStageFlagBits::eColorAttachmentOutput;
        if (dev_.cmdCounter.Get(CmdType::RenderPass) - renderPassCmdCount == 0) {
            info.setCommandBuffers(bufs)
                .setWaitSemaphores(dev_.imageAvaliableSems[dev_.curFrame])
                .setSignalSemaphores(dev_.renderFinishSems[dev_.curFrame])
                .setWaitDstStageMask(waitDstStage);
        } else {
            info.setCommandBuffers(bufs)
                .setWaitSemaphores(dev_.renderFinishSems[dev_.curFrame])
                .setSignalSemaphores(dev_.renderFinishSems[dev_.curFrame])
                .setWaitDstStageMask(waitDstStage);
        }
        VK_CALL_NO_VALUE(queue.submit(info, fence));
    } else {
        vk::SubmitInfo info;
        info.setCommandBuffers(bufs);
        VK_CALL_NO_VALUE(queue.submit(info));
        // TODO: may use semaphore to accelerate?
        dev_.WaitIdle();
    }

    for (auto& cmd : cmds) {
        static_cast<vulkan::CommandBufferImpl*>(cmd.Impl())
            ->ApplyLayoutTransition();
    }
}

}  // namespace nickel::rhi::vulkan