#pragma once
#include "nickel/graphics/texture.hpp"
#include "nickel/graphics/lowlevel/device.hpp"

namespace nickel::graphics {
class TextureManagerImpl;

class TextureImpl {
public:
    TextureImpl(Device device, const std::string& filename,
         Format format);
    SVector<uint32_t, 2> Extent() const;

private:
    Image m_image;
    ImageView m_view;
};

}
