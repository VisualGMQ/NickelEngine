#pragma once
#include "nickel/common/memory/refcountable.hpp"
#include "nickel/graphics/lowlevel/pipeline_layout.hpp"
#include "nickel/internal/pch.hpp"

namespace nickel::graphics {

class DeviceImpl;
class BindGroupLayout;

class PipelineLayoutImpl final : public RefCountable {
public:
    PipelineLayoutImpl(DeviceImpl&, const PipelineLayout::Descriptor&);
    PipelineLayoutImpl(const PipelineLayoutImpl&) = delete;
    PipelineLayoutImpl(PipelineLayoutImpl&&) = delete;
    PipelineLayoutImpl& operator=(const PipelineLayoutImpl&) = delete;
    PipelineLayoutImpl& operator=(PipelineLayoutImpl&&) = delete;

    ~PipelineLayoutImpl();

    VkPipelineLayout m_pipeline_layout = VK_NULL_HANDLE;

    void DecRefcount() override;

private:
    DeviceImpl& m_device;
};

}  // namespace nickel::graphics