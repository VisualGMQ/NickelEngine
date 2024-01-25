#pragma once

#include "pch.hpp"
#include "vulkan/shader.hpp"
#include "vulkan/vertex_layout.hpp"


namespace nickel::vulkan {

class Device;

class Pipeline {
public:
    virtual ~Pipeline() = default;
};

class GraphicsPipeline : public Pipeline {
public:
    GraphicsPipeline(
        Device*, const VertexLayout& vertexLayout,
        const vk::PipelineInputAssemblyStateCreateInfo& inputAsm,
        const std::vector<ShaderModule*>& shaders,
        const vk::PipelineViewportStateCreateInfo& viewport,
        const vk::PipelineRasterizationStateCreateInfo& raster,
        const vk::PipelineDepthStencilStateCreateInfo& depthStencil,
        const vk::PipelineMultisampleStateCreateInfo& multisample,
        const vk::PipelineColorBlendStateCreateInfo& colorBlend,
        vk::PipelineLayout layout, vk::RenderPass renderPass);

    GraphicsPipeline(const GraphicsPipeline&) = delete;
    GraphicsPipeline& operator=(const GraphicsPipeline&) = delete;

    GraphicsPipeline(GraphicsPipeline&& o) { swap(o, *this); }

    GraphicsPipeline& operator=(GraphicsPipeline&& o) {
        if (&o != this) {
            swap(o, *this);
        }
        return *this;
    }

    ~GraphicsPipeline();

    operator vk::Pipeline() const { return pipeline_; }

    operator vk::Pipeline() { return pipeline_; }

private:
    Device* device_{};
    vk::Pipeline pipeline_;

    friend void swap(GraphicsPipeline& o1, GraphicsPipeline& o2) noexcept {
        using std::swap;

        swap(o1.device_, o2.device_);
        swap(o1.pipeline_, o2.pipeline_);
    }
};

class PipelineLayout final {
public:
    PipelineLayout(Device* device,
                   const std::vector<vk::DescriptorSetLayout>& layouts,
                   const std::vector<vk::PushConstantRange>& pushConstantRange);
    PipelineLayout(const PipelineLayout&) = delete;
    PipelineLayout& operator=(const PipelineLayout&) = delete;

    PipelineLayout(PipelineLayout&& o) { swap(o, *this); }

    PipelineLayout& operator=(PipelineLayout&& o) {
        if (&o != this) {
            swap(o, *this);
        }
        return *this;
    }

    ~PipelineLayout();

    operator vk::PipelineLayout() { return layout_; }

    operator vk::PipelineLayout() const { return layout_; }

    std::vector<vk::DescriptorSet> AllocSet(
        uint32_t count, const std::vector<vk::DescriptorSetLayout>& layouts);

private:
    Device* device_{};
    vk::PipelineLayout layout_;

    friend void swap(PipelineLayout& o1, PipelineLayout& o2) noexcept {
        using std::swap;

        swap(o1.device_, o2.device_);
        swap(o1.layout_, o2.layout_);
    }
};


}  // namespace nickel::vulkan