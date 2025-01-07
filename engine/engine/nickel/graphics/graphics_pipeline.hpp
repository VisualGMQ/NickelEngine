#pragma once
#include "nickel/graphics/pipeline_layout.hpp"
#include "nickel/graphics/render_pass.hpp"
#include "nickel/graphics/shader_module.hpp"
#include "nickel/common/dllexport.hpp"

namespace nickel::graphics {

class GraphicsPipelineImpl;

class NICKEL_API GraphicsPipeline {
public:
    struct ShaderStage {
        ShaderModule module;
        std::string entry_name;
    };

    struct Descriptor {
        RenderPass m_render_pass;
        std::unordered_map<VkShaderStageFlagBits, ShaderStage> m_shader_stages;

        VkPipelineVertexInputStateCreateInfo m_vertex_input_state;
        VkPipelineInputAssemblyStateCreateInfo m_input_assembly_state;
        VkPipelineTessellationStateCreateInfo m_tessellation_state;
        VkPipelineViewportStateCreateInfo m_viewport_state;
        VkPipelineRasterizationStateCreateInfo m_rasterization_state;
        VkPipelineMultisampleStateCreateInfo m_multisample_state;
        VkPipelineDepthStencilStateCreateInfo m_depth_stencil_state;
        VkPipelineColorBlendStateCreateInfo m_color_blend_state;
        VkPipelineDynamicStateCreateInfo m_dynamic_state;
        PipelineLayout m_layout;
        uint32_t m_subpass;
    };

    GraphicsPipeline() = default;
    explicit GraphicsPipeline(GraphicsPipelineImpl*);
    GraphicsPipeline(const GraphicsPipeline&);
    GraphicsPipeline(GraphicsPipeline&&) noexcept;
    GraphicsPipeline& operator=(const GraphicsPipeline&) noexcept;
    GraphicsPipeline& operator=(GraphicsPipeline&&) noexcept;
    ~GraphicsPipeline();

    const GraphicsPipelineImpl& Impl() const noexcept;
    GraphicsPipelineImpl& Impl() noexcept;

    operator bool() const noexcept;
    void Release();

private:
    GraphicsPipelineImpl* m_impl{};
};

}  // namespace nickel::graphics