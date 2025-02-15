#pragma once
#include "nickel/fs/path.hpp"
#include "nickel/graphics/lowlevel/device.hpp"

namespace nickel::graphics {
class TextureManagerImpl;

class TextureImpl {
public:
    TextureImpl(Device device, const Path& filename,
         Format format);
    SVector<uint32_t, 2> Extent() const;

    Image m_image;
    ImageView m_view;
};

}
