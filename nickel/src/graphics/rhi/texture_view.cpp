#include "graphics/rhi/texture_view.hpp"
#include "graphics/rhi/null/texture.hpp"
#ifdef NICKEL_HAS_VULKAN
#include "graphics/rhi/vk/device.hpp"
#include "graphics/rhi/vk/texture.hpp"
#include "graphics/rhi/vk/texture_view.hpp"
#endif
#include "graphics/rhi/gl4/texture.hpp"
#include "graphics/rhi/gl4/texture_view.hpp"
#include "graphics/rhi/null/texture_view.hpp"


namespace nickel::rhi {

TextureView::TextureView(APIPreference api, DeviceImpl& dev,
                         TextureImpl& texture,
                         const TextureView::Descriptor& desc) {
    switch (api) {
        case APIPreference::Undefine:
            break;
        case APIPreference::GL:
            impl_ = new gl4::TextureViewImpl(
                static_cast<gl4::TextureImpl&>(texture), desc);
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

class Texture TextureView::Texture() const {
    return impl_->Texture();
}

void TextureView::Destroy() {
    if (impl_) {
        delete impl_;
        impl_ = nullptr;
    }
}

}  // namespace nickel::rhi