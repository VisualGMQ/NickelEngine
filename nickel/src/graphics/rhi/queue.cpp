#include "graphics/rhi/queue.hpp"
#include "graphics/rhi/vk/queue.hpp"

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