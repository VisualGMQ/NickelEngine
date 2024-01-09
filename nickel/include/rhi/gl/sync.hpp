#pragma once

#include "pch.hpp"
#include "rhi/sync.hpp"

namespace nickel::rhi::gl {

class Semaphore : public rhi::Semaphore {};

class Fence {
public:
    Fence();
    Fence(const Fence&) = delete;
    Fence& operator=(const Fence&) = delete;
    Fence(Fence&& o) { swap(o, *this); }
    Fence& operator=(Fence&& o) {
        if (&o != this) {
            swap(*this, o);
        }
        return *this;
    }
    ~Fence();

private:
    GLsync fence_ = 0;

    friend void swap(Fence& o1, Fence& o2) noexcept {
        using std::swap;
        swap(o1.fence_, o2.fence_);
    }
};

class Event : public rhi::Semaphore {};

}  // namespace nickel::rhi::gl