#pragma once
#include "graphics/rhi/bind_group.hpp"
#include "graphics/rhi/common.hpp"

namespace nickel::rhi {

class PipelineLayoutImpl;

struct PushConstantRange final {
    uint64_t offset {};
    uint64_t size {};
    Flags<ShaderStage> stage;
};

class PipelineLayout final {
public:
    struct Descriptor final {
        std::vector<BindGroupLayout> layouts;
        std::vector<PushConstantRange> pushConstRanges;
    };

    PipelineLayout() = default;
    PipelineLayout(APIPreference, DeviceImpl&, const Descriptor&);
    PipelineLayout(const PipelineLayout& o) = default;
    PipelineLayout& operator=(const PipelineLayout& o) = default;

    PipelineLayout(PipelineLayout&& o) noexcept { swap(o, *this); }

    PipelineLayout& operator=(PipelineLayout&& o) noexcept {
        if (&o != this) swap(o, *this);
        return *this;
    }

    void Destroy();

    auto Impl() const { return impl_; }

private:
    PipelineLayoutImpl* impl_{};

    friend void swap(PipelineLayout& o1, PipelineLayout& o2) noexcept {
        using std::swap;

        swap(o1.impl_, o2.impl_);
    }
};

}  // namespace nickel::rhi