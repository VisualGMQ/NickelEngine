#include "graphics/rhi/null/queue.hpp"
#include "graphics/rhi/gl/command.hpp"

namespace nickel::rhi::null {

void QueueImpl::Submit(const std::vector<CommandBuffer>& bufs) { }

}  // namespace nickel::rhi::null