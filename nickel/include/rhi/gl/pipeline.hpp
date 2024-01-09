#pragma once

#include "rhi/gl/shader.hpp"
#include "rhi/pipeline.hpp"
#include "rhi/vertex_layout.hpp"

namespace nickel::rhi::gl {

class GraphicsPipeline : public rhi::GraphicsPipeline {
public:
    GraphicsPipeline(const VertexLayout& vertexLayout,
                     const InputAssemblyState& assemblyState,
                     const std::vector<ShaderModule*>& shaders,
                     const ViewportState& viewportState,
                     const RasterizationState& rasterState,
                     const DepthStencilState& depthStencilState,
                     const MultisampleState& multisampleState,
                     const ColorBlendState& colorBlendState);
    GraphicsPipeline(const GraphicsPipeline&) = delete;
    GraphicsPipeline& operator=(const GraphicsPipeline&) = delete;

    GraphicsPipeline(GraphicsPipeline&& o) { swap(o, *this); }

    GraphicsPipeline& operator=(GraphicsPipeline&& o) {
        if (&o != this) {
            swap(o, *this);
        }
        return *this;
    }

    void Apply();

private:
    VertexLayout vertexLayout_;
    InputAssemblyState assemblyState_;
    ViewportState viewportState_;
    RasterizationState rasterState_;
    DepthStencilState depthStencilState_;
    MultisampleState multisampleState_;
    ColorBlendState colorBlendState_;
    GLuint shader_ = 0;

    friend void swap(GraphicsPipeline& o1, GraphicsPipeline& o2) noexcept {
        using std::swap;
        swap(o1.vertexLayout_, o2.vertexLayout_);
        swap(o1.assemblyState_, o2.assemblyState_);
        swap(o1.shader_, o2.shader_);
        swap(o1.viewportState_, o2.viewportState_);
        swap(o1.rasterState_, o2.rasterState_);
        swap(o1.depthStencilState_, o2.depthStencilState_);
        swap(o1.multisampleState_, o2.multisampleState_);
        swap(o1.colorBlendState_, o2.colorBlendState_);
    }

    void createShader(const std::vector<ShaderModule*>& modules);
};

}  // namespace nickel::rhi::gl