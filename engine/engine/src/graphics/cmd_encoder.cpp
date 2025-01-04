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

RenderPassEncoder::RenderPassEncoder(Command& cmd) : m_cmd{cmd} {}

void RenderPassEncoder::Draw(uint32_t vertex_count, uint32_t instance_count,
                             uint32_t first_vertex, uint32_t first_instance) {
    vkCmdDraw(m_cmd.Impl().m_cmd, vertex_count, instance_count, first_vertex,
              first_instance);
}

void RenderPassEncoder::DrawIndexed(uint32_t index_count,
                                    uint32_t instance_count,
                                    uint32_t first_index,
                                    uint32_t vertex_offset,
                                    uint32_t first_instance) {
    vkCmdDrawIndexed(m_cmd.Impl().m_cmd, index_count, instance_count,
                     first_index, vertex_offset, first_instance);
}

void RenderPassEncoder::BindVertexBuffer(uint32_t slot, Buffer buffer,
                                         uint64_t offset) {
    VkDeviceSize device_size = offset;
    vkCmdBindVertexBuffers(m_cmd.Impl().m_cmd, slot, 1, &buffer.Impl().m_buffer,
                           &device_size);
}

void RenderPassEncoder::BindIndexBuffer(Buffer buffer, VkIndexType type,
                                        uint64_t offset) {
    vkCmdBindIndexBuffer(m_cmd.Impl().m_cmd, buffer.Impl().m_buffer, offset,
                         type);
}

void RenderPassEncoder::SetBindGroup(const BindGroup& bind_group) {
    auto& descriptor_sets = bind_group.Impl().m_set_lists;
    vkCmdBindDescriptorSets(
        m_cmd.Impl().m_cmd, VK_PIPELINE_BIND_POINT_GRAPHICS,
        m_pipeline->Impl().m_layout.Impl().m_pipeline_layout, 0,
        descriptor_sets.size(), descriptor_sets.data(), 0, nullptr);
}

void RenderPassEncoder::SetBindGroup(
    const BindGroup& bind_group, const std::vector<uint32_t>& dynamicOffset) {
    auto& descriptor_sets = bind_group.Impl().m_set_lists;
    vkCmdBindDescriptorSets(
        m_cmd.Impl().m_cmd, VK_PIPELINE_BIND_POINT_GRAPHICS,
        m_pipeline->Impl().m_layout.Impl().m_pipeline_layout, 0,
        descriptor_sets.size(), descriptor_sets.data(), dynamicOffset.size(),
        dynamicOffset.data());
}

void RenderPassEncoder::SetPipeline(const GraphicsPipeline& pipeline) {
    m_pipeline = &pipeline;
    vkCmdBindPipeline(m_cmd.Impl().m_cmd, VK_PIPELINE_BIND_POINT_GRAPHICS,
                      pipeline.Impl().m_pipeline);
}

void RenderPassEncoder::SetPushConstant(VkShaderStageFlags stage,
                                        const void* value, uint32_t offset,
                                        uint32_t size) {
    vkCmdPushConstants(m_cmd.Impl().m_cmd,
                       m_pipeline->Impl().m_layout.Impl().m_pipeline_layout,
                       stage, offset, size, value);
}

void RenderPassEncoder::SetViewport(float x, float y, float width, float height,
                                    float min_depth, float max_depth) {
    VkViewport viewport;
    viewport.x = x;
    viewport.y = y;
    viewport.width = width;
    viewport.height = height;
    viewport.minDepth = min_depth;
    viewport.maxDepth = max_depth;
    vkCmdSetViewport(m_cmd.Impl().m_cmd, 0, 1, &viewport);
}

void RenderPassEncoder::End() {
    vkCmdEndRenderPass(m_cmd.Impl().m_cmd);
}

CommandEncoder::CommandEncoder(Command& cmd) : m_cmd{cmd} {}

void CommandEncoder::CopyBufferToBuffer(const Buffer& src, uint64_t srcOffset,
                                        const Buffer& dst, uint64_t dstOffset,
                                        uint64_t size) {
    VkBufferCopy region;
    region.size = size;
    region.srcOffset = srcOffset;
    region.dstOffset = dstOffset;

    vkCmdCopyBuffer(m_cmd.Impl().m_cmd, src.Impl().m_buffer,
                    dst.Impl().m_buffer, 1, &region);
}

void CommandEncoder::CopyBufferToTexture(const Buffer& src, const Image& dst,
                                         const VkBufferImageCopy& copy) {
    vkCmdCopyBufferToImage(m_cmd.Impl().m_cmd, src.Impl().m_buffer,
                           dst.Impl().m_image,
                           // TODO: layout maybe not correct
                           VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, 1, &copy);
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

    vkCmdBeginRenderPass(m_cmd.Impl().m_cmd, &render_pass_info,
                         VK_SUBPASS_CONTENTS_INLINE);

    return RenderPassEncoder{m_cmd};
}

Command& CommandEncoder::Finish() {
    vkCmdEndRendering(m_cmd.Impl().m_cmd);
    return m_cmd;
}

}  // namespace nickel::graphics