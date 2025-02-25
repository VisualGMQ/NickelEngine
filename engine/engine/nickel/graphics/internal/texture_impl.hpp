#pragma once
#include "nickel/fs/path.hpp"
#include "nickel/graphics/lowlevel/device.hpp"

namespace nickel::graphics {

class TextureManagerImpl;

class TextureImpl: public RefCountable {
public:
    TextureImpl(TextureManagerImpl* mgr, Device device, const Path& filename,
         Format format);
    SVector<uint32_t, 2> Extent() const;

    TextureImpl(const TextureImpl&) = delete;
    TextureImpl(TextureImpl&&) = delete;
    TextureImpl& operator=(const TextureImpl&) = delete;
    TextureImpl& operator=(TextureImpl&&) = delete;

    void DecRefcount() override;

    Image m_image;
    ImageView m_view;

private:
    TextureManagerImpl* m_mgr;
};

}
