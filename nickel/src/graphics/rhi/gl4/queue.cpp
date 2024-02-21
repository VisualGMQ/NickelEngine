#include "graphics/rhi/gl4/queue.hpp"
#include "graphics/rhi/gl4/command.hpp"

namespace nickel::rhi::gl4 {

void QueueImpl::Submit(const std::vector<CommandBuffer>& cmds) {
    for (auto cmd : cmds) {
        static_cast<const CommandBufferImpl*>(cmd.Impl())->Execute();
    }
}

}