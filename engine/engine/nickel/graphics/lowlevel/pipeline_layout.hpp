#pragma once
#include "nickel/graphics/lowlevel/bind_group_layout.hpp"
#include "nickel/common/dllexport.hpp"

namespace nickel::graphics {

class PipelineLayoutImpl;

class NICKEL_API PipelineLayout: public ImplWrapper<PipelineLayoutImpl> {
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

    using ImplWrapper::ImplWrapper;
};

}  // namespace nickel::graphics