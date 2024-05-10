#pragma once

#include "graphics/rhi/common.hpp"
#include "stdpch.hpp"

namespace nickel::rhi {

class ShaderModuleImpl;
class DeviceImpl;
class Adapter;

class ShaderModule final {
public:
    struct Descriptor {
        std::vector<char> code;
    };

    ShaderModule() = default;
    ShaderModule(APIPreference, DeviceImpl&, const Descriptor&);
    ShaderModule(const ShaderModule& o) = default;
    ShaderModule& operator=(const ShaderModule& o) = default;

    ShaderModule(ShaderModule&& o) noexcept { swap(o, *this); }

    ShaderModule& operator=(ShaderModule&& o) noexcept {
        if (&o != this) swap(o, *this);
        return *this;
    }
    void Destroy();

    auto Impl() const { return impl_; }
    auto Impl() { return impl_; }

    operator bool() const {
        return impl_ != nullptr;
    }

private:
    std::shared_ptr<ShaderModuleImpl> impl_{};

    friend void swap(ShaderModule& o1, ShaderModule& o2) noexcept {
        using std::swap;

        swap(o1.impl_, o2.impl_);
    }
};

}
