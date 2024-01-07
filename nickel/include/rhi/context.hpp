#pragma once

#include "rhi/instance.hpp"
#include "rhi/device.hpp"
#include "rhi/shader.hpp"

namespace nickel::rhi {

enum APIType {
    GL3,
    Vulkan,
};

class Context {
public:
    Context() = default;
    virtual ~Context() = default;

    Context(const Context&) = delete;
    Context& operator=(const Context&) = delete;

    virtual const class Instance& Instance() const = 0;
    virtual const class Device& Device() const = 0;

    class Instance& Instance() {
        return const_cast<class Instance&>(std::as_const(*this).Instance());
    }
    class Device& Device() {
        return const_cast<class Device&>(std::as_const(*this).Device());
    }
};


// call this before create Context
void SetAPIHint();

}  // namespace nickel::rhi