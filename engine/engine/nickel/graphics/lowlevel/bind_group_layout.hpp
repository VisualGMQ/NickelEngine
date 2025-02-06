#pragma once
#include "nickel/common/dllexport.hpp"
#include "nickel/common/flags.hpp"
#include "nickel/graphics/lowlevel/bind_group.hpp"
#include "nickel/graphics/lowlevel/enums.hpp"

namespace nickel::graphics {

class BindGroupLayoutImpl;

class NICKEL_API BindGroupLayout final {
public:
    struct Entry final {
        BindGroupEntryType type;
        Flags<ShaderStage> shader_stage;
        uint32_t arraySize = 1;
    };

    struct Descriptor final {
        std::unordered_map<uint32_t, Entry> entries;
    };

    BindGroupLayout() = default;
    explicit BindGroupLayout(BindGroupLayoutImpl*);
    BindGroupLayout(const BindGroupLayout&);
    BindGroupLayout(BindGroupLayout&&) noexcept;
    BindGroupLayout& operator=(const BindGroupLayout&) noexcept;
    BindGroupLayout& operator=(BindGroupLayout&&) noexcept;
    ~BindGroupLayout();

    const BindGroupLayoutImpl& Impl() const noexcept;
    BindGroupLayoutImpl& Impl() noexcept;

    BindGroup RequireBindGroup(const BindGroup::Descriptor& desc);

    void Release();

private:
    BindGroupLayoutImpl* m_impl{};
};

}  // namespace nickel::graphics