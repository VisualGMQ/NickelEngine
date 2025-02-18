#pragma once
#include "nickel/graphics/lowlevel/bind_group_layout.hpp"
#include "nickel/common/dllexport.hpp"

namespace nickel::graphics {

class PipelineLayoutImpl;

class NICKEL_API PipelineLayout {
public:
    struct Descriptor {
        struct PushConstantRange {
            Flags<ShaderStage> m_shader_stage;
            uint32_t m_offset{};
            uint32_t m_size{};
        };
        
        std::vector<BindGroupLayout> m_layouts;
        std::vector<PushConstantRange> m_push_contants;
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