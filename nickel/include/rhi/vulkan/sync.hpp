#pragma once

#include "pch.hpp"
#include "rhi/sync.hpp"

namespace nickel::rhi::vulkan {

class Device;

class Semaphore {
public:
    explicit Semaphore(Device* device);
    Semaphore(const Semaphore&) = delete;
    Semaphore& operator=(const Semaphore&) = delete;

    Semaphore(Semaphore&& o) { swap(o, *this); }

    Semaphore& operator=(Semaphore&& o) {
        if (&o != this) {
            swap(o, *this);
        }
        return *this;
    }

    ~Semaphore();

    auto& Raw() const { return sem_; }

private:
    Device* device_{};
    vk::Semaphore sem_;

    friend void swap(Semaphore& o1, Semaphore& o2) noexcept {
        using std::swap;
        swap(o1.device_, o2.device_);
        swap(o1.sem_, o2.sem_);
    }
};

class Fence : public rhi::Fence {
public:
    explicit Fence(Device*, bool signaled);
    Fence(const Fence&) = delete;
    Fence& operator=(const Fence&) = delete;

    Fence(Fence&& o) { swap(o, *this); }

    Fence& operator=(Fence&& o) {
        if (&o != this) {
            swap(o, *this);
        }
        return *this;
    }

    ~Fence();

    auto& Raw() const { return fence_; }

private:
    Device* device_{};
    vk::Fence fence_;

    friend void swap(Fence& o1, Fence& o2) noexcept {
        using std::swap;
        swap(o1.device_, o2.device_);
        swap(o1.fence_, o2.fence_);
    }
};

class Event : public rhi::Event {
public:
    explicit Event(Device*, bool deviceOnly);
    Event(const Event&) = delete;
    Event& operator=(const Event&) = delete;

    Event(Event&& o) { swap(o, *this); }

    Event& operator=(Event&& o) {
        if (&o != this) {
            swap(o, *this);
        }
        return *this;
    }

    ~Event();

    auto& Raw() const { return event_; }

private:
    Device* device_{};
    vk::Event event_;

    friend void swap(Event& o1, Event& o2) noexcept {
        using std::swap;
        swap(o1.device_, o2.device_);
        swap(o1.event_, o2.event_);
    }
};

}  // namespace nickel::rhi::vulkan