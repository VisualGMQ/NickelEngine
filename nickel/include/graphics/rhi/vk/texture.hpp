#pragma once

#include "graphics/rhi/impl/texture.hpp"
#include "graphics/rhi/vk/pch.hpp"
#include "graphics/rhi/vk/util.hpp"
#include "graphics/rhi/adapter.hpp"

namespace nickel::rhi::vulkan {

class DeviceImpl;
class AdapterImpl;

class TextureImpl : public rhi::TextureImpl {
public:
    TextureImpl(AdapterImpl&, DeviceImpl&, const Texture::Descriptor& desc,
                const std::vector<uint32_t>& queueIndices);
    ~TextureImpl();

    TextureView CreateView(const TextureView::Descriptor&) override;

    vk::Image GetImage() const;

    vk::DeviceMemory mem;

private:
    vk::Image image_;
    DeviceImpl& dev_;

    void createImage(const Texture::Descriptor& desc,
                     const std::vector<uint32_t>& queueIndices);
    void allocMem(vk::PhysicalDevice phyDevice, enum Format, const Extent3D&);
};

}  // namespace nickel::rhi::vulkan