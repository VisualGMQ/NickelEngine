#include "graphics/rhi/vk/texture_view.hpp"
#include "graphics/rhi/vk/device.hpp"
#include "graphics/rhi/vk/texture.hpp"

namespace nickel::rhi::vulkan {

vk::Flags<vk::ImageAspectFlagBits> DetermineTextureAspectByFormat(TextureFormat format) {
    if (format ==TextureFormat::DEPTH16_UNORM || format ==TextureFormat::DEPTH24_PLUS ||
        format ==TextureFormat::DEPTH32_FLOAT) {
        return vk::ImageAspectFlagBits::eDepth;
    } else if (format ==TextureFormat::DEPTH32_FLOAT_STENCIL8 ||
                format ==TextureFormat::DEPTH24_PLUS_STENCIL8) {
        return vk::ImageAspectFlagBits::eDepth |
                vk::ImageAspectFlagBits::eStencil;
    } else if (format ==TextureFormat::STENCIL8) {
        return vk::ImageAspectFlagBits::eStencil;
    } else {
        return vk::ImageAspectFlagBits::eColor;
    }
}

TextureViewImpl::TextureViewImpl(DeviceImpl& dev, TextureImpl& texture,
                                 const TextureView::Descriptor& desc)
    : rhi::TextureViewImpl(desc.format ? desc.format.value() : texture.Format(),
                           texture),
      dev_{dev} {

    if (texture.Format() != TextureFormat::Presentation) {
        vk::ImageViewCreateInfo info;
        vk::ImageSubresourceRange range;
        enum TextureFormat format =
            desc.format ? desc.format.value() : texture.Format();
        vk::Flags<vk::ImageAspectFlagBits> aspect;
        if (desc.aspect == TextureAspect::Unknown) {
            aspect = DetermineTextureAspectByFormat(format);
        } else {
            aspect = TextureAspect2Vk(desc.aspect);
        }

        TextureViewType dimension;

        if (!desc.dimension) {
            if (texture.Dimension() == TextureType::Dim1) {
                dimension = TextureViewType::Dim1;
            } else if (texture.Dimension() == TextureType::Dim2 &&
                       texture.Extent().depthOrArrayLayers == 1) {
                dimension = TextureViewType::Dim2;
            } else if (texture.Dimension() == TextureType::Dim2 &&
                       texture.Extent().depthOrArrayLayers > 1) {
                dimension = TextureViewType::Dim2Array;
            } else if (texture.Dimension() == TextureType::Dim3) {
                dimension = TextureViewType::Dim3;
            }
        } else {
            dimension = desc.dimension.value();
        }
        uint32_t arrayLayerCount = 0;
        if (!desc.arrayLayerCount) {
            if (dimension == TextureViewType::Dim1 ||
                dimension == TextureViewType::Dim2 ||
                dimension == TextureViewType::Dim3) {
                arrayLayerCount = 1;
            } else if (dimension == TextureViewType::Cube) {
                arrayLayerCount = 6;
            } else if (dimension == TextureViewType::Dim2Array ||
                       dimension == TextureViewType::CubeArray) {
                arrayLayerCount = texture.Extent().depthOrArrayLayers;
            }
        }
        range.setAspectMask(aspect)
            .setBaseArrayLayer(desc.baseArrayLayer)
            .setBaseMipLevel(desc.baseMipLevel)
            .setLayerCount(arrayLayerCount)
            .setLevelCount(desc.mipLevelCount ? desc.mipLevelCount.value()
                                              : texture.MipLevelCount());
        info.setImage(texture.GetImage())
            .setViewType(TextureViewType2Vk(dimension))
            .setFormat(TextureFormat2Vk(format))
            .setComponents({})
            .setSubresourceRange(range);

        VK_CALL(view_, dev_.device.createImageView(info));
    }
}

TextureViewImpl::TextureViewImpl(DeviceImpl& dev, TextureImpl& texture,
                                 vk::ImageView view)
    : rhi::TextureViewImpl{texture.Format(), texture}, dev_{dev}, view_{view} {}

TextureViewImpl::~TextureViewImpl() {
    if (view_) {
        dev_.device.destroyImageView(view_);
    }
}

vk::ImageView TextureViewImpl::GetView() const {
    return view_;
}

}  // namespace nickel::rhi::vulkan