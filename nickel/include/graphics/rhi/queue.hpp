#pragma once

#include "graphics/rhi/command.hpp"
namespace nickel::rhi {

class QueueImpl;

class Queue final {
public: 
    explicit Queue(QueueImpl* impl);
    void Submit(const std::vector<CommandBuffer>&);
    void Destroy();

    auto Impl() const { return impl_; }
    auto Impl() { return impl_; }

private:
    QueueImpl* impl_{};
};

}