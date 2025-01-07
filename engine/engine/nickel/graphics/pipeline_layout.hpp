#pragma once
#include "nickel/graphics/bind_group_layout.hpp"
#include "nickel/common/dllexport.hpp"

namespace nickel::graphics {

class PipelineLayoutImpl;

class NICKEL_API PipelineLayout {
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
    void Release();

private:
    PipelineLayoutImpl* m_impl{};
};

}  // namespace nickel::graphics