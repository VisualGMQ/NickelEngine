#include "nickel/graphics/internal/texture_impl.hpp"

#include "nickel/graphics/internal/texture_manager_impl.hpp"
#include "nickel/graphics/lowlevel/common.hpp"
#include "nickel/graphics/lowlevel/internal/image_impl.hpp"

namespace nickel::graphics {
TextureImpl::TextureImpl(Device device,
                         const Path& filename, Format format) {
    ImageRawData raw_data{filename};
    if (!raw_data) {
        return;
    }

    {
        Image::Descriptor desc;
        desc.imageType = ImageType::Dim2;
        desc.extent.w = raw_data.GetExtent().w;
        desc.extent.h = raw_data.GetExtent().h;
        desc.extent.l = 1;
        desc.format = format;
        desc.usage = Flags{ImageUsage::CopyDst} | ImageUsage::Sampled;
        m_image = device.CreateImage(desc);
    }

    // buffer data to image
    // TODO: optimize this: don't create buffer per image
    {
        Buffer::Descriptor desc;
        desc.m_memory_type = MemoryType::CPULocal;
        desc.m_size = 4 * raw_data.GetExtent().w * raw_data.GetExtent().h;
        desc.m_usage = BufferUsage::CopySrc;
        Buffer buffer = device.CreateBuffer(desc);
        buffer.MapAsync();
        void* data = buffer.GetMappedRange();
        memcpy(data, raw_data.GetData(), desc.m_size);
        buffer.Unmap();

        CommandEncoder encoder = device.CreateCommandEncoder();
        CopyEncoder copy = encoder.BeginCopy();
        CopyEncoder::BufferImageCopy copy_info;
        copy_info.bufferOffset = 0;
        copy_info.imageExtent.w = raw_data.GetExtent().w;
        copy_info.imageExtent.h = raw_data.GetExtent().h;
        copy_info.imageExtent.l = 1;
        copy_info.bufferImageHeight = 0;
        copy_info.bufferRowLength = 0;
        copy_info.imageSubresource.aspectMask = ImageAspect::Color;
        copy.CopyBufferToTexture(buffer, m_image, copy_info);
        copy.End();
        Command cmd = encoder.Finish();
        device.Submit(cmd, {}, {}, {});
        device.WaitIdle();
    }

    // create view
    {
        ImageView::Descriptor view_desc;
        view_desc.format = format;
        view_desc.components = ComponentMapping::SwizzleIdentity;
        view_desc.subresourceRange.aspectMask = ImageAspect::Color;
        view_desc.viewType = ImageViewType::Dim2;
        m_view = m_image.CreateView(view_desc);
    }
}

SVector<uint32_t, 2> TextureImpl::Extent() const {
    auto extent = m_image.Extent();
    return {extent.w, extent.h};
}
}
