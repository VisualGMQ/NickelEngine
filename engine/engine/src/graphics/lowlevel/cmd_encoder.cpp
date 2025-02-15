#include "nickel/graphics/lowlevel/cmd_encoder.hpp"

#include "nickel/graphics/lowlevel/framebuffer.hpp"
#include "nickel/graphics/lowlevel/internal/bind_group_impl.hpp"
#include "nickel/graphics/lowlevel/internal/buffer_impl.hpp"
#include "nickel/graphics/lowlevel/internal/cmd_impl.hpp"
#include "nickel/graphics/lowlevel/internal/enum_convert.hpp"
#include "nickel/graphics/lowlevel/internal/framebuffer_impl.hpp"
#include "nickel/graphics/lowlevel/internal/graphics_pipeline_impl.hpp"
#include "nickel/graphics/lowlevel/internal/image_impl.hpp"
#include "nickel/graphics/lowlevel/internal/pipeline_layout_impl.hpp"
#include "nickel/graphics/lowlevel/internal/render_pass_impl.hpp"
#include "nickel/graphics/lowlevel/internal/vk_call.hpp"

namespace nickel::graphics {
struct RenderPassEncoder::ApplyRenderCmd {
    explicit ApplyRenderCmd(CommandEncoderImpl& cmd) : m_cmd{cmd} {}

    void operator()(const BindGraphicsPipelineCmd& cmd) {
        vkCmdBindPipeline(m_cmd.m_cmd, VK_PIPELINE_BIND_POINT_GRAPHICS,
                          cmd.pipeline.Impl().m_pipeline);
        m_pipeline = &cmd.pipeline;
    }

    void operator()(const BindVertexBufferCmd& cmd) {
        VkDeviceSize device_size = cmd.offset;
        vkCmdBindVertexBuffers(m_cmd.m_cmd, cmd.slot, 1,
                               &cmd.buffer.Impl().m_buffer, &device_size);
    }

    void operator()(const BindIndexBufferCmd& cmd) {
        vkCmdBindIndexBuffer(m_cmd.m_cmd, cmd.buffer.Impl().m_buffer,
                             cmd.offset, IndexType2Vk(cmd.index_type));
    }

    void operator()(const SetPushConstantCmd& cmd) {
        vkCmdPushConstants(m_cmd.m_cmd,
                           m_pipeline->Impl().m_layout.Impl().m_pipeline_layout,
                           cmd.stage, cmd.offset, cmd.size, cmd.data);
    }

    void operator()(const DrawCmd& cmd) {
        switch (cmd.m_type) {
            case DrawCmd::Type::Unknown:
                LOGE("unknown draw call tyep");
                break;
            case DrawCmd::Type::Vertices:
                vkCmdDraw(m_cmd.m_cmd, cmd.elem_count, cmd.instance_count,
                          cmd.first_elem, cmd.first_instance);
                break;
            case DrawCmd::Type::Indexed:
                vkCmdDrawIndexed(m_cmd.m_cmd, cmd.elem_count,
                                 cmd.instance_count, cmd.first_elem,
                                 cmd.vertex_offset, cmd.first_instance);
                break;
        }
    }

    void operator()(const NextSubpassCmd& cmd) {
        vkCmdNextSubpass(m_cmd.m_cmd, SubpassContent2Vk(cmd.content));
    }

    void operator()(const SetViewportCmd& cmd) {
        VkViewport viewport;
        viewport.x = cmd.x;
        viewport.y = cmd.y;
        viewport.width = cmd.w;
        viewport.height = cmd.h;
        viewport.minDepth = cmd.min_depth;
        viewport.maxDepth = cmd.max_depth;
        vkCmdSetViewport(m_cmd.m_cmd, 0, 1, &viewport);
    }

    void operator()(const SetScissorCmd& cmd) {
        VkRect2D scissor;
        scissor.extent.width = cmd.w;
        scissor.extent.height = cmd.h;
        scissor.offset.x = cmd.x;
        scissor.offset.y = cmd.y;
        vkCmdSetScissor(m_cmd.m_cmd, 0, 1, &scissor);
    }

