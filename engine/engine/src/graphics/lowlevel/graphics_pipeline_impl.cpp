#include "nickel/graphics/lowlevel/internal/graphics_pipeline_impl.hpp"
#include "nickel/graphics/lowlevel/graphics_pipeline.hpp"
#include "nickel/graphics/lowlevel/internal/device_impl.hpp"
#include "nickel/graphics/lowlevel/internal/enum_convert.hpp"
#include "nickel/graphics/lowlevel/internal/render_pass_impl.hpp"
#include "nickel/graphics/lowlevel/internal/vk_call.hpp"

namespace nickel::graphics {
GraphicsPipelineImpl::GraphicsPipelineImpl(
    DeviceImpl& dev, const GraphicsPipeline::Descriptor& desc)
    : m_layout{desc.m_layout}, m_device{dev}, m_render_pass{desc.m_render_pass} {
    VkGraphicsPipelineCreateInfo ci{};
    ci.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    ci.subpass = desc.m_subpass;

    std::vector<VkPipelineShaderStageCreateInfo> stage_ci_list;
    // shader stage
    {
        stage_ci_list.reserve(desc.m_shader_stages.size());
        for (auto&& [stage, module] : desc.m_shader_stages) {
            VkPipelineShaderStageCreateInfo shader_stage{};
            shader_stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            shader_stage.stage = static_cast<VkShaderStageFlagBits>(ShaderStage2Vk(stage));
            shader_stage.pName = module.m_entry_name.c_str();
            shader_stage.module = module.m_module.GetImpl()->m_module;
            stage_ci_list.push_back(shader_stage);
        }

        ci.stageCount = stage_ci_list.size();
        ci.pStages = stage_ci_list.data();
    }

    // vertex input state
    auto& vertexInputState = desc.m_vertex;
    VkPipelineVertexInputStateCreateInfo vertexInput{};
    vertexInput.sType =
        VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    std::vector<VkVertexInputAttributeDescription> attrDescs;
    std::vector<VkVertexInputBindingDescription> bindings;
    for (int i = 0; i < vertexInputState.m_buffers.size(); i++) {
        auto& state = vertexInputState.m_buffers[i];
        for (auto& attr : state.m_attributes) {
            VkVertexInputAttributeDescription attrDesc{};
            attrDesc.binding = i;
            attrDesc.location = attr.m_shader_location;
            attrDesc.format = VertexFormat2Vk(attr.m_format);
            attrDesc.offset = attr.m_offset;
            attrDescs.emplace_back(attrDesc);
        }

        VkVertexInputBindingDescription binding{};
        binding.binding = i;
        binding.stride = state.m_array_stride;
        binding.inputRate =
            state.m_step_mode ==
                    GraphicsPipeline::Descriptor::BufferState::StepMode::Vertex
                ? VK_VERTEX_INPUT_RATE_VERTEX
                : VK_VERTEX_INPUT_RATE_INSTANCE;
        bindings.emplace_back(binding);
    }
    vertexInput.vertexAttributeDescriptionCount = attrDescs.size();
    vertexInput.pVertexAttributeDescriptions = attrDescs.data();
    vertexInput.vertexBindingDescriptionCount = bindings.size();
    vertexInput.pVertexBindingDescriptions = bindings.data();

    // input assembly state
    VkPipelineInputAssemblyStateCreateInfo inputAsm{};
    inputAsm.sType =
        VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAsm.topology = Topology2Vk(desc.m_primitive.m_topology);
    inputAsm.primitiveRestartEnable = false;  // TODO: read from config later

    // rasterization
    VkPipelineRasterizationStateCreateInfo rasterState{};
    rasterState.sType =
        VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterState.rasterizerDiscardEnable =
        false;  // TODO: read from config later

    if (desc.m_depth_stencil) {
        rasterState.depthBiasEnable = desc.m_depth_stencil->m_depth_bias.has_value();
        rasterState.depthClampEnable =
            desc.m_depth_stencil->m_depth_bias_clamp.has_value();
        rasterState.depthBiasSlopeFactor =
            desc.m_depth_stencil->m_depth_bias_slope_scale;

        if (desc.m_depth_stencil->m_depth_bias) {
            rasterState.depthBiasConstantFactor =
                desc.m_depth_stencil->m_depth_bias.value();
        }

        if (desc.m_depth_stencil->m_depth_bias_clamp) {
            rasterState.depthBiasClamp =
                desc.m_depth_stencil->m_depth_bias_clamp.value();
        }
    }

    rasterState.cullMode = CullMode2Vk(desc.m_primitive.m_cull_mode);
    rasterState.polygonMode = PolygonMode2Vk(desc.m_primitive.m_polygon_mode);
    rasterState.lineWidth = 1.0f;  // TODO: read from config later
    rasterState.frontFace = FrontFace2Vk(desc.m_primitive.m_front_face);

    // multisample
    VkPipelineMultisampleStateCreateInfo multisample{};
    multisample.sType =
        VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisample.alphaToCoverageEnable = desc.m_multisample.m_alpha_to_coverage_enabled;
    multisample.sampleShadingEnable = false;
    multisample.rasterizationSamples = static_cast<VkSampleCountFlagBits>(
        SampleCount2Vk(desc.m_multisample.m_count));
    multisample.pSampleMask = &desc.m_multisample.m_mask;

    // color blend
    std::vector<VkPipelineColorBlendAttachmentState> attachmentStates;
    for (auto& state : desc.m_blend_state) {
        VkPipelineColorBlendAttachmentState colorState;
        colorState.colorBlendOp = BlendOp2Vk(state.m_color.m_operation);
        colorState.srcColorBlendFactor =
            BlendFactor2Vk(state.m_color.m_src_factor);
        colorState.dstColorBlendFactor = BlendFactor2Vk(state.m_color.m_dst_factor);
        colorState.alphaBlendOp = BlendOp2Vk(state.m_alpha.m_operation);
        colorState.srcAlphaBlendFactor = BlendFactor2Vk(state.m_alpha.m_src_factor);
        colorState.dstAlphaBlendFactor = BlendFactor2Vk(state.m_alpha.m_dst_factor);
        colorState.blendEnable = true;
        colorState.colorWriteMask = ColorComponent2Vk(state.m_color_mask);
        attachmentStates.emplace_back(colorState);
    }

    VkPipelineColorBlendStateCreateInfo colorBlend{};
    colorBlend.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlend.logicOpEnable = false;
    colorBlend.attachmentCount = attachmentStates.size();
    colorBlend.pAttachments = attachmentStates.data();

    // m_depth m_stencil state
    VkPipelineDepthStencilStateCreateInfo depthStencilState{};
    depthStencilState.sType =
        VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;

    if (desc.m_depth_stencil) {
        VkStencilOpState frontState, backState;
        frontState.writeMask = desc.m_depth_stencil->m_stencil_front.m_write_mask;
        frontState.compareMask = desc.m_depth_stencil->m_stencil_front.m_compare_mask;
        frontState.compareOp =
            CompareOp2Vk(desc.m_depth_stencil->m_stencil_front.m_compare);
        frontState.failOp =
            StencilOp2Vk(desc.m_depth_stencil->m_stencil_front.m_failed_op);
        frontState.passOp =
            StencilOp2Vk(desc.m_depth_stencil->m_stencil_front.m_pass_op);
        frontState.depthFailOp =
            StencilOp2Vk(desc.m_depth_stencil->m_stencil_front.m_depth_fail_op);
        frontState.reference = 0;

        backState.writeMask = desc.m_depth_stencil->m_stencil_back.m_write_mask;
        backState.compareMask = desc.m_depth_stencil->m_stencil_back.m_compare_mask;
        backState.compareOp =
            CompareOp2Vk(desc.m_depth_stencil->m_stencil_back.m_compare);
        backState.failOp =
            StencilOp2Vk(desc.m_depth_stencil->m_stencil_back.m_failed_op);
        backState.passOp = StencilOp2Vk(desc.m_depth_stencil->m_stencil_back.m_pass_op);
        backState.depthFailOp =
            StencilOp2Vk(desc.m_depth_stencil->m_stencil_back.m_depth_fail_op);
        backState.reference = 0;

        depthStencilState.depthBoundsTestEnable = false;
        depthStencilState.depthWriteEnable =
            desc.m_depth_stencil->m_depth_write_enabled;
        depthStencilState.depthTestEnable = true;
        depthStencilState.stencilTestEnable = false;
        depthStencilState.depthCompareOp =
            CompareOp2Vk(desc.m_depth_stencil->m_depth_compare);
        depthStencilState.front = frontState;
        depthStencilState.back = backState;

        ci.pDepthStencilState = &depthStencilState;
    }

    // dynamic states
    VkPipelineDynamicStateCreateInfo dynState{};
    dynState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    std::array states = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR,
    };
    dynState.dynamicStateCount = states.size();
    dynState.pDynamicStates = states.data();
    ci.pDynamicState = &dynState;

    // temporary viewport
    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    VkViewport viewport{};
    VkRect2D scissor{};
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    // create pipeline
    ci.pDepthStencilState = &depthStencilState;
    ci.renderPass = desc.m_render_pass.GetImpl()->m_render_pass;
    ci.pVertexInputState = &vertexInput;
    ci.pInputAssemblyState = &inputAsm;
    ci.pTessellationState = nullptr;
    ci.pViewportState = &viewportState;
    ci.pRasterizationState = &rasterState;
    ci.pMultisampleState = &multisample;
    ci.pColorBlendState = &colorBlend;
    ci.pDynamicState = &dynState;
    ci.layout = desc.m_layout.GetImpl()->m_pipeline_layout;

    VK_CALL(vkCreateGraphicsPipelines(dev.m_device, VK_NULL_HANDLE, 1, &ci,
                                      nullptr, &m_pipeline));
}

GraphicsPipelineImpl::~GraphicsPipelineImpl() {
    vkDestroyPipeline(m_device.m_device, m_pipeline, nullptr);
}

void GraphicsPipelineImpl::DecRefcount() {
    RefCountable::DecRefcount();

    if (Refcount() == 0) {
        m_device.m_graphics_pipeline_allocator.MarkAsGarbage(this);
    }
}
}