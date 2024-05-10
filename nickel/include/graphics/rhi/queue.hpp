#pragma once

#include "graphics/rhi/command.hpp"
namespace nickel::rhi {

class QueueImpl;

class Queue final {
public: 
    explicit Queue(QueueImpl* impl);
    Queue(Queue&& o) noexcept { swap(o, *this); }
    Queue(const Queue& o) = default;
    Queue& operator=(const Queue& o) = default;

    Queue& operator=(Queue&& o) noexcept {
        if (&o != this) swap(o, *this);
        return *this;
    }
    void Submit(const std::vector<CommandBuffer>&);
    void Destroy();

    auto Impl() const { return impl_; }
    auto Impl() { return impl_; }

private:
    QueueImpl* impl_{};

    friend void swap(Queue & o1, Queue& o2) noexcept {
        using std::swap;

        swap(o1.impl_, o2.impl_);
    }
};

}