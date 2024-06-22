#include "graphics/rhi/gl/queue.hpp"
#include "graphics/rhi/gl/command.hpp"
#include "graphics/rhi/gl/device.hpp"

namespace nickel::rhi::gl {

QueueImpl::QueueImpl(DeviceImpl& device): device_{device} {}

void QueueImpl::Submit(const std::vector<CommandBuffer>& cmds) {
    for (auto cmd : cmds) {
        static_cast<CommandBufferImpl*>(cmd.Impl())->Execute(device_);
    }
}

}