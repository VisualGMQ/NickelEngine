#pragma once

#include "core/manager.hpp"
#include "pch.hpp"
#include "rhi/shader.hpp"


namespace nickel::rhi::vulkan {

class Device;

class ShaderModule : public rhi::ShaderModule {
public:
    ShaderModule(Device* device, const std::filesystem::path& filename,
                 rhi::ShaderModule::Type);

    ShaderModule(const ShaderModule&) = delete;

    ShaderModule(ShaderModule&& o)
        : rhi::ShaderModule{rhi::ShaderModule::Type::Unknown} {
        swap(*this, o);
    }

    ShaderModule& operator=(const ShaderModule&) = delete;

    ShaderModule& operator=(ShaderModule&& o) {
        if (&o != this) {
            swap(*this, o);
        }
        return *this;
    }

    ~ShaderModule();

    void SetType(Type type) { type_ = type; }

    auto GetType() const { return type_; }

    toml::table Save2Toml() const override {
        // TODO: not finish
        return {};
    }

    vk::PipelineShaderStageCreateInfo GetShaderStageCreateInfo();

private:
    vk::ShaderModule module_;
    Device* device_ = nullptr;
    Type type_ = Type::Unknown;

    friend void swap(ShaderModule& o1, ShaderModule& o2) noexcept {
        using std::swap;

        swap(o1.module_, o2.module_);
        swap(o1.device_, o2.device_);
        swap(o1.type_, o2.type_);
    }
};

vk::ShaderStageFlagBits ShaderType2Vulkan(ShaderModule::Type);

}  // namespace nickel::rhi::vulkan