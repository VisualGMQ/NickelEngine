#pragma once

#include "graphics/rhi/impl/texture_view.hpp"
#include "graphics/rhi/texture.hpp"
#include "graphics/rhi/texture_view.hpp"
#include "graphics/rhi/vk/convert.hpp"
#include "graphics/rhi/vk/pch.hpp"
#include "graphics/rhi/vk/util.hpp"


namespace nickel::rhi::vulkan {

vk::Flags<vk::ImageAspectFlagBits> DetermineTextureAspect(TextureAspect aspect,
                                                          Format format);

class DeviceImpl;
class TextureImpl;

class TextureViewImpl : public rhi::TextureViewImpl {
public:
    TextureViewImpl(DeviceImpl&, TextureImpl&, const TextureView::Descriptor&);
    TextureViewImpl(DeviceImpl&, TextureImpl&, vk::ImageView);
    ~TextureViewImpl();

    vk::ImageView GetView() const;

private:
    vk::ImageView view_;
    DeviceImpl& dev_;
};

}  // namespace nickel::rhi::vulkan