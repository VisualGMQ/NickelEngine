#pragma once
#include "nickel/graphics/bind_group_layout.hpp"

namespace nickel::graphics {

class PipelineLayoutImpl;

class PipelineLayout {
public:
    struct Descriptor {
        std::vector<BindGroupLayout> layouts;
        std::vector<VkPushConstantRange> push_contants;
    };

    PipelineLayout() = default;
    explicit PipelineLayout(PipelineLayoutImpl*);
    PipelineLayout(const PipelineLayout&);
    PipelineLayout(PipelineLayout&&) noexcept;
    PipelineLayout& operator=(const PipelineLayout&) noexcept;
    PipelineLayout& operator=(PipelineLayout&&) noexcept;
    ~PipelineLayout();

    const PipelineLayoutImpl& Impl() const noexcept;
    PipelineLayoutImpl& Impl() noexcept;

    operator bool() const noexcept;

private:
    PipelineLayoutImpl* m_impl;
};

}  // namespace nickel::graphics