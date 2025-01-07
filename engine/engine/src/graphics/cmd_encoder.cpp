#include "nickel/graphics/cmd_encoder.hpp"

#include "nickel/graphics/framebuffer.hpp"
#include "nickel/graphics/internal/bind_group_impl.hpp"
#include "nickel/graphics/internal/buffer_impl.hpp"
#include "nickel/graphics/internal/cmd_impl.hpp"
#include "nickel/graphics/internal/framebuffer_impl.hpp"
#include "nickel/graphics/internal/graphics_pipeline_impl.hpp"
#include "nickel/graphics/internal/image_impl.hpp"
#include "nickel/graphics/internal/pipeline_layout_impl.hpp"
#include "nickel/graphics/internal/render_pass_impl.hpp"

namespace nickel::graphics {

struct RenderPassEncoder::ApplyRenderCmd {
    explicit ApplyRenderCmd(Command& cmd): m_cmd{cmd} {}

    void operator()(const BindGraphicsPipelineCmd& cmd) {
        vkCmdBindPipeline(m_cmd.Impl().m_cmd, VK_PIPELINE_BIND_POINT_GRAPHICS,
                          cmd.pipeline.Impl().m_pipeline);
        m_pipeline = &cmd.pipeline;
    }

    void operator()(const BindVertexBufferCmd& cmd) {
        VkDeviceSize device_size = cmd.offset;
        vkCmdBindVertexBuffers(m_cmd.Impl().m_cmd, cmd.slot, 1,
                               &cmd.buffer.Impl().m_buffer, &device_size);
    }

    void operator()(const BindIndexBufferCmd& cmd) {
        vkCmdBindIndexBuffer(m_cmd.Impl().m_cmd, cmd.buffer.Impl().m_buffer,
                             cmd.offset, cmd.index_type);
    }

    void operator()(const SetPushConstantCmd& cmd) {
        vkCmdPushConstants(m_cmd.Impl().m_cmd,
                           m_pipeline->Impl().m_layout.Impl().m_pipeline_layout,
                           cmd.stage, cmd.offset, cmd.size, cmd.data);
    }

    void operator()(const DrawCmd& cmd) {
        switch (cmd.m_type) {
            case DrawCmd::Type::Unknown:
                LOGE("unknown draw call tyep");
                break;
            case DrawCmd::Type::Vertices:
                vkCmdDrawIndexed(m_cmd.Impl().m_cmd, cmd.elem_count,
                                 cmd.instance_count, cmd.first_elem,
                                 cmd.vertex_offset, cmd.first_instance);
                break;
            case DrawCmd::Type::Indexed:
                vkCmdDraw(m_cmd.Impl().m_cmd, cmd.elem_count,
                          cmd.instance_count, cmd.first_elem,
                          cmd.first_instance);
                break;
        }
    }

    void operator()(const SetViewportCmd& cmd) {
        VkViewport viewport;
        viewport.x = cmd.viewport.x;
        viewport.y = cmd.viewport.y;
        viewport.width = cmd.viewport.width;
        viewport.height = cmd.viewport.height;
        viewport.minDepth = cmd.viewport.min_depth;
        viewport.maxDepth = cmd.viewport.max_depth;
        vkCmdSetViewport(m_cmd.Impl().m_cmd, 0, 1, &viewport);
    }

