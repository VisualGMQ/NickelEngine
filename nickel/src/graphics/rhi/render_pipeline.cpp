#include "graphics/rhi/render_pipeline.hpp"
#include "graphics/rhi/common.hpp"
#ifdef NICKEL_HAS_VULKAN
#include "graphics/rhi/vk/device.hpp"
#include "graphics/rhi/vk/pipeline_layout.hpp"
#include "graphics/rhi/vk/render_pipeline.hpp"
#endif
#include "graphics/rhi/gl4/render_pipeline.hpp"
#include "graphics/rhi/null/render_pipeline.hpp"


namespace nickel::rhi {

RenderPipeline::RenderPipeline(APIPreference api, DeviceImpl& dev,
                               const Descriptor& desc) {
    switch (api) {
        case APIPreference::Undefine:
            break;
        case APIPreference::GL:
            impl_ = new gl4::RenderPipelineImpl(desc);
            break;
        case APIPreference::Vulkan:
#ifdef NICKEL_HAS_VULKAN
            impl_ = new vulkan::RenderPipelineImpl(
                static_cast<vulkan::DeviceImpl&>(dev), desc);
#endif
            break;
        case APIPreference::Null:
            impl_ = new null::RenderPipelineImpl{};
            break;
    }
}

PipelineLayout RenderPipeline::GetLayout() const {
    return impl_->GetLayout();
}

void RenderPipeline::Destroy() {
    if (impl_) {
        delete impl_;
        impl_ = nullptr;
    }
}

}  // namespace nickel::rhi