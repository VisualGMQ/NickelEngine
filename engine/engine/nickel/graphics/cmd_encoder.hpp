#pragma once
#include "nickel/common/math/smatrix.hpp"
#include "nickel/graphics/bind_group.hpp"
#include "nickel/graphics/buffer.hpp"
#include "nickel/graphics/cmd.hpp"
#include "nickel/graphics/graphics_pipeline.hpp"
#include "nickel/graphics/image.hpp"
#include "nickel/common/dllexport.hpp"

namespace nickel::graphics {

class Framebuffer;

struct NICKEL_API Viewport {
    float x, y, width, height, min_depth, max_depth;
};

class NICKEL_API RenderPassEncoder final {
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
    void SetViewport(const Viewport&);

    void End();

private:
    struct SetBindGroupCmd {
        BindGroup bind_group;
        std::vector<uint32_t> dynamic_offsets;
    };

    struct SetPushConstantCmd {
        VkShaderStageFlags stage;
        const void* data;
        uint32_t offset;
        uint32_t size;

        operator bool() const noexcept { return data != nullptr; }
    };

    struct BindVertexBufferCmd {
        uint32_t slot;
        Buffer buffer;
        uint64_t offset;

        operator bool() const noexcept { return buffer; }
    };

    struct BindIndexBufferCmd {
        Buffer buffer;
        VkIndexType index_type;
        uint64_t offset;

        operator bool() const noexcept { return buffer; }
    };

    struct BindGraphicsPipelineCmd {
        GraphicsPipeline pipeline;
    };

    struct DrawCmd {
        enum class Type {
            Unknown,
            Vertices,
            Indexed,

            // Indirect
        } m_type = Type::Unknown;

        uint32_t elem_count;
        uint32_t instance_count;
        uint32_t first_elem;
        uint32_t vertex_offset;
        uint32_t first_instance;
    };

    struct SetViewportCmd {
        Viewport viewport;
    };

    using Cmd = std::variant<BindGraphicsPipelineCmd, BindIndexBufferCmd,
                             BindVertexBufferCmd, SetPushConstantCmd,
                             SetBindGroupCmd, DrawCmd, SetViewportCmd>;
    struct ApplyRenderCmd;

    Command& m_cmd;
    std::vector<Cmd> m_record_cmds;
};

class NICKEL_API CopyEncoder final {
public:
    struct BufferImageCopy {
        VkDeviceSize bufferOffset;
        uint32_t bufferRowLength;
        uint32_t bufferImageHeight;
        VkImageSubresourceLayers imageSubresource;
        VkOffset3D imageOffset;
        VkExtent3D imageExtent;
    };

    explicit CopyEncoder(Command& cmd);
    void CopyBufferToBuffer(const Buffer& src, uint64_t srcOffset,
                            const Buffer& dst, uint64_t dstOffset,
                            uint64_t size);
    void CopyBufferToTexture(const Buffer& src, Image& dst,
                             const BufferImageCopy&);

    void End();

private:
    struct BufCopyBuf {
        const Buffer& src;
        const Buffer& dst;
        uint64_t src_offset{};
        uint64_t dst_offset{};
        uint64_t size{};
    };

    struct BufCopyImage {
        const Buffer& src;
        Image& dst;
        BufferImageCopy copy;
    };

    Command& m_cmd;
    std::vector<BufCopyBuf> m_buffer_copies;
    std::vector<BufCopyImage> m_image_copies;
};

class NICKEL_API CommandEncoder {
public:
    explicit CommandEncoder(Command& cmd);

    CopyEncoder BeginCopy();
    RenderPassEncoder BeginRenderPass(
        const RenderPass&, const Framebuffer& fbo,
        const std::vector<VkClearValue>& clear_values);

    Command& Finish();

private:
    Command& m_cmd;
};

}  // namespace nickel::graphics