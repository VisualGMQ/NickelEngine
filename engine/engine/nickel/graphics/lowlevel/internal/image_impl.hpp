#pragma once
#include "nickel/common/flags.hpp"
#include "nickel/common/math/smatrix.hpp"
#include "nickel/graphics/lowlevel/image.hpp"
#include "nickel/common/memory/refcountable.hpp"
#include "nickel/internal/pch.hpp"

namespace nickel::graphics {

class Memory;
class AdapterImpl;
class DeviceImpl;
class MemoryImpl;

class ImageImpl : public RefCountable {
public:
    ImageImpl(const AdapterImpl&, DeviceImpl&, const Image::Descriptor&);
    ImageImpl(const ImageImpl&) = delete;
    ImageImpl(ImageImpl&&) = delete;
    ImageImpl& operator=(const ImageImpl&) = delete;
    ImageImpl& operator=(ImageImpl&&) = delete;

    ~ImageImpl();

    VkImageType Type() const;
    SVector<uint32_t, 3> Extent() const;
    VkFormat Format() const;
    uint32_t MipLevelCount() const;
    VkSampleCountFlags SampleCount() const;
    Flags<VkImageUsageFlagBits> Usage() const;
    ImageView CreateView(const Image& image, const ImageView::Descriptor&);

    void DecRefcount() override;

    VkImage m_image = VK_NULL_HANDLE;
    MemoryImpl* m_memory{};
    std::vector<ImageLayout> m_layouts;

private:
    DeviceImpl& m_device;
    VkImageCreateInfo m_create_info;

    void createImage(const Image::Descriptor&, DeviceImpl&);
    void allocMem(VkPhysicalDevice phyDevice);
    void findSupportedFormat(VkFormat candidates, VkImageTiling tiling,
                             VkFormatFeatureFlags features);
};

}  // namespace nickel::graphics