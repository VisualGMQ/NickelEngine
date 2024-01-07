#pragma once
#include "pch.hpp"
#include "rhi/descriptor.hpp"
#include "rhi/enums.hpp"

namespace nickel::rhi {

struct DescriptorSetLayoutBinding final {
    uint32_t binding;
    uint32_t count;
    DescriptorType type;
    ShaderStageFlags shaderStage;
};

class DescriptorLayout {
public:
    virtual ~DescriptorLayout() = default;
};

struct PushConstantRange final {
    uint32_t offset;
    uint32_t size;
    ShaderStageFlags shaderStage;
};

}  // namespace nickel::rhi