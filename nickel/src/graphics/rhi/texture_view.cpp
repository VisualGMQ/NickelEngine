#include "graphics/rhi/texture_view.hpp"
#include "graphics/rhi/vk/device.hpp"
#include "graphics/rhi/vk/texture.hpp"
#include "graphics/rhi/vk/texture_view.hpp"

namespace nickel::rhi {

TextureView::TextureView(APIPreference api, DeviceImpl& dev,
                         TextureImpl& texture,
                         const TextureView::Descriptor& desc) {
    switch (api) {
        case APIPreference::Undefine:
            break;
        case APIPreference::GL:
            break;
        case APIPreference::Vulkan:
            impl_ = new vulkan::TextureViewImpl(
                static_cast<vulkan::DeviceImpl&>(dev),
                static_cast<vulkan::TextureImpl&>(texture), desc);
            break;
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