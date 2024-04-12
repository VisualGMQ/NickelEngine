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
    void Destroy();

    auto Impl() const { return impl_; }
    auto Impl() { return impl_; }

    operator bool() const {
        return impl_ != nullptr;
    }

private:
    std::shared_ptr<ShaderModuleImpl> impl_{};
};

}