    void operator()(const SetBindGroupCmd& cmd) {
        auto& descriptor_sets = cmd.bind_group.Impl().m_set_lists;
        vkCmdBindDescriptorSets(
            m_cmd.Impl().m_cmd, VK_PIPELINE_BIND_POINT_GRAPHICS,
            m_pipeline->Impl().m_layout.Impl().m_pipeline_layout, 0,
            descriptor_sets.size(), descriptor_sets.data(),
            cmd.dynamic_offsets.size(), cmd.dynamic_offsets.data());
    }

private:
    Command& m_cmd;
    const GraphicsPipeline* m_pipeline{};
};

RenderPassEncoder::RenderPassEncoder(Command& cmd) : m_cmd{cmd} {}

void RenderPassEncoder::Draw(uint32_t vertex_count, uint32_t instance_count,
                             uint32_t first_vertex, uint32_t first_instance) {
    DrawCmd cmd;
    cmd.m_type = DrawCmd::Type::Vertices;
    cmd.elem_count = vertex_count;
    cmd.instance_count = instance_count;
    cmd.first_elem = first_vertex;
    cmd.first_instance = first_instance;
    m_record_cmds.push_back(cmd);
}

void RenderPassEncoder::DrawIndexed(uint32_t index_count,
                                    uint32_t instance_count,
                                    uint32_t first_index,
                                    uint32_t vertex_offset,
                                    uint32_t first_instance) {
    DrawCmd cmd;
    cmd.m_type = DrawCmd::Type::Vertices;
    cmd.elem_count = index_count;
    cmd.instance_count = instance_count;
    cmd.first_elem = first_index;
    cmd.first_instance = first_instance;
    cmd.vertex_offset = vertex_offset;
    m_record_cmds.push_back(cmd);
}

void RenderPassEncoder::BindVertexBuffer(uint32_t slot, Buffer buffer,
                                         uint64_t offset) {
    BindVertexBufferCmd cmd;
    cmd.slot = slot;
    cmd.buffer = buffer;
    cmd.offset = offset;
    m_record_cmds.push_back(cmd);
}

void RenderPassEncoder::BindIndexBuffer(Buffer buffer, VkIndexType type,
                                        uint64_t offset) {
    BindIndexBufferCmd cmd;
    cmd.index_type = type;
    cmd.buffer = buffer;
    cmd.offset = offset;
    m_record_cmds.push_back(cmd);
}

void RenderPassEncoder::SetBindGroup(const BindGroup& bind_group) {
    SetBindGroupCmd cmd;
    cmd.bind_group = bind_group;
    m_record_cmds.push_back(cmd);
}

void RenderPassEncoder::SetBindGroup(
    const BindGroup& bind_group, const std::vector<uint32_t>& dynamicOffset) {
    SetBindGroupCmd cmd;
    cmd.bind_group = bind_group;
    cmd.dynamic_offsets = dynamicOffset;
    m_record_cmds.push_back(cmd);

    auto& desc = cmd.bind_group.Impl().GetDescriptor();
    for (auto& [_, entry] : desc.entries) {
        auto& bind_entry = entry.binding.entry;
        ImageImpl* image_impl{};
        if (auto binding = std::get_if<BindGroup::ImageBinding>(&bind_entry)) {
            image_impl = &binding->view.GetImage().Impl();
        }
        if (auto binding = std::get_if<BindGroup::CombinedSamplerBinding>(&bind_entry)) {
            image_impl = &binding->view.GetImage().Impl();
        }

        if (!image_impl) {
            continue;
        }

        VkImageAspectFlags aspect = VK_IMAGE_ASPECT_COLOR_BIT;
        auto format = image_impl->Format();
        if (format == VK_FORMAT_D16_UNORM || format == VK_FORMAT_D32_SFLOAT) {
            aspect = VK_IMAGE_ASPECT_DEPTH_BIT;
        }
        if (format == VK_FORMAT_S8_UINT) {
            aspect = VK_IMAGE_ASPECT_STENCIL_BIT;
        }
        if (format == VK_FORMAT_D16_UNORM_S8_UINT ||
            format == VK_FORMAT_D24_UNORM_S8_UINT ||
            format == VK_FORMAT_D32_SFLOAT_S8_UINT) {
            aspect = VK_IMAGE_ASPECT_STENCIL_BIT | VK_IMAGE_ASPECT_DEPTH_BIT;
        }
        
        for (size_t i = 0; i < image_impl->m_layouts.size(); i++) {
            auto& layout = image_impl->m_layouts[i];
            if (layout != VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
                VkImageMemoryBarrier barrier{};
                barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
                barrier.image = image_impl->m_image;
                barrier.oldLayout = layout;
                barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                barrier.srcAccessMask = 0;
                barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
                barrier.subresourceRange.aspectMask = aspect;
                barrier.subresourceRange.layerCount = 1;
                barrier.subresourceRange.levelCount = 1;
                barrier.subresourceRange.baseArrayLayer = 0;
                barrier.subresourceRange.baseMipLevel = 0;
                vkCmdPipelineBarrier(m_cmd.Impl().m_cmd,
                                     VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                                     VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0,
                                     nullptr, 0, nullptr, 1, &barrier);
                m_cmd.Impl().AddLayoutTransition(
                    image_impl, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, i);
            }
        }
    }
}

void RenderPassEncoder::SetPipeline(const GraphicsPipeline& pipeline) {
    BindGraphicsPipelineCmd cmd;
    cmd.pipeline = pipeline;
    m_record_cmds.push_back(cmd);
}

void RenderPassEncoder::SetPushConstant(VkShaderStageFlags stage,
                                        const void* value, uint32_t offset,
                                        uint32_t size) {
    SetPushConstantCmd cmd;
    cmd.stage = stage;
    cmd.data = value;
    cmd.offset = offset;
    cmd.size = size;
    m_record_cmds.push_back(cmd);
}

void RenderPassEncoder::SetViewport(const Viewport& viewport) {
    SetViewportCmd cmd;
    cmd.viewport = viewport;
    m_record_cmds.push_back(cmd);
}

void RenderPassEncoder::End() {
    ApplyRenderCmd applier(m_cmd);
    for (auto& cmd : m_record_cmds) {
        std::visit(applier, cmd);
    }
    m_record_cmds.clear();
    vkCmdEndRenderPass(m_cmd.Impl().m_cmd);
}

CopyEncoder::CopyEncoder(Command& cmd) : m_cmd{cmd} {}

CommandEncoder::CommandEncoder(Command& cmd) : m_cmd{cmd} {}

CopyEncoder CommandEncoder::BeginCopy() {
    return CopyEncoder{m_cmd};
}

void CopyEncoder::CopyBufferToBuffer(const Buffer& src, uint64_t srcOffset,
                                     const Buffer& dst, uint64_t dstOffset,
                                     uint64_t size) {
    BufCopyBuf copy_cmd{src, dst, srcOffset, dstOffset, size};
    m_buffer_copies.emplace_back(std::move(copy_cmd));
}

void CopyEncoder::CopyBufferToTexture(const Buffer& src, Image& dst,
                                      const BufferImageCopy& copy) {
    BufCopyImage copy_cmd{src, dst, copy};
    m_image_copies.emplace_back(std::move(copy_cmd));
}

void CopyEncoder::End() {
    for (auto& copy_cmd : m_buffer_copies) {
        VkBufferCopy region;
        region.size = copy_cmd.size;
        region.srcOffset = copy_cmd.src_offset;
        region.dstOffset = copy_cmd.dst_offset;

        vkCmdCopyBuffer(m_cmd.Impl().m_cmd, copy_cmd.src.Impl().m_buffer,
                        copy_cmd.dst.Impl().m_buffer, 1, &region);

        m_cmd.Impl().m_flags |= CommandImpl::Flag::Transfer;
    }

    for (auto& copy_cmd : m_image_copies) {
        for (size_t i = 0; i < copy_cmd.copy.imageExtent.depth; i++) {
            // TODO: more precious range
            size_t idx = i + copy_cmd.copy.imageOffset.z;

            VkImageMemoryBarrier barrier{};
            barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            barrier.image = copy_cmd.dst.Impl().m_image;
            barrier.oldLayout = copy_cmd.dst.Impl().m_layouts[idx];
            barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_NONE;
            barrier.subresourceRange.layerCount = 1;
            barrier.subresourceRange.levelCount = 1;
            barrier.subresourceRange.baseArrayLayer = 0;
            barrier.subresourceRange.baseMipLevel = 0;
            vkCmdPipelineBarrier(m_cmd.Impl().m_cmd,
                                 VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                                 VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr,
                                 0, nullptr, 1, &barrier);
            m_cmd.Impl().AddLayoutTransition(
                &copy_cmd.dst.Impl(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, i);
        }

        VkBufferImageCopy copy;
        copy.bufferRowLength = copy_cmd.copy.bufferRowLength;
        copy.bufferOffset = copy_cmd.copy.bufferOffset;
        copy.bufferImageHeight = copy_cmd.copy.bufferImageHeight;
        copy.imageSubresource = copy_cmd.copy.imageSubresource;
        copy.imageOffset = copy_cmd.copy.imageOffset;
        copy.imageExtent = copy_cmd.copy.imageExtent;

        vkCmdCopyBufferToImage(m_cmd.Impl().m_cmd, copy_cmd.src.Impl().m_buffer,
                               copy_cmd.dst.Impl().m_image,
                               VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, 1,
                               &copy);

        m_cmd.Impl().m_flags |= CommandImpl::Flag::Transfer;
    }

    m_buffer_copies.clear();
    m_image_copies.clear();
}

RenderPassEncoder CommandEncoder::BeginRenderPass(
    const RenderPass& render_pass, const Framebuffer& fbo,
    const std::vector<VkClearValue>& clear_values) {
    VkRenderPassBeginInfo render_pass_info = {};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    render_pass_info.renderPass = render_pass.Impl().m_render_pass;
    render_pass_info.pClearValues = clear_values.data();
    render_pass_info.clearValueCount = clear_values.size();
    render_pass_info.framebuffer = fbo.Impl().m_fbo;

    for (auto& view : fbo.Impl().m_views) {
        auto& layouts = view.GetImage().Impl().m_layouts;
        for (int i = 0; i < layouts.size(); i++) {
            VkImageLayout layout = view.GetImage().Impl().m_layouts[i];
            VkImageMemoryBarrier barrier{};
            barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            barrier.image = view.GetImage().Impl().m_image;
            barrier.oldLayout = layout;
            barrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            barrier.subresourceRange.layerCount = 1;
            barrier.subresourceRange.levelCount = 1;
            barrier.subresourceRange.baseArrayLayer = 0;
            barrier.subresourceRange.baseMipLevel = 0;
            vkCmdPipelineBarrier(m_cmd.Impl().m_cmd,
                                 VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,
                                 VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0,
                                 nullptr, 0, nullptr, 1, &barrier);
            m_cmd.Impl().AddLayoutTransition(&view.GetImage().Impl(), layout,
                                             i);
        }
    }

    vkCmdBeginRenderPass(m_cmd.Impl().m_cmd, &render_pass_info,
                         VK_SUBPASS_CONTENTS_INLINE);

    m_cmd.Impl().m_flags |= CommandImpl::Flag::Render;

    return RenderPassEncoder{m_cmd};
}

Command& CommandEncoder::Finish() {
    return m_cmd;
}

}  // namespace nickel::graphics