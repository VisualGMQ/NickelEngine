#include "graphics/rhi/renderpass.hpp"
#ifdef NICKEL_HAS_VULKAN
#include "graphics/rhi/vk/device.hpp"
#include "graphics/rhi/vk/renderpass.hpp"
#endif
#ifdef NICKEL_HAS_GLES3
#include "graphics/rhi/gl/renderpass.hpp"
#endif
#include "graphics/rhi/null/renderpass.hpp"


namespace nickel::rhi {

RenderPass::RenderPass(RenderPassImpl* impl) : impl_{impl} {}

const RenderPass::Descriptor& RenderPass::GetDescriptor() const {
    return impl_->GetDescriptor();
}

void RenderPass::Destroy() {
    if (impl_) {
        delete impl_;
        impl_ = nullptr;
    }
}

}  // namespace nickel::rhi