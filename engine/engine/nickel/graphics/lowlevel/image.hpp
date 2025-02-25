#pragma once
#include "nickel/common/dllexport.hpp"
#include "nickel/common/flags.hpp"
#include "nickel/common/impl_wrapper.hpp"
#include "nickel/common/math/smatrix.hpp"
#include "nickel/graphics/lowlevel/enums.hpp"

namespace nickel::graphics {

class ImageViewImpl;
class Image;

class NICKEL_API ImageView: public ImplWrapper<ImageViewImpl> {
public:
    struct Descriptor {
        struct ImageSubresourceRange {
            Flags<ImageAspect> m_aspect_mask = ImageAspect::None;
            uint32_t m_base_mipLevel = 0;
            uint32_t m_level_count = 1;
            uint32_t m_base_array_layer = 0;
            uint32_t m_layer_count = 1;
        };

        ImageViewType m_view_type;
        Format m_format;
        ComponentMapping m_components;
        ImageSubresourceRange m_subresource_range;
    };

    using ImplWrapper::ImplWrapper;

    Image GetImage() const;
};

class ImageImpl;

class NICKEL_API Image: public ImplWrapper<ImageImpl> {
public:
    struct Descriptor {
        bool m_is_cube_map = false;
        ImageType m_image_type = ImageType::Dim2;
        Format m_format = Format::R8G8B8A8_SRGB;
        SVector<uint32_t, 3> m_extent;
        uint32_t m_mip_levels = 1;
        uint32_t m_array_layers = 1;
        SampleCount m_samples = SampleCount::Count1;
        ImageTiling m_tiling = ImageTiling::Linear;
        Flags<ImageUsage> m_usage = ImageUsage::Sampled;
        SharingMode m_sharing_mode = SharingMode::Concurrent;
        ImageLayout m_initial_layout = ImageLayout::Undefined;
    };

    using ImplWrapper::ImplWrapper;

    ImageView CreateView(const ImageView::Descriptor&);
    
    SVector<uint32_t, 3> Extent() const;
    uint32_t MipLevelCount() const;
};

}  // namespace nickel::graphics