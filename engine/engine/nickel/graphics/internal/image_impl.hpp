#pragma once
#include "nickel/common/flags.hpp"
#include "nickel/common/math/smatrix.hpp"
#include "nickel/graphics/image.hpp"
#include "nickel/graphics/internal/refcountable.hpp"
#include "nickel/internal/pch.hpp"

namespace nickel::graphics {

class Memory;
class AdapterImpl;
class DeviceImpl;
class MemoryImpl;

class ImageImpl : public RefCountable {
public:
    ImageImpl(AdapterImpl&, DeviceImpl&, const Image::Descriptor&);
    ~ImageImpl();

    VkImageType Type() const;
    SVector<uint32_t, 3> Extent() const;
    VkFormat Format() const;
    uint32_t MipLevelCount() const;
    VkSampleCountFlags SampleCount() const;
    Flags<VkImageUsageFlagBits> Usage() const;
    // TextureView CreateView(const TextureView::Descriptor& = {});

    VkImage m_image;
    MemoryImpl* m_memory{};

private:
    VkDevice m_device;
    VkImageCreateInfo m_create_info;

    void createImage(const Image::Descriptor&, DeviceImpl&);
    void allocMem(VkPhysicalDevice phyDevice);
};

}  // namespace nickel::graphics