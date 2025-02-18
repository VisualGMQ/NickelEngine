#pragma once
#include "nickel/graphics/lowlevel/enums.hpp"
#include "nickel/graphics/lowlevel/buffer.hpp"
#include "nickel/graphics/lowlevel/image.hpp"
#include "nickel/graphics/lowlevel/sampler.hpp"
#include "nickel/common/dllexport.hpp"

#include <map>
#include <variant>
#include <optional>

namespace nickel::graphics {

class BindGroupImpl;

class NICKEL_API BindGroup final {
public:
    struct BufferBinding final {
        enum class Type {
            Storage,
            Uniform,
            DynamicStorage,
            DynamicUniform,
        };
        Type m_type = Type::Uniform;
        Buffer m_buffer;
        std::optional<uint32_t> m_offset;
        std::optional<uint32_t> m_size;
    };

    struct SamplerBinding final {
        Sampler m_sampler;

        SamplerBinding() = default;
    };

    struct CombinedSamplerBinding final {
        ImageView m_view;
        Sampler m_sampler;
    };

    struct ImageBinding final {
        enum class Type {
            Image,
            StorageImage,
        };
        Type m_type = Type::Image;
        ImageView m_view;
    };

    struct BindingPoint {
        std::variant<SamplerBinding, BufferBinding, CombinedSamplerBinding,
                     ImageBinding>
            m_entry;
    };

    struct Entry final {
        BindingPoint m_binding;
        Flags<ShaderStage> m_shader_stage;
        uint32_t m_array_size = 1;
    };

    struct Descriptor final {
        std::map<uint32_t, Entry> m_entries;
    };

    BindGroup() = default;
    explicit BindGroup(BindGroupImpl*);
    BindGroup(BindGroup&&) noexcept;
    BindGroup& operator=(BindGroup&&) noexcept;
    ~BindGroup();

    const BindGroupImpl& Impl() const noexcept;
    BindGroupImpl& Impl() noexcept;
    
    operator bool() const noexcept;

    const Descriptor& GetDescriptor() const;

private:
    BindGroupImpl* m_impl{};
};

}  // namespace nickel::graphics