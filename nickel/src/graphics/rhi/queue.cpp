#include "graphics/rhi/queue.hpp"
#ifdef NICKEL_HAS_VULKAN
#include "graphics/rhi/vk/queue.hpp"
#endif
#include "graphics/rhi/gl4/queue.hpp"

namespace nickel::rhi {

Queue::Queue(QueueImpl* impl) : impl_{impl} {}

void Queue::Submit(const std::vector<CommandBuffer>& cmds) {
    impl_->Submit(cmds);
}

void Queue::Destroy() {
    if (impl_) {
        delete impl_;
        impl_ = nullptr;
    }
}

}  // namespace nickel::rhi