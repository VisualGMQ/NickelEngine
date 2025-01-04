#pragma once
#include "nickel/graphics/internal/refcountable.hpp"
#include "nickel/internal/pch.hpp"

namespace nickel::graphics {

class DeviceImpl;
class BindGroupLayout;

class PipelineLayoutImpl final : public RefCountable {
public:
    PipelineLayoutImpl(
        DeviceImpl&, const std::vector<BindGroupLayout>&,
        const std::vector<VkPushConstantRange>&);
    ~PipelineLayoutImpl();

    VkPipelineLayout m_pipeline_layout;

private:
    DeviceImpl& m_device;
};

}  // namespace nickel::graphics