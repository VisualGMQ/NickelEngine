﻿#include "nickel/graphics/internal/pipeline_layout_impl.hpp"
#include "nickel/graphics/bind_group_layout.hpp"
#include "nickel/graphics/internal/bind_group_layout_impl.hpp"
#include "nickel/graphics/internal/device_impl.hpp"
#include "nickel/graphics/internal/vk_call.hpp"

namespace nickel::graphics {

PipelineLayoutImpl::PipelineLayoutImpl(
    DeviceImpl& device, const std::vector<BindGroupLayout>& layouts,
    const std::vector<VkPushConstantRange>& push_constant_ranges)
    : m_device{device} {
    std::vector<VkDescriptorSetLayout> set_layouts;
    set_layouts.reserve(layouts.size());
    for (auto& layout : layouts) {
        set_layouts.push_back(layout.Impl().m_layout);
    }

    VkPipelineLayoutCreateInfo ci{};
    ci.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    ci.setLayoutCount = set_layouts.size();
    ci.pSetLayouts = set_layouts.data();
    ci.pushConstantRangeCount = push_constant_ranges.size();
    ci.pPushConstantRanges = push_constant_ranges.data();

    VK_CALL(vkCreatePipelineLayout(device.m_device, &ci, nullptr,
                                   &m_pipeline_layout));
}

PipelineLayoutImpl::~PipelineLayoutImpl() {
    vkDestroyPipelineLayout(m_device.m_device, m_pipeline_layout, nullptr);
}

}  // namespace nickel::graphics