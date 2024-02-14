#pragma once

#include "graphics/rhi/common.hpp"
#include "graphics/rhi/pipeline_layout.hpp"
#include "graphics/rhi/renderpass.hpp"
#include "graphics/rhi/shader.hpp"
#include <array>
#include <string>
#include <vector>

namespace nickel::rhi {

class DeviceImpl;

struct StencilOpState {
    CompareOp compare = CompareOp::Always;
    StencilOp depthFailOp = StencilOp::Keep;
    StencilOp failedOp = StencilOp::Keep;
    StencilOp passOp = StencilOp::Keep;
};

class RenderPipelineImpl;

class RenderPipeline {
public:
    struct DepthStencilState final {
        std::optional<float> depthBias;
        std::optional<float> depthBiasClamp;
        float depthBiasSlopeScale = 0;
        CompareOp depthCompare = CompareOp::Greater;
        bool depthWriteEnabled = false;
        TextureFormat depthFormat;
        StencilOpState stencilBack;
        StencilOpState stencilFront;
        uint32_t stencilReadMask = 0xFFFFFFFF;
        uint32_t stencilWriteMask = 0xFFFFFFFF;
    };

    struct BlendComponentState final {
        BlendFactor dstFactor = BlendFactor::eZero;
        BlendFactor srcFactor = BlendFactor::eOne;
        BlendOp operation = BlendOp::Add;
    };

    struct BlendState final {
        BlendComponentState alpha;
        BlendComponentState color;
    };

    struct FragmentTarget final {
        TextureFormat format;
        BlendState blend;
        ColorWriteMask writeMask = ColorWriteMask::All;
    };

    struct FragmentState final {
        std::string entryPoint = "main";
        ShaderModule module;
        std::vector<FragmentTarget> targets;
        TextureFormat format;
        FlagsConstant writeMask;
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

    struct VertexState final {
        std::string entryPoint = "main";
        ShaderModule module;
        std::vector<BufferState> buffers;
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

    struct ViewportState final {
        struct {
            float x, y, w, h;
        } viewport;

        Rect2D scissor;
    };

    struct Descriptor final {
        std::optional<DepthStencilState> depthStencil;
        VertexState vertex;
        FragmentState fragment;
        MultisampleState multisample;
        PrimitiveState primitive;
        PipelineLayout layout;
        ViewportState viewport;
    };

    RenderPipeline() = default;
    RenderPipeline(APIPreference api, DeviceImpl&, const Descriptor& desc);
    PipelineLayout GetLayout() const;

    auto Impl() const { return impl_; }

    auto Impl() { return impl_; }

    void Destroy();

private:
    RenderPipelineImpl* impl_{};
};

}  // namespace nickel::rhi