#pragma once

#include "graphics/rhi/buffer.hpp"
#include "graphics/rhi/common.hpp"
#include "graphics/rhi/sampler.hpp"
#include "graphics/rhi/texture_view.hpp"
#include <vector>
#include <variant>


namespace nickel::rhi {

enum class BufferType {
    ReadOnlyStorage,
    Storage,
    Uniform,
};

struct BufferBinding final {
    Buffer buffer;
    bool hasDynamicOffset = false;
    uint64_t minBindingSize = 0;
    BufferType type = BufferType::Uniform;
};

struct SamplerBinding final {
    enum class SamplerType {
        Comparision,
        Filtering,
        NonFiltering,
    } type = SamplerType::Filtering;
    Sampler sampler;
};

struct StorageTextureBinding final {
    enum class StorageTextureAccess {
        Undefined,
        WriteOnly,
    } access = StorageTextureAccess::Undefined;
    TextureView view;
    Format format;
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
    TextureView view;
    bool multisampled = false;
    TextureViewType viewDimension = TextureViewType::Dim2;
};

using ResourceLayout = std::variant<BufferBinding, SamplerBinding,
                                    StorageTextureBinding, TextureBinding>;

struct Entry final {
    uint32_t binding;
    ShaderStage visibility;
    ResourceLayout resourceLayout;
    uint32_t arraySize = 1;
};

class BindGroupLayoutImpl;

class BindGroupLayout final {
public:
    friend class PipelineLayout;

    struct Descriptor final {
        std::vector<Entry> entries;
    };

    BindGroupLayout(APIPreference api, DeviceImpl&, const Descriptor&);
    void Destroy();

    auto Impl() const { return impl_; }
    auto Impl() { return impl_; }

private:
    BindGroupLayoutImpl* impl_{};
};

class BindGroupImpl;

class BindGroup final {
public:
    struct Descriptor {
        std::vector<Entry> entries;
        BindGroupLayout layout;
    };

    BindGroup(APIPreference, DeviceImpl&, const Descriptor&);
    BindGroupLayout GetLayout() const;
    auto Impl() const { return impl_; }
    auto Impl() { return impl_; }
    void Destroy();

private:
    BindGroupImpl* impl_{};
};

}  // namespace nickel::rhi