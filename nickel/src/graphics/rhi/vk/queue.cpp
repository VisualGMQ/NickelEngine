#include "graphics/rhi/vk/queue.hpp"
#include "graphics/rhi/vk/command.hpp"
#include "graphics/rhi/vk/device.hpp"

namespace nickel::rhi::vulkan {

QueueImpl::QueueImpl(DeviceImpl& dev, vk::Queue queue)
    : dev_{dev}, queue{queue} {}

void QueueImpl::Submit(const std::vector<CommandBuffer>& cmds) {
    std::vector<vk::CommandBuffer> bufs(cmds.size());
    uint32_t renderPassCmdCount = 0;
    bool needWaitImageAvaliable =
        std::any_of(cmds.begin(), cmds.end(), [](CommandBuffer buf) {
            return static_cast<CommandBufferImpl*>(buf.Impl())->needWaitImageAvaliFence;
        });

    std::transform(cmds.begin(), cmds.end(), bufs.begin(),
                   [](CommandBuffer cmd) {
                       return static_cast<CommandBufferImpl*>(cmd.Impl())->buf;
                   });

    if (needWaitImageAvaliable) {
        auto fence = dev_.fences[dev_.curFrame];

        vk::SubmitInfo info;
        vk::PipelineStageFlags waitDstStage =
            vk::PipelineStageFlagBits::eColorAttachmentOutput;
        info.setCommandBuffers(bufs)
            .setWaitSemaphores(dev_.imageAvaliableSems[dev_.curFrame])
            .setSignalSemaphores(dev_.renderFinishSems[dev_.curFrame])
            .setWaitDstStageMask(waitDstStage);
        VK_CALL_NO_VALUE(queue.submit(info, fence));
        dev_.needPresent = true;
    } else {
        vk::SubmitInfo info;
        info.setCommandBuffers(bufs);
        VK_CALL_NO_VALUE(queue.submit(info));
        VK_CALL_NO_VALUE(queue.waitIdle());
    }
}

}  // namespace nickel::rhi::vulkan