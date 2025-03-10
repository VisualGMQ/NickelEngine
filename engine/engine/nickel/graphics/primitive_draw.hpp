#pragma once
#include "nickel/fs/storage.hpp"
#include "nickel/graphics/lowlevel/device.hpp"
#include "nickel/graphics/lowlevel/graphics_pipeline.hpp"
#include "nickel/video/window.hpp"
#include <span>

namespace nickel::graphics {
class CommonResource;

struct Vertex {
    Vec3 m_position;
    Vec4 m_color;
};

class PrimitiveRenderPass {
public:
    PrimitiveRenderPass(Device, StorageManager&, RenderPass&, CommonResource&);

    void Begin();
    void UploadData2GPU(Device& device);
    void ApplyDrawCall(RenderPassEncoder&);
    void DrawLineList(std::span<Vertex> vertices);
    void DrawTriangleList(std::span<Vertex> vertices,
                          std::span<uint32_t> indices, bool wireframe);

    bool NeedDraw() const;

private:
    static constexpr uint32_t MaxLineNum = 1024 * 1024;
    static constexpr uint32_t MaxTriangleNum = 1024 * 1024;
    static constexpr uint32_t MaxLineVertexNum = MaxLineNum * 2;
    static constexpr uint32_t MaxTriangleVertexNum = MaxTriangleNum * 3;

    struct BufferBundle {
        Buffer m_cpu;
        Buffer m_gpu;
        char* m_ptr{};
        uint32_t m_elem_count{};
    };

    BindGroupLayout m_bind_group_layout;
    BindGroup m_bind_group;
    PipelineLayout m_pipeline_layout;
    GraphicsPipeline m_line_pipeline;
    GraphicsPipeline m_triangle_wire_pipeline;
    GraphicsPipeline m_triangle_solid_pipeline;
    std::vector<Image> m_depth_images;
    BufferBundle m_line_vertex_buffer;
    BufferBundle m_triangle_vertex_buffer;
    BufferBundle m_triangle_indices_buffer;
    BufferBundle m_triangle_wireframe_vertex_buffer;
    BufferBundle m_triangle_wireframe_indices_buffer;

    void initBindGroupLayout(Device&);
    void initBindGroup(CommonResource& res);
    void initPipelineLayout(Device&);
    void initLinePipeline(Device&, ShaderModule& vertex, ShaderModule& frag,
                          RenderPass& render_pass);
    void initTrianglePipeline(Device&, ShaderModule& vertex, ShaderModule& frag,
                              RenderPass& render_pass);
    void initTriangleSolidPipeline(Device&, ShaderModule& vertex,
                                   ShaderModule& frag, RenderPass& render_pass);
    void initVertexBuffer(Device&);
    void initIndicesBuffer(Device&);

    GraphicsPipeline::Descriptor getPipelineDescTmpl(
        ShaderModule& vertex_shader, ShaderModule& frag_shader,
        RenderPass& render_pass);

    template <uint32_t MaxElemNum, typename T>
    void copyData2Buffer(BufferBundle& bundle, std::span<T> values) {
        if (MaxElemNum <= bundle.m_elem_count) {
            LOGW("Not enough buffer elements to copy data");
            return;
        }

        size_t size = sizeof(T) * values.size();
        memcpy(bundle.m_ptr, values.data(), size);

        bundle.m_ptr += size;
        bundle.m_elem_count += values.size();
    }

    template <uint32_t MaxElemNum, typename T>
    void copyData2IndexBuffer(BufferBundle& bundle, std::span<T> values,
                              size_t cur_vertex_count) {
        if (MaxElemNum <= bundle.m_elem_count) {
            LOGW("Not enough buffer elements to copy data");
            return;
        }

        std::ranges::transform(
            values, (uint32_t*)bundle.m_ptr,
            [=](const T& value) { return value + cur_vertex_count; });

        bundle.m_ptr += sizeof(T) * values.size();
        bundle.m_elem_count += values.size();
    }
};
}  // namespace nickel::graphics
