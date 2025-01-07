﻿#pragma once
#include "nickel/common/flags.hpp"
#include "volk.h"
#include "nickel/common/dllexport.hpp"

namespace nickel::graphics {

class BindGroupLayoutImpl;

class NICKEL_API BindGroupLayout final {
public:
    struct Entry final {
        VkDescriptorType type;
        Flags<VkShaderStageFlagBits> shader_stage;
        uint32_t arraySize = 1;
    };

    struct Descriptor final {
        std::unordered_map<uint32_t, Entry> entries;
    };
    
    explicit BindGroupLayout(BindGroupLayoutImpl*);
    BindGroupLayout(const BindGroupLayout&);
    BindGroupLayout(BindGroupLayout&&) noexcept;
    BindGroupLayout& operator=(const BindGroupLayout&) noexcept;
    BindGroupLayout& operator=(BindGroupLayout&&) noexcept;
    ~BindGroupLayout();

    const BindGroupLayoutImpl& Impl() const noexcept;
    BindGroupLayoutImpl& Impl() noexcept;

    void Release();

private:
    BindGroupLayoutImpl* m_impl{};
};

}  // namespace nickel::graphics