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
    ImageImpl(const AdapterImpl&, DeviceImpl&, const Image::Descriptor&);
    ~ImageImpl();

    VkImageType Type() const;
    SVector<uint32_t, 3> Extent() const;
    VkFormat Format() const;
    uint32_t MipLevelCount() const;
    VkSampleCountFlags SampleCount() const;
    Flags<VkImageUsageFlagBits> Usage() const;
    ImageView CreateView(const Image& image, const ImageView::Descriptor&);

    VkImage m_image;
    MemoryImpl* m_memory{};
    std::vector<VkImageLayout> m_layouts;

private:
    DeviceImpl& m_device;
    VkImageCreateInfo m_create_info;

    void createImage(const Image::Descriptor&, DeviceImpl&);
    void allocMem(VkPhysicalDevice phyDevice);
};

}  // namespace nickel::graphics