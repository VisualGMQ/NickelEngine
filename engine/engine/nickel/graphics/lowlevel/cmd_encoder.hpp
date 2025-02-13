﻿#pragma once
#include "nickel/graphics/lowlevel/framebuffer.hpp"
#include "nickel/graphics/lowlevel/bind_group.hpp"
#include "nickel/graphics/lowlevel/buffer.hpp"
#include "nickel/graphics/lowlevel/cmd.hpp"
#include "nickel/graphics/lowlevel/graphics_pipeline.hpp"
#include "nickel/graphics/lowlevel/image.hpp"
#include "nickel/common/dllexport.hpp"
#include "nickel/common/math/math.hpp"
#include "nickel/common/math/smatrix.hpp"
#include <span>

#include "nickel/common/memory/memory.hpp"

namespace nickel::graphics {

class Framebuffer;

struct ClearValue {
    struct DepthStencilValue {
        float depth{};
        uint32_t stencil{};
    };

    ClearValue() = default;
    ClearValue(float r, float g, float b, float a);
    ClearValue(int32_t r, int32_t g, int32_t b, int32_t a);
    ClearValue(uint32_t r, uint32_t g, uint32_t b, uint32_t a);
    ClearValue(DepthStencilValue);

    std::variant<std::array<float, 4>, std::array<int32_t, 4>,
                 std::array<uint32_t, 4>, DepthStencilValue>
        m_value;
};

class NICKEL_API RenderPassEncoder final {
public:
    RenderPassEncoder(CommandEncoderImpl& cmd, const RenderPass& render_pass,
                      const Framebuffer& fbo, const Rect& render_area,
                      std::span<ClearValue> clear_values);

    void Draw(uint32_t vertex_count, uint32_t instance_count,
              uint32_t first_vertex, uint32_t first_instance);
    void DrawIndexed(uint32_t index_count, uint32_t instance_count,
                     uint32_t first_index, uint32_t vertex_offset,
                     uint32_t first_instance);
    void BindVertexBuffer(uint32_t slot, Buffer buffer, uint64_t offset);
    void BindIndexBuffer(Buffer buffer, IndexType, uint64_t offset);
    void SetBindGroup(uint32_t set, BindGroup&);
    void SetBindGroup(uint32_t set, BindGroup&, const std::vector<uint32_t>& dynamicOffset);
    void SetPushConstant(Flags<ShaderStage> stage, const void* value,
                         uint32_t offset, uint32_t size);
    void SetViewport(float x, float y, float width, float height,
                     float min_depth, float max_depth);
    void SetScissor(int32_t x, int32_t y, uint32_t width, uint32_t height);
    void BindGraphicsPipeline(const GraphicsPipeline&);

    void End();

private:
    struct RenderPassInfo {
        RenderPass render_pass;
        Framebuffer fbo;
        Rect render_area;
        std::vector<ClearValue> clear_values;
    };

    struct SetBindGroupCmd {
        uint32_t set = 0;
        const BindGroup* bind_group{};
        std::vector<uint32_t> dynamic_offsets;
    };

    struct BindPipelineCmd {
        const GraphicsPipeline* pipeline{};
    };

    struct SetPushConstantCmd {
        Flags<ShaderStage> stage;
        char data[128]{}; // NOTE: temporary store datas
        uint32_t offset{};
        uint32_t size{};

        operator bool() const noexcept { return size != 0;; }
    };

    struct BindVertexBufferCmd {
        uint32_t slot;
        Buffer buffer;
        uint64_t offset;

        operator bool() const noexcept { return buffer; }
    };

    struct BindIndexBufferCmd {
        Buffer buffer;
        IndexType index_type;
        uint64_t offset;

        operator bool() const noexcept { return buffer; }
    };

    struct BindGraphicsPipelineCmd {
        const GraphicsPipeline pipeline;
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
        float x, y, w, h, min_depth, max_depth;
    };

    struct SetScissorCmd {
        int32_t x, y;
        uint32_t w, h;
    };

    using Cmd =
        std::variant<BindGraphicsPipelineCmd, BindIndexBufferCmd,
                     BindVertexBufferCmd, SetPushConstantCmd, SetBindGroupCmd,
                     DrawCmd, SetViewportCmd, SetScissorCmd>;
    struct ApplyRenderCmd;

    CommandEncoderImpl& m_cmd;
    std::vector<Cmd> m_record_cmds;
    RenderPassInfo m_render_pass_info;

    void transferImageLayoutInBindGroup(BindGroup&) const;
    void transferImageLayout2ShaderReadOnlyOptimal(ImageImpl& impl) const;
    void beginRenderPass();
};

class NICKEL_API CopyEncoder final {
public:
    friend class BufferImpl;

    struct BufferImageCopy {
        struct ImageSubresourceLayers {
            Flags<ImageAspect> aspectMask;
            uint32_t mipLevelCount = 1;
            uint32_t baseMipLevel = 0;
            uint32_t layerCount = 1;
            uint32_t baseArrayLayer = 0;
        };

        uint64_t bufferOffset{};
        uint32_t bufferRowLength{};
        uint32_t bufferImageHeight{};
        ImageSubresourceLayers imageSubresource;
        SVector<uint32_t, 3> imageOffset;
        SVector<uint32_t, 3> imageExtent;
    };

    explicit CopyEncoder(CommandEncoderImpl& cmd);
    void CopyBufferToBuffer(const Buffer& src, uint64_t srcOffset,
                            const Buffer& dst, uint64_t dstOffset,
                            uint64_t size);
    void CopyBufferToTexture(const Buffer& src, Image& dst,
                             const BufferImageCopy&);

    void End();

private:
    struct BufCopyBuf {
        const BufferImpl& src;
        const BufferImpl& dst;
        uint64_t src_offset{};
        uint64_t dst_offset{};
        uint64_t size{};
    };

    struct BufCopyImage {
        const Buffer& src;
        Image& dst;
        BufferImageCopy copy;
    };

    CommandEncoderImpl& m_cmd;
    std::vector<BufCopyBuf> m_buffer_copies;
    std::vector<BufCopyImage> m_image_copies;

    void copyBufferToBuffer(const BufferImpl& src, uint64_t srcOffset,
                            const BufferImpl& dst, uint64_t dstOffset,
                            uint64_t size);
};

class CommandEncoderImpl;

class NICKEL_API CommandEncoder {
public:
    explicit CommandEncoder(CommandEncoderImpl& cmd);

    CopyEncoder BeginCopy();
    RenderPassEncoder BeginRenderPass(
        const RenderPass&, const Framebuffer& fbo, const Rect& render_area,
        std::span<ClearValue> clear_values);

    Command Finish();

    CommandEncoderImpl& GetImpl();
    const CommandEncoderImpl& GetImpl() const;

private:
    CommandEncoderImpl& m_cmd;
};

}  // namespace nickel::graphics