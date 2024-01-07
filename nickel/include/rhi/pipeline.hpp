#pragma once

#include "pch.hpp"
#include "rhi/enums.hpp"


namespace nickel::rhi {

class Pipeline {
public:
    enum class Type {
        Graphics,
        Compute,
    };

    explicit Pipeline(Type type) : type_(type) {}
    virtual ~Pipeline() = default;

    auto Type() const { return type_; }

private:
    enum Type type_;
};

struct InputAssemblyState final {
    PrimitiveType primitive = PrimitiveType::Triangles;
    bool primitiveRestartEnable = false;;
};

struct Viewport final {
    float x, y, width, height, minDepth, maxDepth;
};

struct Sissor final {
    int32_t x, y;
    uint32_t width, height;
};

struct ViewportState final {
    std::vector<Viewport> viewports;
    std::vector<Sissor> sissors;
};

struct RasterizationState {
    bool depthClampEnable = false;
    bool discardEnable = false;
    PolygonMode polygonMode = PolygonMode::Fill;
    CullMode cullMode = CullMode::None;
    FrontFace frontFace = FrontFace::CCW;
    bool depthBiasEnable = false;
    float depthBiasConstantFactor = 0;
    float depthBiasClamp = 0;
    float depthBiasSlopeFactor = 0;
    float lineWidth = 1.0;
};

struct MultisampleState {
    SampleCountFlag sampleCount = SampleCountFlag::e1;
    bool sampleShadingEnable = false;
    float minSampleShading = 1.0;
    bool alphaToCoverageEnable = false;
    bool alphaToOneEnable = false;
    std::vector<uint32_t> sampleMasks = {0xFFFFFFFF};
};

struct StencilOp final {
    StencilOpEnum onFail = StencilOpEnum::Zero;
    StencilOpEnum onPass = StencilOpEnum::Zero;
    StencilOpEnum onDepthFail = StencilOpEnum::Zero;
    CompareOp compare = CompareOp::Always;
    uint32_t compareMask = 0xFFFFFFFF;
    uint32_t writeMask = 0xFFFFFFFF;
    uint32_t reference = 0xFFFFFFFF;
};

struct DepthStencilState final {
    bool depthTestEnable = false;
    bool depthWriteEnable = false;
    CompareOp depthCompare = CompareOp::Always;
    bool depthBoundsTestEnable = false;
    bool stencilTestEnable = false;
    StencilOp front = {};
    StencilOp back = {};
    float minDepthBound = 0.0;
    float maxDepthBound = 0.0;
};

struct ColorBlendAttachmentState {
    bool blendEnable = false;;
    BlendFactor srcColorBlendFactor = BlendFactor::Zero;
    BlendFactor dstColorBlendFactor = BlendFactor::Zero;
    BlendOp colorBlendOp = BlendOp::Add;
    BlendFactor srcAlphaBlendFactor = BlendFactor::Zero;
    BlendFactor dstAlphaBlendFactor = BlendFactor::Zero;
    BlendOp alphaBlendOp = BlendOp::Add;
    ColorComponentFlags colorWriteMask =
        static_cast<ColorComponentFlags>(ColorComponentFlagBits::A) |
        static_cast<ColorComponentFlags>(ColorComponentFlagBits::R) |
        static_cast<ColorComponentFlags>(ColorComponentFlagBits::G) |
        static_cast<ColorComponentFlags>(ColorComponentFlagBits::B);
};

struct ColorBlendState final {
    bool logicOpEnable = false;
    LogicOp logicOp = LogicOp::NoOp;
    std::array<float, 4> blendConstants = {0, 0, 0, 0};
    std::vector<ColorBlendAttachmentState> attachmentStates;
};

class PipelineLayout {
public:
    virtual ~PipelineLayout() = default;
};

class GraphicsPipeline : public Pipeline {
public:
    GraphicsPipeline() : Pipeline{Pipeline::Type::Graphics} {}
};

}  // namespace nickel::rhi