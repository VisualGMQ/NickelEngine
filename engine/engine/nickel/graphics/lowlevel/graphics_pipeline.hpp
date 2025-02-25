#pragma once
#include "nickel/common/dllexport.hpp"
#include "nickel/graphics/lowlevel/enums.hpp"
#include "nickel/graphics/lowlevel/pipeline_layout.hpp"
#include "nickel/graphics/lowlevel/render_pass.hpp"
#include "nickel/graphics/lowlevel/shader_module.hpp"
#include <optional>

namespace nickel::graphics {

class GraphicsPipelineImpl;

class NICKEL_API GraphicsPipeline: public ImplWrapper<GraphicsPipelineImpl> {
public:
    struct Descriptor {
        struct ShaderStage {
            ShaderModule m_module;
            std::string m_entry_name = "main";
        };

        struct DepthStencilState final {
            struct StencilOpState {
                CompareOp m_compare = CompareOp::Always;
                StencilOp m_depth_fail_op = StencilOp::Keep;
                StencilOp m_failed_op = StencilOp::Keep;
                StencilOp m_pass_op = StencilOp::Keep;
                uint32_t m_write_mask = 0xFFFFFFFF;
                uint32_t m_compare_mask = 0xFFFFFFFF;
            };

            std::optional<float> m_depth_bias;
            std::optional<float> m_depth_bias_clamp;
            float m_depth_bias_slope_scale = 0;
            CompareOp m_depth_compare = CompareOp::Greater;
            bool m_depth_write_enabled = true;
            Format m_depth_format;
            StencilOpState m_stencil_back;
            StencilOpState m_stencil_front;
        };

        struct BlendComponentState final {
            BlendFactor m_dst_factor = BlendFactor::Zero;
            BlendFactor m_src_factor = BlendFactor::One;
            BlendOp m_operation = BlendOp::Add;
        };

        struct BlendState final {
            BlendComponentState m_alpha;
            BlendComponentState m_color;
            Flags<ColorComponent> m_color_mask = ColorComponent::All;
        };

        struct BufferState final {
            struct Attribute {
                VertexFormat m_format;
                uint64_t m_offset;
                uint32_t m_shader_location;
            };

            enum class StepMode {
                Instance,
                Vertex,
            } m_step_mode = StepMode::Vertex;

            uint64_t m_array_stride;
            std::vector<Attribute> m_attributes;
        };

        struct MultisampleState final {
            bool m_alpha_to_coverage_enabled = false;
            SampleCount m_count = SampleCount::Count1;
            uint32_t m_mask = 0xFFFFFFFF;
        };

        struct PrimitiveState final {
            CullMode m_cull_mode = CullMode::None;
            FrontFace m_front_face = FrontFace::CCW;
            StripIndexFormat m_strip_index_format = StripIndexFormat::Uint32;
            Topology m_topology = Topology::TriangleList;
            bool m_unclipped_depth = false;
            PolygonMode m_polygon_mode = PolygonMode::Fill;
        };

        struct ShaderState {
            ShaderModule m_module;
            std::string m_entry_point = "main";
        };

        struct VertexState {
            std::vector<BufferState> m_buffers;
        };

        uint32_t m_subpass{};
        std::unordered_map<graphics::ShaderStage, ShaderStage> m_shader_stages;
        VertexState m_vertex;
        std::optional<DepthStencilState> m_depth_stencil;
        std::vector<BlendState> m_blend_state;
        MultisampleState m_multisample;
        PrimitiveState m_primitive;
        PipelineLayout m_layout;
        RenderPass m_render_pass;
    };

    using ImplWrapper::ImplWrapper;
};

}  // namespace nickel::graphics