#pragma once

#include "graphics/rhi/buffer.hpp"
#include "graphics/rhi/common.hpp"
#include "graphics/rhi/sampler.hpp"
#include "graphics/rhi/texture_view.hpp"
#include <vector>
#include <variant>
#include <string>


namespace nickel::rhi {

enum class BufferType {
    ReadOnlyStorage,
    Storage,
    Uniform,
};

struct BufferBinding final {
    Buffer buffer;
    bool hasDynamicOffset = false;
    std::optional<uint64_t> minBindingSize;
    BufferType type = BufferType::Uniform;
};

struct SamplerBinding final {
    enum class SamplerType {
        Comparision,
        Filtering,
        NonFiltering,
    } type = SamplerType::Filtering;
    std::string name;   // require for GL
    Sampler sampler;
    TextureView view;
};

struct StorageTextureBinding final {
    enum class StorageTextureAccess {
        Undefined,
        WriteOnly,
    } access = StorageTextureAccess::Undefined;
    TextureView view;
    VertexFormat format;
    TextureViewType viewDimension = TextureViewType::Dim2;
};

struct TextureBinding final {
    enum class SampleType {
        Depth,
        Float,
        Sint,
        Uint,
        UnfilterableFloat,
    } sampleType = SampleType::Float;
    std::string name;
    TextureView view;
    bool multisampled = false;
    TextureViewType viewDimension = TextureViewType::Dim2;
};

enum class BindingType {
    Buffer,
    Sampler,
    Texture,
    StorageTexture,
};

struct BindingPoint {
    uint32_t binding;
    std::variant<BufferBinding, SamplerBinding, StorageTextureBinding,
                 TextureBinding> entry;

};

struct Entry final {
    BindingPoint binding;
    Flags<ShaderStage> visibility;
    uint32_t arraySize = 1;
};

class BindGroupLayoutImpl;

class BindGroupLayout final {
public:
    friend class PipelineLayout;

    struct Descriptor final {
        std::vector<Entry> entries;
    };

    BindGroupLayout() = default;
    BindGroupLayout(APIPreference api, DeviceImpl&, const Descriptor&);
    void Destroy();

    operator bool() const { return impl_; }

    auto Impl() const { return impl_; }
    auto Impl() { return impl_; }

private:
    BindGroupLayoutImpl* impl_{};
};

class BindGroupImpl;

class BindGroup final {
public:
    struct Descriptor {
        std::vector<BindingPoint> entries;
        BindGroupLayout layout;
    };

    BindGroup() = default;
    BindGroup(APIPreference, DeviceImpl&, const Descriptor&);
    BindGroupLayout GetLayout() const;
    auto Impl() const { return impl_; }
    auto Impl() { return impl_; }
    void Destroy();

    operator bool() const { return impl_; }

private:
    BindGroupImpl* impl_{};
};

}  // namespace nickel::rhi