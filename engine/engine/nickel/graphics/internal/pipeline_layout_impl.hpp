#pragma once
#include "nickel/graphics/internal/refcountable.hpp"
#include "nickel/graphics/pipeline_layout.hpp"
#include "nickel/internal/pch.hpp"

namespace nickel::graphics {

class DeviceImpl;
class BindGroupLayout;

class PipelineLayoutImpl final : public RefCountable {
public:
    PipelineLayoutImpl(DeviceImpl&, const PipelineLayout::Descriptor&);
    ~PipelineLayoutImpl();

    VkPipelineLayout m_pipeline_layout;

    void PendingDelete();

private:
    DeviceImpl& m_device;
};

}  // namespace nickel::graphics