    void operator()(const SetBindGroupCmd& cmd) {
        auto& write_infos =
            cmd.bind_group->Impl().GetWriteInfo().GetWriteDescriptorSets();

        vkCmdPushDescriptorSetKHR(
            m_cmd.m_cmd, VK_PIPELINE_BIND_POINT_GRAPHICS,
            m_pipeline->Impl().m_layout.Impl().m_pipeline_layout, cmd.set,
            write_infos.size(), write_infos.data());
    }

private:
    CommandEncoderImpl& m_cmd;
    const GraphicsPipeline* m_pipeline{};
};

ClearValue::ClearValue(float r, float g, float b, float a) {
    m_value = std::array{r, g, b, a};
}

ClearValue::ClearValue(int32_t r, int32_t g, int32_t b, int32_t a) {
    m_value = std::array{r, g, b, a};
}

ClearValue::ClearValue(uint32_t r, uint32_t g, uint32_t b, uint32_t a) {
    m_value = std::array{r, g, b, a};
}

ClearValue::ClearValue(DepthStencilValue value) {
    m_value = value;
}

RenderPassEncoder::RenderPassEncoder(
    CommandEncoderImpl& cmd, const RenderPass& render_pass, const Framebuffer& fbo,
    const Rect& render_area, std::span<ClearValue> clear_values)
    : m_cmd{cmd},
      m_render_pass_info{render_pass, fbo, render_area,
                         std::vector<ClearValue>{
                             clear_values.begin(), clear_values.end()}} {
}

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
    cmd.m_type = DrawCmd::Type::Indexed;
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

void RenderPassEncoder::BindIndexBuffer(Buffer buffer, IndexType type,
                                        uint64_t offset) {
    BindIndexBufferCmd cmd;
    cmd.index_type = type;
    cmd.buffer = buffer;
    cmd.offset = offset;
    m_record_cmds.push_back(cmd);
}

void RenderPassEncoder::SetBindGroup(uint32_t set, BindGroup& bind_group) {
    SetBindGroupCmd cmd;
    cmd.bind_group = &bind_group;
    cmd.set = set;
    m_record_cmds.push_back(cmd);

    transferImageLayoutInBindGroup(bind_group);
}

void RenderPassEncoder::SetPushConstant(Flags<ShaderStage> stage,
                                        const void* value, uint32_t offset,
                                        uint32_t size) {
    NICKEL_ASSERT(size <= 128,
                  "currently we don't support > 128 bytes in push constants");

    SetPushConstantCmd cmd;
    cmd.stage = stage;
    memcpy(cmd.data, value, size);
    cmd.offset = offset;
    cmd.size = size;
    m_record_cmds.push_back(cmd);
}

void RenderPassEncoder::SetViewport(float x, float y, float width, float height,
                                    float min_depth, float max_depth) {
    SetViewportCmd cmd;
    cmd.x = x;
    cmd.y = y;
    cmd.w = width;
    cmd.h = height;
    cmd.min_depth = min_depth;
    cmd.max_depth = max_depth;
    m_record_cmds.push_back(cmd);
}

void RenderPassEncoder::SetScissor(int32_t x, int32_t y, uint32_t width,
                                   uint32_t height) {
    SetScissorCmd cmd{x, y, width, height};
    m_record_cmds.push_back(cmd);
}

void RenderPassEncoder::BindGraphicsPipeline(const GraphicsPipeline& pipeline) {
    BindGraphicsPipelineCmd cmd{pipeline};
    m_record_cmds.push_back(cmd);
}

void RenderPassEncoder::NextSubpass(SubpassContent content) {
    m_record_cmds.push_back(NextSubpassCmd{content});
}

void RenderPassEncoder::End() {
    beginRenderPass();

    ApplyRenderCmd applier(m_cmd);
    for (auto& cmd : m_record_cmds) {
        std::visit(applier, cmd);
    }
    m_record_cmds.clear();
    vkCmdEndRenderPass(m_cmd.m_cmd);
}

void RenderPassEncoder::transferImageLayoutInBindGroup(
    BindGroup& bind_group) const {
    auto& desc = bind_group.Impl().GetDescriptor();
    for (auto& [_, entry] : desc.entries) {
        auto& bind_entry = entry.binding.entry;
        ImageImpl* image_impl{};
        if (auto binding = std::get_if<BindGroup::ImageBinding>(&bind_entry)) {
            image_impl = &binding->view.GetImage().Impl();
        }
        if (auto binding =
                std::get_if<BindGroup::CombinedSamplerBinding>(&bind_entry)) {
            image_impl = &binding->view.GetImage().Impl();
        }

        if (!image_impl) {
            continue;
        }

        transferImageLayout2ShaderReadOnlyOptimal(*image_impl);
    }
}

void RenderPassEncoder::transferImageLayout2ShaderReadOnlyOptimal(
    ImageImpl& impl) const {
    VkImageAspectFlags aspect = VK_IMAGE_ASPECT_COLOR_BIT;
    auto format = impl.Format();
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

    for (size_t i = 0; i < impl.m_layouts.size(); i++) {
        auto layout = m_cmd.QueryImageLayout(&impl, i);
        if (!layout) {
            layout = ImageLayout2Vk(impl.m_layouts[i]);
        }
        if (layout != VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
            VkImageMemoryBarrier barrier{};
            barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            barrier.image = impl.m_image;
            barrier.oldLayout = layout.value();
            barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
            barrier.subresourceRange.aspectMask = aspect;
            barrier.subresourceRange.layerCount = 1;
            barrier.subresourceRange.levelCount = 1;
            barrier.subresourceRange.baseArrayLayer = i;
            barrier.subresourceRange.baseMipLevel = 0;
            vkCmdPipelineBarrier(m_cmd.m_cmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                                 VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0,
                                 nullptr, 0, nullptr, 1, &barrier);
            m_cmd.AddLayoutTransition(&impl, ImageLayout::ShaderReadOnlyOptimal,
                                      i);
        }
    }
}

void RenderPassEncoder::beginRenderPass() {
    VkRenderPassBeginInfo render_pass_info = {};

    std::vector<VkClearValue> values{};
    for (auto& clear_value : m_render_pass_info.clear_values) {
        VkClearValue vk_clear_value{};
        if (auto color =
                std::get_if<std::array<float, 4>>(&clear_value.m_value)) {
            vk_clear_value.color.float32[0] = (*color)[0];
            vk_clear_value.color.float32[1] = (*color)[1];
            vk_clear_value.color.float32[2] = (*color)[2];
            vk_clear_value.color.float32[3] = (*color)[3];
        } else if (auto color = std::get_if<std::array<int32_t, 4>>(
                       &clear_value.m_value)) {
            vk_clear_value.color.int32[0] = (*color)[0];
            vk_clear_value.color.int32[1] = (*color)[1];
            vk_clear_value.color.int32[2] = (*color)[2];
            vk_clear_value.color.int32[3] = (*color)[3];
        } else if (auto color = std::get_if<std::array<uint32_t, 4>>(
                       &clear_value.m_value)) {
            vk_clear_value.color.uint32[0] = (*color)[0];
            vk_clear_value.color.uint32[1] = (*color)[1];
            vk_clear_value.color.uint32[2] = (*color)[2];
            vk_clear_value.color.uint32[3] = (*color)[3];
        } else if (auto color = std::get_if<ClearValue::DepthStencilValue>(
                       &clear_value.m_value)) {
            vk_clear_value.depthStencil.depth = color->depth;
            vk_clear_value.depthStencil.stencil = color->stencil;
        }
        values.push_back(vk_clear_value);
    }

    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    render_pass_info.renderPass =
        m_render_pass_info.render_pass.Impl().m_render_pass;
    render_pass_info.pClearValues = values.data();
    render_pass_info.clearValueCount = values.size();
    render_pass_info.framebuffer = m_render_pass_info.fbo.Impl().m_fbo;

    auto& render_area = m_render_pass_info.render_area;
    render_pass_info.renderArea.offset.x = render_area.position.x;
    render_pass_info.renderArea.offset.y = render_area.position.y;
    render_pass_info.renderArea.extent.width = render_area.size.w;
    render_pass_info.renderArea.extent.height = render_area.size.h;

    vkCmdBeginRenderPass(m_cmd.m_cmd, &render_pass_info,
                         VK_SUBPASS_CONTENTS_INLINE);

    m_cmd.m_flags |= CommandEncoderImpl::Flag::Render;
}

CopyEncoder::CopyEncoder(CommandEncoderImpl& cmd) : m_cmd{cmd} {}

CommandEncoder::CommandEncoder(CommandEncoderImpl& cmd) : m_cmd{cmd} {
    VkCommandBufferBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    VK_CALL(vkBeginCommandBuffer(cmd.m_cmd, &begin_info));
}

CopyEncoder CommandEncoder::BeginCopy() {
    return CopyEncoder{m_cmd};
}

void CopyEncoder::CopyBufferToBuffer(const Buffer& src, uint64_t srcOffset,
                                     const Buffer& dst, uint64_t dstOffset,
                                     uint64_t size) {
    copyBufferToBuffer(src.Impl(), srcOffset, dst.Impl(), dstOffset, size);
}

void CopyEncoder::copyBufferToBuffer(const BufferImpl& src, uint64_t srcOffset,
                                     const BufferImpl& dst, uint64_t dstOffset,
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

        vkCmdCopyBuffer(m_cmd.m_cmd, copy_cmd.src.m_buffer,
                        copy_cmd.dst.m_buffer, 1, &region);

        m_cmd.m_flags |= CommandEncoderImpl::Flag::Transfer;
    }

