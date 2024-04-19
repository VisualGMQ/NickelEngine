#pragma once

#include "graphics/rhi/buffer.hpp"
#include "graphics/rhi/texture.hpp"
#include "graphics/rhi/texture_view.hpp"

namespace nickel::rhi {

class RenderPassImpl;
class DeviceImpl;

class RenderPass final {
public:
    struct Descriptor {
        struct ColorAttachment {
            std::array<float, 4> clearValue;
            AttachmentLoadOp loadOp = AttachmentLoadOp::Clear;
            AttachmentStoreOp storeOp = AttachmentStoreOp::Store;
            std::optional<TextureView> resolveTarget;
            TextureView view;
        };

        struct DepthStencilAttachment {
            float depthClearValue;
            uint32_t stencilClearValue;
            AttachmentLoadOp depthLoadOp = AttachmentLoadOp::Clear;
            AttachmentStoreOp depthStoreOp = AttachmentStoreOp::Discard;
            bool depthReadOnly = false;
            AttachmentLoadOp stencilLoadOp = AttachmentLoadOp::Clear;
            AttachmentStoreOp stencilStoreOp = AttachmentStoreOp::Discard;
            bool stencilReadOnly = false;
            TextureView view;
        };

        std::vector<ColorAttachment> colorAttachments;
        std::optional<DepthStencilAttachment> depthStencilAttachment;
        std::optional<Rect2D> renderArea;
    };

    RenderPass() = default;
    RenderPass(APIPreference, DeviceImpl&, const Descriptor&);
    RenderPass(RenderPass&& o) { swap(o, *this); }
    RenderPass(const RenderPass& o) = default;
    RenderPass& operator=(const RenderPass& o) = default;

    RenderPass& operator=(RenderPass&& o) {
        if (&o != this) swap(o, *this);
        return *this;
    }
    auto& GetDescriptor() const { return desc_; }
    void Destroy();

    auto Impl() const { return impl_; }
    auto Impl() { return impl_; }

    operator bool() const {
        return impl_ != nullptr;
    }

private:
    Descriptor desc_;
    RenderPassImpl* impl_{};

    friend void swap(RenderPass& o1, RenderPass& o2) noexcept {
        using std::swap;

        swap(o1.impl_, o2.impl_);
        swap(o1.desc_, o2.desc_);
    }
};

}  // namespace nickel::rhi