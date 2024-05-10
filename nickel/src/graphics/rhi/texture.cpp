#include "graphics/rhi/texture.hpp"
#include "graphics/rhi/gl4/texture.hpp"
#include "graphics/rhi/gl4/adapter.hpp"
#include "graphics/rhi/gl4/device.hpp"
#include "graphics/rhi/null/texture.hpp"
#ifdef NICKEL_HAS_VULKAN
#include "graphics/rhi/vk/device.hpp"
#include "graphics/rhi/vk/texture.hpp"
#endif

namespace nickel::rhi {

Texture::Texture(AdapterImpl& adapter, rhi::DeviceImpl& dev,
                 const Descriptor& desc,
                 const std::vector<uint32_t>& queueIndices) {
    switch (adapter.RequestAdapterInfo().api) {
        case APIPreference::Undefine:
            break;
        case APIPreference::GL:
            impl_ =
                new gl4::TextureImpl(static_cast<gl4::DeviceImpl&>(dev), desc);
            break;
        case APIPreference::Vulkan:
#ifdef NICKEL_HAS_VULKAN
            impl_ = new vulkan::TextureImpl(
                static_cast<vulkan::AdapterImpl&>(adapter),
                static_cast<vulkan::DeviceImpl&>(dev), desc, queueIndices);
#endif
            break;
        case APIPreference::Null:
            impl_ = new null::TextureImpl{};
            break;
    }
}

Texture::Texture(TextureImpl* impl) : impl_{impl} {}

void Texture::Destroy() {
    if (impl_) {
        delete impl_;
        impl_ = nullptr;
    }
}

TextureType Texture::Dimension() const {
    return impl_->Dimension();
}

Extent3D Texture::Extent() const {
    return impl_->Extent();
}

enum TextureFormat Texture::Format() const {
    return impl_->Format();
}

uint32_t Texture::MipLevelCount() const {
    return impl_->MipLevelCount();
}

SampleCount Texture::SampleCount() const {
    return impl_->SampleCount();
}

TextureUsage Texture::Usage() const {
    return impl_->Usage();
}

TextureView Texture::CreateView(const TextureView::Descriptor& desc) {
    return impl_->CreateView(desc);
}

const Texture::Descriptor& Texture::GetDescriptor() const {
    return impl_->Descriptor();
}

}  // namespace nickel::rhi