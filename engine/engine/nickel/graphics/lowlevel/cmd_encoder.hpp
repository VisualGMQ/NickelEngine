#pragma once
#include "nickel/common/dllexport.hpp"
#include "nickel/common/math/math.hpp"
#include "nickel/common/math/smatrix.hpp"
#include "nickel/common/memory/memory.hpp"
#include "nickel/graphics/lowlevel/bind_group.hpp"
#include "nickel/graphics/lowlevel/buffer.hpp"
#include "nickel/graphics/lowlevel/cmd.hpp"
#include "nickel/graphics/lowlevel/framebuffer.hpp"
#include "nickel/graphics/lowlevel/graphics_pipeline.hpp"
#include "nickel/graphics/lowlevel/image.hpp"
#include <span>

namespace nickel::graphics {

class Framebuffer;

struct ClearValue {
    struct DepthStencilValue {
        float m_depth{};
        uint32_t m_stencil{};
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
    void SetPushConstant(Flags<ShaderStage> stage, const void* value,
                         uint32_t offset, uint32_t size);
    void SetViewport(float x, float y, float width, float height,
                     float min_depth, float max_depth);
    void SetScissor(int32_t x, int32_t y, uint32_t width, uint32_t height);
    void BindGraphicsPipeline(const GraphicsPipeline&);
    void NextSubpass(SubpassContent);

    void End();

private:
    struct RenderPassInfo {
        RenderPass m_render_pass;
        Framebuffer m_fbo;
        Rect m_render_area;
        std::vector<ClearValue> m_clear_values;
    };

    struct SetBindGroupCmd {
        uint32_t m_set = 0;
        const BindGroup* m_bind_group{};
        std::vector<uint32_t> m_dynamic_offsets;
    };

    struct NextSubpassCmd {
        SubpassContent m_content;
    };

    struct BindPipelineCmd {
        const GraphicsPipeline* m_pipeline{};
    };

    struct SetPushConstantCmd {
        Flags<ShaderStage> m_stage;
        char m_data[128]{};  // NOTE: temporary store datas
        uint32_t m_offset{};
        uint32_t m_size{};

        operator bool() const noexcept {
            return m_size != 0;
            ;
        }
    };

    struct BindVertexBufferCmd {
        uint32_t m_slot;
        Buffer m_buffer;
        uint64_t m_offset;

        operator bool() const noexcept { return m_buffer; }
    };

    struct BindIndexBufferCmd {
        Buffer m_buffer;
        IndexType m_index_type;
        uint64_t m_offset;

        operator bool() const noexcept { return m_buffer; }
    };

    struct BindGraphicsPipelineCmd {
        const GraphicsPipeline m_pipeline;
    };

    struct DrawCmd {
        enum class Type {
            Unknown,
            Vertices,
            Indexed,

            // Indirect
        } m_type = Type::Unknown;

        uint32_t m_elem_count;
        uint32_t m_instance_count;
        uint32_t m_first_elem;
        uint32_t m_vertex_offset;
        uint32_t m_first_instance;
    };

    struct SetViewportCmd {
        float m_x, m_y, m_w, m_h, m_min_depth, m_max_depth;
    };

    struct SetScissorCmd {
        SVector<int32_t, 2> m_position;
        SVector<uint32_t, 2> m_size;
    };

    using Cmd =
        std::variant<BindGraphicsPipelineCmd, BindIndexBufferCmd,
                     BindVertexBufferCmd, SetPushConstantCmd, SetBindGroupCmd,
                     DrawCmd, SetViewportCmd, SetScissorCmd, NextSubpassCmd>;
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
            Flags<ImageAspect> m_aspect_mask;
            uint32_t m_mip_level_count = 1;
            uint32_t m_base_mip_level = 0;
            uint32_t m_layer_count = 1;
            uint32_t m_base_array_layer = 0;
        };

        uint64_t m_buffer_offset{};
        uint32_t m_buffer_row_length{};
        uint32_t m_buffer_image_height{};
        ImageSubresourceLayers m_image_subresource;
        SVector<uint32_t, 3> m_image_offset;
        SVector<uint32_t, 3> m_image_extent;
    };

    explicit CopyEncoder(CommandEncoderImpl& cmd);
    void CopyBufferToBuffer(const Buffer& src, uint64_t src_offset,
                            const Buffer& dst, uint64_t dst_offset,
                            uint64_t size);
    void CopyBufferToTexture(const Buffer& src, Image& dst,
                             const BufferImageCopy&);

    void End();

private:
    struct BufCopyBuf {
        const BufferImpl& m_src;
        const BufferImpl& m_dst;
        uint64_t m_src_offset{};
        uint64_t m_dst_offset{};
        uint64_t m_size{};
    };

    struct BufCopyImage {
        const Buffer& m_src;
        Image& m_dst;
        BufferImageCopy m_copy;
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
    RenderPassEncoder BeginRenderPass(const RenderPass&, const Framebuffer& fbo,
                                      const Rect& render_area,
                                      std::span<ClearValue> clear_values);

    Command Finish();

    CommandEncoderImpl& GetImpl();
    const CommandEncoderImpl& GetImpl() const;

private:
    CommandEncoderImpl& m_cmd;
};

}  // namespace nickel::graphics