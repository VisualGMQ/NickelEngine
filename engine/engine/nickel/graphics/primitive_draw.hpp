#pragma once
#include "nickel/fs/storage.hpp"
#include "nickel/graphics/lowlevel/device.hpp"
#include "nickel/graphics/lowlevel/graphics_pipeline.hpp"
#include "nickel/video/window.hpp"
#include <span>

namespace nickel::graphics {
struct Vertex {
    Vec3 position;
    Vec4 color;
};

class PrimitiveDrawPass {
public:
    PrimitiveDrawPass(Device, StorageManager&, RenderPass& render_pass);

    void Begin();
    void UploadData2GPU(Device& device);
    void ApplyDrawCall(RenderPassEncoder&, bool wireframe);
    void DrawLineStrip(std::span<Vertex> vertices);
    void DrawTriangleList(std::span<Vertex> vertices,
                          std::span<uint32_t> indices);

    bool NeedDraw() const;

private:
    static constexpr uint32_t MaxLineNum = 1024;
    static constexpr uint32_t MaxTriangleNum = 1024;

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
    Buffer m_project_buffer;
    BufferBundle m_line_vertex_buffer;
    BufferBundle m_triangle_vertex_buffer;
    BufferBundle m_triangle_indices_buffer;

    void initBindGroupLayout(Device&);
    void initBindGroup();
    void initPipelineLayout(Device&);
    void initProjectBuffer(Device&);
    void initLinePipeline(Device&, ShaderModule& vertex, ShaderModule& frag,
                          RenderPass& render_pass);
    void initTrianglePipeline(Device&, ShaderModule& vertex, ShaderModule& frag,
                              RenderPass& render_pass);
    void initTriangleSolidPipeline(Device&, ShaderModule& vertex,
                                   ShaderModule& frag,
                                   RenderPass& render_pass);
    void initVertexBuffer(Device&);
    void initIndicesBuffer(Device&);

    GraphicsPipeline::Descriptor getPipelineDescTmpl(
        ShaderModule& vertex_shader, ShaderModule& frag_shader,
        RenderPass& render_pass);
};
}
