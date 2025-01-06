#pragma once
#include "nickel/graphics/buffer.hpp"
#include "nickel/graphics/image_view.hpp"
#include "nickel/graphics/sampler.hpp"

#include <map>
#include <variant>

namespace nickel::graphics {

class BindGroupImpl;

class BindGroup final {
public:
    struct BufferBinding final {
        enum class Type {
            Storage,
            Uniform,
            DynamicStorage,
            DynamicUniform,
        };
        Type type = Type::Uniform;
        Buffer buffer;
        std::optional<uint32_t> offset;
        std::optional<uint32_t> size;
    };

    struct SamplerBinding final {
        Sampler sampler;
    };

    struct CombinedSamplerBinding final {
        ImageView view;
        Sampler sampler;
    };

    struct ImageBinding final {
        enum class Type {
            Image,
            StorageImage,
        };
        Type type = Type::Image;
        ImageView view;
    };

    struct BindingPoint {
        uint32_t slot = 0;
        std::variant<BufferBinding, SamplerBinding, CombinedSamplerBinding,
                     ImageBinding>
            entry;
    };

    struct Entry final {
        BindingPoint binding;
        Flags<VkShaderStageFlagBits> shader_stage;
        uint32_t arraySize = 1;
    };

    struct Descriptor final {
        std::map<uint32_t, Entry> entries;
    };

    BindGroup() = default;
    explicit BindGroup(BindGroupImpl*);
    BindGroup(const BindGroup&);
    BindGroup(BindGroup&&) noexcept;
    BindGroup& operator=(const BindGroup&) noexcept;
    BindGroup& operator=(BindGroup&&) noexcept;
    ~BindGroup();
    
    operator bool() const noexcept;

    const BindGroupImpl& Impl() const noexcept;
    BindGroupImpl& Impl() noexcept;

private:
    BindGroupImpl* m_impl{};
};

}  // namespace nickel::graphics