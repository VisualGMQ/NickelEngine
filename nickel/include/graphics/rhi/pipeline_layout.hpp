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
    void Destroy();

    auto Impl() const { return impl_; }

private:
    PipelineLayoutImpl* impl_{};
};

}