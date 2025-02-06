﻿#pragma once
#include "nickel/graphics/lowlevel/graphics_pipeline.hpp"
#include "nickel/graphics/lowlevel/pipeline_layout.hpp"
#include "nickel/common/memory/refcountable.hpp"

namespace nickel::graphics {

class DeviceImpl;

class GraphicsPipelineImpl: public RefCountable {
public:
    GraphicsPipelineImpl(DeviceImpl&, const GraphicsPipeline::Descriptor&);
    GraphicsPipelineImpl(const GraphicsPipelineImpl&) = delete;
    GraphicsPipelineImpl(GraphicsPipelineImpl&&) = delete;
    GraphicsPipelineImpl& operator=(const GraphicsPipelineImpl&) = delete;
    GraphicsPipelineImpl& operator=(GraphicsPipelineImpl&&) = delete;

    ~GraphicsPipelineImpl();

    void DecRefcount() override;

    VkPipeline m_pipeline = VK_NULL_HANDLE;
    PipelineLayout m_layout;

private:
    DeviceImpl& m_device;
    RenderPass m_render_pass;
};


}