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
        desc.m_image_type = ImageType::Dim2;
        desc.m_extent.w = raw_data.GetExtent().w;
        desc.m_extent.h = raw_data.GetExtent().h;
        desc.m_extent.l = 1;
        desc.m_format = format;
        desc.m_usage = Flags{ImageUsage::CopyDst} | ImageUsage::Sampled;
        m_image = device.CreateImage(desc);
    }

    // m_buffer data to image
    // TODO: optimize this: don't create m_buffer per image
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
        copy_info.m_buffer_offset = 0;
        copy_info.m_image_extent.w = raw_data.GetExtent().w;
        copy_info.m_image_extent.h = raw_data.GetExtent().h;
        copy_info.m_image_extent.l = 1;
        copy_info.m_buffer_image_height = 0;
        copy_info.m_buffer_row_length = 0;
        copy_info.m_image_subresource.m_aspect_mask = ImageAspect::Color;
        copy.CopyBufferToTexture(buffer, m_image, copy_info);
        copy.End();
        Command cmd = encoder.Finish();
        device.Submit(cmd, {}, {}, {});
        device.WaitIdle();
    }

    // create m_view
    {
        ImageView::Descriptor view_desc;
        view_desc.m_format = format;
        view_desc.m_components = ComponentMapping::SwizzleIdentity;
        view_desc.m_subresource_range.m_aspect_mask = ImageAspect::Color;
        view_desc.m_view_type = ImageViewType::Dim2;
        m_view = m_image.CreateView(view_desc);
    }
}

SVector<uint32_t, 2> TextureImpl::Extent() const {
    auto extent = m_image.Extent();
    return {extent.w, extent.h};
}
}
