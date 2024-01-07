#pragma once

#include "pch.hpp"
#include "core/manager.hpp"
#include "rhi/device.hpp"

namespace nickel::rhi {

class ShaderModule: public Asset {
public:
    enum class Type {
        Unknown,
        Vertex,
        Fragment,
        Compute,
    };

    explicit ShaderModule(Type type) : type_{type} {}

    ShaderModule(const ShaderModule&) = delete;
    ShaderModule& operator=(const ShaderModule&) = delete;

    auto GetType() const { return type_; }

private:
    Type type_ = Type::Unknown;
};

inline std::string_view ShaderModuleType2Str(ShaderModule::Type type) {
    switch (type) {
        case ShaderModule::Type::Vertex:
            return "Vertex";
        case ShaderModule::Type::Fragment:
            return "Fragment";
        case ShaderModule::Type::Compute:
            return "Compute";
        case ShaderModule::Type::Unknown:
            return "Unknown";
    }
}

}  // namespace nickel::rhi