#pragma once

#include "rhi/pipeline.hpp"
#include "rhi/vertex_layout.hpp"
#include "rhi/vulkan/descriptor.hpp"
#include "rhi/vulkan/shader.hpp"

namespace nickel::rhi::vulkan {

class Device;

std::vector<vk::VertexInputAttributeDescription> CvtLayout2AttribDescription(
    const VertexLayout& layout);

std::vector<vk::VertexInputBindingDescription> CvtLayout2BindingDescription(
    const VertexLayout& layout);

inline vk::StencilOpState StencilOp2Vk(const StencilOp& op) {
    vk::StencilOpState state;
    state.setFailOp(StencilOpEnum2Vk(op.onFail))
        .setDepthFailOp(StencilOpEnum2Vk(op.onDepthFail))
        .setPassOp(StencilOpEnum2Vk(op.onDepthPass))
        .setCompareMask(op.compareMask)
        .setWriteMask(op.writeMask)
        .setReference(op.reference);
    return state;
}

class Device;

class PipelineLayout : public rhi::PipelineLayout {
public:
    PipelineLayout(Device* device,
                   const std::vector<DescriptorSetLayout*>& layouts,
                   std::optional<PushConstantRange> pushConstantRange = {});
    PipelineLayout(const PipelineLayout&) = delete;
    PipelineLayout& operator=(const PipelineLayout&) = delete;

    PipelineLayout(PipelineLayout&& o) { swap(*this, o); }

    PipelineLayout& operator=(PipelineLayout&& o) {
        if (&o != this) {
            swap(*this, o);
        }
        return *this;
    }

    ~PipelineLayout();

    auto& Raw() { return layout_; }

    auto& Raw() const { return layout_; }

private:
    Device* device_;
    vk::PipelineLayout layout_;

    friend void swap(PipelineLayout& o1, PipelineLayout& o2) {
        using std::swap;
        swap(o1.device_, o2.device_);
        swap(o1.layout_, o2.layout_);
    }
};

class GraphicsPipeline : public rhi::GraphicsPipeline {
public:
    GraphicsPipeline(const VertexLayout& vertexLayout,
                     const InputAssemblyState& assemblyState,
                     const std::vector<ShaderModule*>& shaders,
                     const ViewportState& viewportState,
                     const RasterizationState& rasterState,
                     const DepthStencilState& depthStencilState,
                     const MultisampleState& multisampleState,
                     const ColorBlendState& colorBlendState,
                     const PipelineLayout& layout);
    GraphicsPipeline(const GraphicsPipeline&) = delete;
    GraphicsPipeline& operator=(const GraphicsPipeline&) = delete;

    GraphicsPipeline(GraphicsPipeline&& o) { swap(o, *this); }

    GraphicsPipeline& operator=(GraphicsPipeline&& o) {
        if (&o != this) {
            swap(*this, o);
        }
        return *this;
    }

    ~GraphicsPipeline();

    auto& Raw() const { return pipeline_; }

private:
    vk::Pipeline pipeline_;
    Device* device_ = nullptr;

    friend void swap(GraphicsPipeline& o1, GraphicsPipeline& o2) {
        using std::swap;
        swap(o1.pipeline_, o2.pipeline_);
        swap(o1.device_, o2.device_);
    }
};

}  // namespace nickel::rhi::vulkan