    for (auto& copy_cmd : m_image_copies) {
        auto& subresource = copy_cmd.copy.imageSubresource;
        // TODO: more precious range
        for (size_t i = subresource.baseArrayLayer; i < subresource.layerCount;
             i++) {
            auto layout = m_cmd.QueryImageLayout(&copy_cmd.dst.Impl(), i);
            if (!layout) {
                layout = ImageLayout2Vk(copy_cmd.dst.Impl().m_layouts[i]);
            }
            VkImageMemoryBarrier barrier{};
            barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            barrier.image = copy_cmd.dst.Impl().m_image;
            barrier.oldLayout = layout.value();
            barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            auto& subresource = copy_cmd.copy.imageSubresource;
            barrier.subresourceRange.aspectMask =
                ImageAspect2Vk(subresource.aspectMask);
            barrier.subresourceRange.layerCount = subresource.layerCount;
            barrier.subresourceRange.levelCount = subresource.mipLevelCount;
            barrier.subresourceRange.baseArrayLayer =
                subresource.baseArrayLayer;
            barrier.subresourceRange.baseMipLevel = subresource.baseMipLevel;
            vkCmdPipelineBarrier(m_cmd.m_cmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                                 VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr,
                                 0, nullptr, 1, &barrier);
            m_cmd.AddLayoutTransition(&copy_cmd.dst.Impl(),
                                      ImageLayout::TransferDstOptimal, i);
        }

        VkBufferImageCopy copy{};
        copy.bufferRowLength = copy_cmd.copy.bufferRowLength;
        copy.bufferOffset = copy_cmd.copy.bufferOffset;
        copy.bufferImageHeight = copy_cmd.copy.bufferImageHeight;
        copy.imageSubresource.aspectMask =
            ImageAspect2Vk(copy_cmd.copy.imageSubresource.aspectMask);
        copy.imageSubresource.layerCount =
            copy_cmd.copy.imageSubresource.layerCount;
        copy.imageSubresource.mipLevel =
            copy_cmd.copy.imageSubresource.baseMipLevel;
        copy.imageSubresource.baseArrayLayer =
            copy_cmd.copy.imageSubresource.baseArrayLayer;
        copy.imageOffset.x = copy_cmd.copy.imageOffset.x;
        copy.imageOffset.y = copy_cmd.copy.imageOffset.y;
        copy.imageOffset.z = copy_cmd.copy.imageOffset.z;
        copy.imageExtent.width = copy_cmd.copy.imageExtent.w;
        copy.imageExtent.height = copy_cmd.copy.imageExtent.h;
        copy.imageExtent.depth = copy_cmd.copy.imageExtent.l;

        vkCmdCopyBufferToImage(m_cmd.m_cmd, copy_cmd.src.Impl().m_buffer,
                               copy_cmd.dst.Impl().m_image,
                               VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copy);

        m_cmd.m_flags |= CommandEncoderImpl::Flag::Transfer;
    }

    m_buffer_copies.clear();
    m_image_copies.clear();
}

RenderPassEncoder CommandEncoder::BeginRenderPass(
    const RenderPass& render_pass, const Framebuffer& fbo,
    const Rect& render_area, std::span<ClearValue> clear_values) {
    return RenderPassEncoder{m_cmd, render_pass, fbo, render_area,
                             clear_values};
}

Command CommandEncoder::Finish() {
    VK_CALL(vkEndCommandBuffer(m_cmd.m_cmd));
    return Command{m_cmd};
}

CommandEncoderImpl& CommandEncoder::GetImpl() {
    return m_cmd;
}

const CommandEncoderImpl& CommandEncoder::GetImpl() const {
    return m_cmd;
}

}  // namespace nickel::graphics
