#pragma once
#include "nickel/common/dllexport.hpp"
#include "nickel/common/flags.hpp"
#include "nickel/graphics/lowlevel/bind_group.hpp"
#include "nickel/graphics/lowlevel/enums.hpp"

namespace nickel::graphics {

class BindGroupLayoutImpl;

class NICKEL_API BindGroupLayout final: public ImplWrapper<BindGroupLayoutImpl> {
public:
    struct Entry final {
        BindGroupEntryType m_type;
        Flags<ShaderStage> m_shader_stage;
        uint32_t m_array_size = 1;
    };

    struct Descriptor final {
        std::map<uint32_t, Entry> m_entries;
    };

    using ImplWrapper::ImplWrapper;

    BindGroup RequireBindGroup(const BindGroup::Descriptor& desc);
};

}  // namespace nickel::graphics