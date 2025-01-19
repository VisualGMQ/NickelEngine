#pragma once
#include "nickel/common/dllexport.hpp"
#include "nickel/graphics/enums.hpp"
#include "nickel/graphics/pipeline_layout.hpp"
#include "nickel/graphics/render_pass.hpp"
#include "nickel/graphics/shader_module.hpp"
#include <optional>

namespace nickel::graphics {

class GraphicsPipelineImpl;

class NICKEL_API GraphicsPipeline {
public:
    struct Descriptor {
        struct ShaderStage {
            ShaderModule module;
            std::string entry_name = "main";
        };

        struct DepthStencilState final {
            struct StencilOpState {
                CompareOp compare = CompareOp::Always;
                StencilOp depthFailOp = StencilOp::Keep;
                StencilOp failedOp = StencilOp::Keep;
                StencilOp passOp = StencilOp::Keep;
                uint32_t writeMask = 0xFFFFFFFF;
                uint32_t compareMask = 0xFFFFFFFF;
            };

            std::optional<float> depthBias;
            std::optional<float> depthBiasClamp;
            float depthBiasSlopeScale = 0;
            CompareOp depthCompare = CompareOp::Greater;
            bool depthWriteEnabled = true;
            Format depthFormat;
            StencilOpState stencilBack;
            StencilOpState stencilFront;
        };

        struct BlendComponentState final {
            BlendFactor dstFactor = BlendFactor::Zero;
            BlendFactor srcFactor = BlendFactor::One;
            BlendOp operation = BlendOp::Add;
        };

        struct BlendState final {
            BlendComponentState alpha;
            BlendComponentState color;
            Flags<ColorComponent> colorMask = ColorComponent::All;
        };

        struct BufferState final {
            struct Attribute {
                VertexFormat format;
                uint64_t offset;
                uint32_t shaderLocation;
            };

            enum class StepMode {
                Instance,
                Vertex,
            } stepMode = StepMode::Vertex;

            uint64_t arrayStride;
            std::vector<Attribute> attributes;
        };

        struct MultisampleState final {
            bool alphaToCoverageEnabled = false;
            SampleCount count = SampleCount::Count1;
            uint32_t mask = 0xFFFFFFFF;
        };

        struct PrimitiveState final {
            CullMode cullMode = CullMode::None;
            FrontFace frontFace = FrontFace::CCW;
            StripIndexFormat stripIndexFormat = StripIndexFormat::Uint32;
            Topology topology = Topology::TriangleList;
            bool unclippedDepth = false;
            PolygonMode polygonMode = PolygonMode::Fill;
        };

        struct ShaderState {
            ShaderModule module;
            std::string entryPoint = "main";
        };

        struct VertexState {
            std::vector<BufferState> buffers;
        };

        std::unordered_map<graphics::ShaderStage, ShaderStage> m_shader_stages;
        VertexState vertex;
        std::optional<DepthStencilState> depthStencil;
        std::vector<BlendState> blend_state;
        MultisampleState multisample;
        PrimitiveState primitive;
        PipelineLayout layout;
        RenderPass m_render_pass;
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