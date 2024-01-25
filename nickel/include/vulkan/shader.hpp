#pragma once

#include "core/manager.hpp"
#include "pch.hpp"

namespace nickel::vulkan {

class Device;

class ShaderModule {
public:
    ShaderModule(Device* device, vk::ShaderStageFlagBits type,
                 const std::vector<char>& content,
                 const std::string& entry = "main");

    ShaderModule(const ShaderModule&) = delete;

    ShaderModule(ShaderModule&& o) { swap(*this, o); }

    ShaderModule& operator=(const ShaderModule&) = delete;

    ShaderModule& operator=(ShaderModule&& o) {
        if (&o != this) {
            swap(*this, o);
        }
        return *this;
    }

    ~ShaderModule();

    auto GetType() const { return type_; }

    vk::PipelineShaderStageCreateInfo GetShaderStageCreateInfo();

private:
    vk::ShaderModule module_;
    Device* device_ = nullptr;
    std::string entry_ = "main";
    vk::ShaderStageFlagBits type_ = vk::ShaderStageFlagBits::eAll;

    friend void swap(ShaderModule& o1, ShaderModule& o2) noexcept {
        using std::swap;

        swap(o1.module_, o2.module_);
        swap(o1.device_, o2.device_);
        swap(o1.entry_, o2.entry_);
        swap(o1.type_, o2.type_);
    }
};

}  // namespace nickel::vulkan