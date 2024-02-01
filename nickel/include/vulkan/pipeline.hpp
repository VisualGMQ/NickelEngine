#pragma once

#include "vulkan/pch.hpp"
#include "stdpch.hpp"
#include "vulkan/shader.hpp"
#include "vulkan/vertex_layout.hpp"

namespace nickel::vulkan {

class Device;

class Pipeline {
public:
    virtual ~Pipeline() = default;

    vk::Pipeline Raw() { return pipeline_; }

protected:
    vk::Pipeline pipeline_;
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

    friend void swap(GraphicsPipeline& o1, GraphicsPipeline& o2) noexcept {
        using std::swap;

        swap(o1.device_, o2.device_);
        swap(o1.pipeline_, o2.pipeline_);
    }
};

class PipelineLayoutUnitDescription final {
public:
    PipelineLayoutUnitDescription(vk::DescriptorType type,
                                  vk::ShaderStageFlagBits stage,
                                  std::vector<ShaderDataType>&& elems,
                                  uint32_t arraySize = 1)
        : type_{type},
          stage_{stage},
          elems_{std::move(elems)},
          arraySize_{arraySize} {}

    auto& Elems() const { return elems_; }

    auto ArraySize() const { return arraySize_; }

    auto Type() const { return type_; }

    auto Stage() const { return stage_; }

private:
    vk::DescriptorType type_;
    vk::ShaderStageFlagBits stage_;
    std::vector<ShaderDataType> elems_;
    uint32_t arraySize_ = 1;
};

class PipelineLayoutDescription final {
public:
    void AddBinding(uint32_t binding,
                    const PipelineLayoutUnitDescription& unit) {
        if (auto it = bindings_.find(binding); it != bindings_.end()) {
            LOGW(log_tag::Vulkan, "binding ", binding, " already exists");
        } else {
            bindings_.emplace(binding, unit);
        }
    }

    void SetPushConstant(uint32_t offset, uint32_t size,
                         vk::ShaderStageFlagBits stage) {
        pushConstsRange_.setOffset(offset).setSize(size).setStageFlags(stage);
    }

    std::vector<vk::DescriptorSetLayoutBinding> GetLayoutBindings() const {
        std::vector<vk::DescriptorSetLayoutBinding> bindings;
        for (auto&& [binding, unit] : bindings_) {
            bindings.emplace_back(getLayoutBinding(binding, unit));
        }
        return bindings;
    }

    std::vector<vk::DescriptorPoolSize> GetSizes(uint32_t frameCount) const {
        std::vector<vk::DescriptorPoolSize> sizes;
        for (auto&& [binding, unit] : bindings_) {
            vk::DescriptorPoolSize size;
            size.setType(unit.Type()).setDescriptorCount(frameCount);
            sizes.emplace_back(std::move(size));
        }
        return sizes;
    }

    uint32_t GetPoolRequireSize(uint32_t frameCount) const {
        return bindings_.size() * frameCount;
    }

    auto& GetPushConstsRange() const { return pushConstsRange_; }

private:
    std::unordered_map<uint32_t, PipelineLayoutUnitDescription> bindings_;
    vk::PushConstantRange pushConstsRange_;

    vk::DescriptorSetLayoutBinding getLayoutBinding(
        uint32_t bind, const PipelineLayoutUnitDescription& unit) const {
        vk::DescriptorSetLayoutBinding binding;
        binding.setBinding(bind)
            .setDescriptorCount(unit.ArraySize())
            .setDescriptorType(unit.Type())
            .setStageFlags(unit.Stage());
        return binding;
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