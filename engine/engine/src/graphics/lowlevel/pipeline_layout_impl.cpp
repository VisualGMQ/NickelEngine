#include "nickel/graphics/lowlevel/internal/pipeline_layout_impl.hpp"
#include "nickel/graphics/lowlevel/bind_group_layout.hpp"
#include "nickel/graphics/lowlevel/internal/bind_group_layout_impl.hpp"
#include "nickel/graphics/lowlevel/internal/device_impl.hpp"
#include "nickel/graphics/lowlevel/internal/enum_convert.hpp"
#include "nickel/graphics/lowlevel/internal/vk_call.hpp"

namespace nickel::graphics {

PipelineLayoutImpl::PipelineLayoutImpl(DeviceImpl& device,
                                       const PipelineLayout::Descriptor& desc)
    : m_device{device} {
    std::vector<VkDescriptorSetLayout> set_layouts;
    set_layouts.reserve(desc.layouts.size());
    for (auto& layout : desc.layouts) {
        set_layouts.push_back(layout.Impl().m_layout);
    }

    VkPipelineLayoutCreateInfo ci{};
    ci.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    ci.setLayoutCount = set_layouts.size();
    ci.pSetLayouts = set_layouts.data();

    std::vector<VkPushConstantRange> push_constants;
    for (auto& range : desc.push_contants) {
        VkPushConstantRange r;
        r.offset = range.offset;
        r.size = range.size;
        r.stageFlags = ShaderStage2Vk(range.shader_stage);
        push_constants.push_back(r);
    }

    ci.pushConstantRangeCount = push_constants.size();
    ci.pPushConstantRanges = push_constants.data();

    VK_CALL(vkCreatePipelineLayout(device.m_device, &ci, nullptr,
                                   &m_pipeline_layout));
}

PipelineLayoutImpl::~PipelineLayoutImpl() {
    vkDestroyPipelineLayout(m_device.m_device, m_pipeline_layout, nullptr);
}

void PipelineLayoutImpl::DecRefcount() {
    RefCountable::DecRefcount();

    if (Refcount() == 0) {
        m_device.m_pending_delete_pipeline_layouts.push_back(this);
    }
}

}  // namespace nickel::graphics