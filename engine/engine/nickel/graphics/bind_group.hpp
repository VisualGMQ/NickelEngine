#pragma once
#include "nickel/graphics/enums.hpp"
#include "nickel/graphics/buffer.hpp"
#include "nickel/graphics/image.hpp"
#include "nickel/graphics/sampler.hpp"
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
        std::variant<BufferBinding, SamplerBinding, CombinedSamplerBinding,
                     ImageBinding>
            entry;
    };

    struct Entry final {
        BindingPoint binding;
        Flags<ShaderStage> shader_stage;
        uint32_t arraySize = 1;
    };

    struct Descriptor final {
        std::map<uint32_t, Entry> entries;
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