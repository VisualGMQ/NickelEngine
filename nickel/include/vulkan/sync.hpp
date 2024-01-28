#pragma once

#include "pch.hpp"

namespace nickel::vulkan {

class Device;

class Semaphore final {
public:
    explicit Semaphore(Device*);
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

    operator vk::Semaphore() const { return sem_; }

    operator vk::Semaphore() { return sem_; }

private:
    Device* device_{};
    vk::Semaphore sem_;

    friend void swap(Semaphore& o1, Semaphore& o2) noexcept {
        using std::swap;

        swap(o1.device_, o2.device_);
        swap(o1.sem_, o2.sem_);
    }
};

class Fence final {
public:
    Fence(Device*, bool signal = false);
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

    operator vk::Fence() const { return fence_; }

    operator vk::Fence() { return fence_; }

    void Wait(std::optional<uint64_t> timeout);
    void Reset();

private:
    Device* device_{};
    vk::Fence fence_;

    friend void swap(Fence& o1, Fence& o2) noexcept {
        using std::swap;

        swap(o1.device_, o2.device_);
        swap(o1.fence_, o2.fence_);
    }
};

class Event final {
public:
    explicit Event(Device*, bool deviceOnly = false);
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

    operator vk::Event() const { return event_; }

    operator vk::Event() { return event_; }

private:
    Device* device_{};
    vk::Event event_;

    friend void swap(Event& o1, Event& o2) noexcept {
        using std::swap;

        swap(o1.device_, o2.device_);
        swap(o1.event_, o2.event_);
    }
};

}  // namespace nickel::vulkan