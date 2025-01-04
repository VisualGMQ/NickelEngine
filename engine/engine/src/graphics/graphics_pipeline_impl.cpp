#include "nickel/graphics/internal/graphics_pipeline_impl.hpp"
#include "nickel/graphics/graphics_pipeline.hpp"
#include "nickel/graphics/internal/device_impl.hpp"
#include "nickel/graphics/internal/render_pass_impl.hpp"
#include "nickel/graphics/internal/shader_module_impl.hpp"
#include "nickel/graphics/internal/vk_call.hpp"

nickel::graphics::GraphicsPipelineImpl::GraphicsPipelineImpl(
    DeviceImpl& dev, const GraphicsPipeline::Descriptor& desc)
    : m_layout{desc.m_layout},
      m_device{dev},
      m_render_pass{desc.m_render_pass} {
    VkGraphicsPipelineCreateInfo ci{};
    ci.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    ci.subpass = desc.m_subpass;

    // shader stage
    {
        std::vector<VkPipelineShaderStageCreateInfo> stage_ci_list;
        stage_ci_list.reserve(desc.m_shader_stages.size());
        for (auto&& [stage, module] : desc.m_shader_stages) {
            VkPipelineShaderStageCreateInfo ci{};
            ci.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            ci.stage = stage;
            ci.pName = module.entry_name.c_str();
            ci.module = module.module.Impl().m_module;
            stage_ci_list.push_back(ci);
        }

        ci.stageCount = stage_ci_list.size();
        ci.pStages = stage_ci_list.data();
    }

    ci.pDepthStencilState = &desc.m_depth_stencil_state;
    ci.renderPass = desc.m_render_pass.Impl().m_render_pass;
    ci.pVertexInputState = &desc.m_vertex_input_state;
    ci.pInputAssemblyState = &desc.m_input_assembly_state;
    ci.pTessellationState = &desc.m_tessellation_state;
    ci.pViewportState = &desc.m_viewport_state;
    ci.pRasterizationState = &desc.m_rasterization_state;
    ci.pMultisampleState = &desc.m_multisample_state;
    ci.pColorBlendState = &desc.m_color_blend_state;
    ci.pDynamicState = &desc.m_dynamic_state;

    VK_CALL(vkCreateGraphicsPipelines(dev.m_device, VK_NULL_HANDLE, 1, &ci,
                                      nullptr, &m_pipeline));
}

nickel::graphics::GraphicsPipelineImpl::~GraphicsPipelineImpl() {
    vkDestroyPipeline(m_device.m_device, m_pipeline, nullptr);
}