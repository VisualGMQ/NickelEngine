#pragma once

#include "pch.hpp"
#include "core/log_tag.hpp"
#include "window/window.hpp"
#include "rhi/instance.hpp"

namespace nickel::rhi::vulkan {

class Instance: public rhi::Instance {
public:
    explicit Instance(Window& window);

    ~Instance() {
        instance_.destroy();
    }

    Instance(const Instance&) = delete;
    Instance(Instance&& o) {
        swap(*this, o);
    }

    Instance& operator=(const Instance&) = delete;
    Instance& operator=(Instance&& o) {
        if (&o != this) {
            swap(*this, o);
        }
        return *this;
    }

    vk::Instance& Raw() { return instance_; }

    auto& GetWindow() const { return *window_; }
    auto& GetWindow() { return *window_; }

private:
    vk::Instance instance_{};
    Window* window_ = nullptr;

    friend void swap(Instance& o1, Instance& o2) noexcept {
        using std::swap;
        swap(o1.instance_, o2.instance_);
        swap(o1.window_, o2.window_);
    }
};

}
