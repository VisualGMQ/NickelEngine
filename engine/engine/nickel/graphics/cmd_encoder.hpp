#pragma once
#include "nickel/common/math/smatrix.hpp"
#include "nickel/graphics/bind_group.hpp"
#include "nickel/graphics/buffer.hpp"
#include "nickel/graphics/cmd.hpp"
#include "nickel/graphics/graphics_pipeline.hpp"
#include "nickel/graphics/image.hpp"

namespace nickel::graphics {

class Framebuffer;

class RenderPassEncoder final {
public:
    explicit RenderPassEncoder(Command& cmd);

    void Draw(uint32_t vertex_count, uint32_t instance_count,
              uint32_t first_vertex, uint32_t first_instance);
    void DrawIndexed(uint32_t index_count, uint32_t instance_count,
                     uint32_t first_index, uint32_t vertex_offset,
                     uint32_t first_instance);
    void BindVertexBuffer(uint32_t slot, Buffer buffer, uint64_t offset);
    void BindIndexBuffer(Buffer buffer, VkIndexType, uint64_t offset);
    void SetBindGroup(const BindGroup&);
    void SetBindGroup(const BindGroup&,
                      const std::vector<uint32_t>& dynamicOffset);
    void SetPipeline(const GraphicsPipeline&);
    void SetPushConstant(VkShaderStageFlags stage, const void* value,
                         uint32_t offset, uint32_t size);
    void SetViewport(float x, float y, float width, float height,
                     float min_depth, float max_depth);

    void End();

private:
    Command& m_cmd;
    const GraphicsPipeline* m_pipeline{};
};

class CommandEncoder {
public:
    struct BufTexCopySrc final {
        Buffer buffer;
        uint64_t offset;
        uint64_t rowLength = 0;     // 0 means whole buffer, in texel
        uint64_t rowsPerImage = 0;  // 0 means whole buffer
    };

    struct BufTexCopyDst final {
        uint32_t miplevel = 0;
        SVector<uint32_t, 3> origin;
        Image image;
    };

    explicit CommandEncoder(Command& cmd);

    void CopyBufferToBuffer(const Buffer& src, uint64_t srcOffset,
                            const Buffer& dst, uint64_t dstOffset,
                            uint64_t size);
    void CopyBufferToTexture(const Buffer& src, const Image& dst,
                             const VkBufferImageCopy&);
    RenderPassEncoder BeginRenderPass(
        const RenderPass&, const Framebuffer& fbo,
        const std::vector<VkClearValue>& clear_values);

    Command& Finish();

    Command& m_cmd;
};

}  // namespace nickel::graphics