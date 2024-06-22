#include "graphics/rhi/texture_view.hpp"
#include "graphics/rhi/null/texture.hpp"
#ifdef NICKEL_HAS_VULKAN
#include "graphics/rhi/vk/device.hpp"
#include "graphics/rhi/vk/texture.hpp"
#include "graphics/rhi/vk/texture_view.hpp"
#endif
#ifdef NICKEL_HAS_GLES3
#include "graphics/rhi/gl/texture.hpp"
#include "graphics/rhi/gl/texture_view.hpp"
#endif
#include "graphics/rhi/null/texture_view.hpp"


namespace nickel::rhi {

TextureView::TextureView(APIPreference api, DeviceImpl& dev,
                         TextureImpl& texture,
                         const TextureView::Descriptor& desc) {
    switch (api) {
        case APIPreference::Undefine:
            break;
        case APIPreference::GL:
#ifdef NICKEL_HAS_GLES3
            impl_ = new gl::TextureViewImpl(
                static_cast<gl::TextureImpl&>(texture), desc);
#endif
            break;
        case APIPreference::Vulkan:
#ifdef NICKEL_HAS_VULKAN
            impl_ = new vulkan::TextureViewImpl(
                static_cast<vulkan::DeviceImpl&>(dev),
                static_cast<vulkan::TextureImpl&>(texture), desc);
#endif
            break;
        case APIPreference::Null: {
            null::TextureImpl fakeImpl{};
            impl_ = new null::TextureViewImpl{fakeImpl};
        } break;
    }
}

TextureView::TextureView(TextureViewImpl* impl) : impl_{impl} {}

enum TextureFormat TextureView::Format() const {
    return impl_->Format();
}

::nickel::rhi::Texture TextureView::Texture() const {
    return impl_->Texture();
}

void TextureView::Destroy() {
    if (impl_) {
        delete impl_;
        impl_ = nullptr;
    }
}

}  // namespace nickel::rhi