#include "nickel/graphics/primitive_draw.hpp"

#include "nickel/graphics/internal/context_impl.hpp"
#include "nickel/graphics/lowlevel/internal/framebuffer_impl.hpp"
#include "nickel/graphics/lowlevel/internal/image_impl.hpp"
#include "nickel/nickel.hpp"

namespace nickel::graphics {
PrimitiveRenderPass::PrimitiveRenderPass(Device device,
                                         StorageManager& storage_mgr,
                                         RenderPass& render_pass,
                                         CommonResource& res) {
    initVertexBuffer(device);
    initIndicesBuffer(device);
    initBindGroupLayout(device);
    initPipelineLayout(device);
    initBindGroup(res);

    auto local_storage = storage_mgr.AcqurieLocalStorage();
    local_storage->WaitStorageReady();
    auto vertex_shader_content = local_storage->ReadStorageFile(
        "engine/assets/shaders/shader_gridline.vert.spv");
    auto frag_shader_content = local_storage->ReadStorageFile(
        "engine/assets/shaders/shader_gridline.frag.spv");

    // shader stages
    ShaderModule vertex_shader = device.CreateShaderModule(
        (uint32_t*)vertex_shader_content.data(), vertex_shader_content.size());
    ShaderModule frag_shader = device.CreateShaderModule(
        (uint32_t*)frag_shader_content.data(), frag_shader_content.size());

    initLinePipeline(device, vertex_shader, frag_shader, render_pass);
    initTrianglePipeline(device, vertex_shader, frag_shader, render_pass);
    initTriangleSolidPipeline(device, vertex_shader, frag_shader, render_pass);
}

void PrimitiveRenderPass::Begin() {
    m_line_vertex_buffer.m_cpu.MapAsync();
    m_line_vertex_buffer.m_ptr =
        static_cast<char*>(m_line_vertex_buffer.m_cpu.GetMappedRange());
    m_triangle_vertex_buffer.m_cpu.MapAsync();
    m_triangle_vertex_buffer.m_ptr =
        static_cast<char*>(m_triangle_vertex_buffer.m_cpu.GetMappedRange());
}

void PrimitiveRenderPass::UploadData2GPU(Device& device) {
    m_line_vertex_buffer.m_cpu.Unmap();
    m_line_vertex_buffer.m_ptr = nullptr;
    m_triangle_vertex_buffer.m_cpu.Unmap();
    m_triangle_vertex_buffer.m_ptr = nullptr;

    if (m_line_vertex_buffer.m_elem_count > 0) {
        CommandEncoder encoder = device.CreateCommandEncoder();
        CopyEncoder copy = encoder.BeginCopy();
        copy.CopyBufferToBuffer(
            m_line_vertex_buffer.m_cpu, 0, m_line_vertex_buffer.m_gpu, 0,
            sizeof(Vertex) * m_line_vertex_buffer.m_elem_count);
        copy.End();
        auto cmd = encoder.Finish();
        device.Submit(cmd, {}, {}, {});
    }

    if (m_triangle_vertex_buffer.m_elem_count > 0) {
        CommandEncoder encoder = device.CreateCommandEncoder();
        CopyEncoder copy = encoder.BeginCopy();
        copy.CopyBufferToBuffer(
            m_triangle_vertex_buffer.m_cpu, 0, m_triangle_vertex_buffer.m_gpu,
            0, sizeof(Vertex) * m_triangle_vertex_buffer.m_elem_count);
        copy.End();
        auto cmd = encoder.Finish();
        device.Submit(cmd, {}, {}, {});
    }

    if (m_triangle_indices_buffer.m_elem_count > 0) {
        CommandEncoder encoder = device.CreateCommandEncoder();
        CopyEncoder copy = encoder.BeginCopy();
        copy.CopyBufferToBuffer(
            m_triangle_indices_buffer.m_cpu, 0, m_triangle_indices_buffer.m_gpu,
            0, sizeof(uint32_t) * m_triangle_indices_buffer.m_elem_count);
        copy.End();
        auto cmd = encoder.Finish();
        device.Submit(cmd, {}, {}, {});
    }

    device.WaitIdle();
}

void PrimitiveRenderPass::ApplyDrawCall(RenderPassEncoder& encoder,
                                        bool wireframe) {
    auto& camera = nickel::Context::GetInst().GetCamera();

    Mat44 model_view[] = {
        Mat44::Identity(),
        camera.GetView(),
    };

    if (m_line_vertex_buffer.m_elem_count > 0) {
        encoder.BindGraphicsPipeline(m_line_pipeline);
        encoder.SetPushConstant(ShaderStage::Vertex, &model_view, 0,
                                sizeof(model_view));
        encoder.SetBindGroup(0, m_bind_group);
        encoder.BindVertexBuffer(0, m_line_vertex_buffer.m_gpu, 0);
        encoder.Draw(m_line_vertex_buffer.m_elem_count, 1, 0, 0);
    }

    if (m_triangle_vertex_buffer.m_elem_count > 0) {
        if (wireframe) {
            encoder.BindGraphicsPipeline(m_triangle_wire_pipeline);
        } else {
            encoder.BindGraphicsPipeline(m_triangle_solid_pipeline);
        }
        encoder.SetBindGroup(0, m_bind_group);
        encoder.SetPushConstant(ShaderStage::Vertex, &model_view, 0,
                                sizeof(model_view));
        encoder.BindVertexBuffer(0, m_triangle_vertex_buffer.m_gpu, 0);
        encoder.BindIndexBuffer(m_triangle_indices_buffer.m_gpu,
                                IndexType::Uint32, 0);
        encoder.DrawIndexed(m_triangle_indices_buffer.m_elem_count, 1, 0, 0, 0);
    }

    m_line_vertex_buffer.m_elem_count = 0;
    m_triangle_vertex_buffer.m_elem_count = 0;
    m_triangle_indices_buffer.m_elem_count = 0;
}

void PrimitiveRenderPass::DrawLineList(std::span<Vertex> vertices) {
    NICKEL_ASSERT(vertices.size() % 2 == 0);

    size_t size = vertices.size() * sizeof(Vertex);
    memcpy(m_line_vertex_buffer.m_ptr, vertices.data(), size);
    m_line_vertex_buffer.m_ptr += size;
    m_line_vertex_buffer.m_elem_count += vertices.size();
}

void PrimitiveRenderPass::DrawTriangleList(std::span<Vertex> vertices,
                                           std::span<uint32_t> indices) {
    NICKEL_ASSERT(indices.size() % 3 == 0);

    size_t vertex_size = vertices.size() * sizeof(Vertex);
    size_t index_size = indices.size() * sizeof(uint32_t);

    memcpy(m_triangle_vertex_buffer.m_ptr, vertices.data(), vertex_size);
    m_triangle_vertex_buffer.m_ptr += vertex_size;
    m_triangle_vertex_buffer.m_elem_count += vertices.size();

    memcpy(m_triangle_indices_buffer.m_ptr, indices.data(), index_size);
    m_triangle_indices_buffer.m_ptr += index_size;
    m_triangle_indices_buffer.m_elem_count += indices.size();
}

bool PrimitiveRenderPass::NeedDraw() const {
    return m_triangle_indices_buffer.m_elem_count != 0 ||
           m_triangle_vertex_buffer.m_elem_count != 0 ||
           m_line_vertex_buffer.m_elem_count != 0;
}

void PrimitiveRenderPass::initBindGroupLayout(Device& device) {
    BindGroupLayout::Descriptor desc;
    BindGroupLayout::Entry entry;
    entry.m_shader_stage = ShaderStage::Vertex;
    entry.m_type = BindGroupEntryType::UniformBuffer;
    entry.m_array_size = 1;
    desc.m_entries[0] = entry;

    m_bind_group_layout = device.CreateBindGroupLayout(desc);
}

void PrimitiveRenderPass::initBindGroup(CommonResource& res) {
    BindGroup::Descriptor desc;
    BindGroup::Entry entry;
    BindGroup::BufferBinding binding;
    binding.m_buffer = res.m_camera_buffer;
    binding.m_type = BindGroup::BufferBinding::Type::Uniform;
    entry.m_binding.m_entry = binding;
    entry.m_shader_stage = ShaderStage::Vertex;
    entry.m_array_size = 1;
    desc.m_entries[0] = entry;

    m_bind_group = m_bind_group_layout.RequireBindGroup(desc);
}

void PrimitiveRenderPass::initPipelineLayout(Device& device) {
    PipelineLayout::Descriptor desc;
    desc.m_layouts.push_back(m_bind_group_layout);
    PipelineLayout::Descriptor::PushConstantRange range;
    range.m_offset = 0;
    range.m_shader_stage = ShaderStage::Vertex;
    range.m_size = sizeof(Mat44) * 2;
    desc.m_push_contants.push_back(range);

    m_pipeline_layout = device.CreatePipelineLayout(desc);
}

void PrimitiveRenderPass::initLinePipeline(Device& device,
                                           ShaderModule& vertex_shader,
                                           ShaderModule& frag_shader,
                                           RenderPass& render_pass) {
    GraphicsPipeline::Descriptor desc =
        getPipelineDescTmpl(vertex_shader, frag_shader, render_pass);
    desc.m_primitive.m_topology = Topology::LineList;
    desc.m_primitive.m_cull_mode = CullMode::None;
    desc.m_primitive.m_front_face = FrontFace::CCW;
    desc.m_primitive.m_polygon_mode = PolygonMode::Line;
    m_line_pipeline = device.CreateGraphicPipeline(desc);
}

void PrimitiveRenderPass::initTrianglePipeline(Device& device,
                                               ShaderModule& vertex_shader,
                                               ShaderModule& frag_shader,
                                               RenderPass& render_pass) {
    GraphicsPipeline::Descriptor desc =
        getPipelineDescTmpl(vertex_shader, frag_shader, render_pass);
    desc.m_primitive.m_topology = Topology::TriangleList;
    desc.m_primitive.m_cull_mode = CullMode::Back;
    desc.m_primitive.m_front_face = FrontFace::CCW;
    desc.m_primitive.m_polygon_mode = PolygonMode::Line;
    m_triangle_wire_pipeline = device.CreateGraphicPipeline(desc);
}

void PrimitiveRenderPass::initTriangleSolidPipeline(Device& device,
                                                    ShaderModule& vertex_shader,
                                                    ShaderModule& frag_shader,
                                                    RenderPass& render_pass) {
    GraphicsPipeline::Descriptor desc =
        getPipelineDescTmpl(vertex_shader, frag_shader, render_pass);
    desc.m_primitive.m_topology = Topology::TriangleList;
    desc.m_primitive.m_cull_mode = CullMode::Back;
    desc.m_primitive.m_front_face = FrontFace::CCW;
    desc.m_primitive.m_polygon_mode = PolygonMode::Fill;
    m_triangle_solid_pipeline = device.CreateGraphicPipeline(desc);
}

void PrimitiveRenderPass::initVertexBuffer(Device& device) {
    {
        Buffer::Descriptor desc;
        desc.m_memory_type = MemoryType::CPULocal;
        desc.m_size = sizeof(Vertex) * MaxLineNum;
        desc.m_usage = BufferUsage::CopySrc;

        m_line_vertex_buffer.m_cpu = device.CreateBuffer(desc);
    }

    {
        Buffer::Descriptor desc;
        desc.m_memory_type = MemoryType::GPULocal;
        desc.m_size = sizeof(Vertex) * MaxLineNum;
        desc.m_usage = Flags{BufferUsage::Vertex} | BufferUsage::CopyDst;

        m_line_vertex_buffer.m_gpu = device.CreateBuffer(desc);
    }

    {
        Buffer::Descriptor desc;
        desc.m_memory_type = MemoryType::CPULocal;
        desc.m_size = sizeof(Vertex) * MaxTriangleNum;
        desc.m_usage = BufferUsage::CopySrc;

        m_triangle_vertex_buffer.m_cpu = device.CreateBuffer(desc);
    }

    {
        Buffer::Descriptor desc;
        desc.m_memory_type = MemoryType::GPULocal;
        desc.m_size = sizeof(Vertex) * MaxTriangleNum;
        desc.m_usage = Flags{BufferUsage::Vertex} | BufferUsage::CopyDst;

        m_triangle_vertex_buffer.m_gpu = device.CreateBuffer(desc);
    }
}

void PrimitiveRenderPass::initIndicesBuffer(Device& device) {
    {
        Buffer::Descriptor desc;
        desc.m_memory_type = MemoryType::GPULocal;
        desc.m_size = sizeof(uint32_t) * 3 * MaxTriangleNum;
        desc.m_usage = BufferUsage::CopySrc;

        m_triangle_indices_buffer.m_gpu = device.CreateBuffer(desc);
    }

    {
        Buffer::Descriptor desc;
        desc.m_memory_type = MemoryType::GPULocal;
        desc.m_size = sizeof(uint32_t) * 3 * MaxTriangleNum;
        desc.m_usage = Flags{BufferUsage::Index} | BufferUsage::CopyDst;

        m_triangle_indices_buffer.m_cpu = device.CreateBuffer(desc);
    }
}

GraphicsPipeline::Descriptor PrimitiveRenderPass::getPipelineDescTmpl(
    ShaderModule& vertex_shader, ShaderModule& frag_shader,
    RenderPass& render_pass) {
    GraphicsPipeline::Descriptor desc;

    desc.m_subpass = 0;

    GraphicsPipeline::Descriptor::ShaderStage vertex_stage{vertex_shader,
                                                           "main"};
    GraphicsPipeline::Descriptor::ShaderStage frag_stage{frag_shader, "main"};

    desc.m_shader_stages[ShaderStage::Vertex] = vertex_stage;
    desc.m_shader_stages[ShaderStage::Fragment] = frag_stage;

    desc.m_primitive.m_topology = Topology::LineStrip;
    desc.m_primitive.m_cull_mode = CullMode::None;
    desc.m_primitive.m_front_face = FrontFace::CCW;
    desc.m_primitive.m_unclipped_depth = false;
    desc.m_primitive.m_strip_index_format = StripIndexFormat::Uint32;

    // input vertex state
    GraphicsPipeline::Descriptor::BufferState buffer_state;
    {
        GraphicsPipeline::Descriptor::BufferState::Attribute attr;
        attr.m_format = VertexFormat::Float32x3;
        attr.m_offset = 0;
        attr.m_shader_location = 0;
        buffer_state.m_attributes.push_back(attr);
    }

    {
        GraphicsPipeline::Descriptor::BufferState::Attribute attr;
        attr.m_format = VertexFormat::Float32x4;
        attr.m_offset = sizeof(float) * 3;
        attr.m_shader_location = 1;
        buffer_state.m_attributes.push_back(attr);
    }

    buffer_state.m_array_stride = sizeof(float) * 7;
    buffer_state.m_step_mode =
        GraphicsPipeline::Descriptor::BufferState::StepMode::Vertex;
    desc.m_vertex.m_buffers.push_back(buffer_state);

    // blend state
    GraphicsPipeline::Descriptor::BlendState blend_state;
    desc.m_blend_state.push_back(blend_state);

    {
        GraphicsPipeline::Descriptor::DepthStencilState depth_stencil_state;
        depth_stencil_state.m_depth_format = Format::D32_SFLOAT_S8_UINT;
        desc.m_depth_stencil = depth_stencil_state;
    }

    desc.m_render_pass = render_pass;
    desc.m_layout = m_pipeline_layout;

    return desc;
}
}  // namespace nickel::graphics
