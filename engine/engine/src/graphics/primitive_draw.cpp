#include "nickel/graphics/primitive_draw.hpp"

#include "nickel/nickel.hpp"
#include "nickel/graphics/lowlevel/internal/framebuffer_impl.hpp"
#include "nickel/graphics/lowlevel/internal/image_impl.hpp"

namespace nickel::graphics {
PrimitiveRenderPass::PrimitiveRenderPass(Device device, StorageManager& storage_mgr,
                                     RenderPass& render_pass) {
    initVertexBuffer(device);
    initIndicesBuffer(device);
    initProjectBuffer(device);
    initBindGroupLayout(device);
    initPipelineLayout(device);
    initBindGroup();

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
    m_line_vertex_buffer.m_ptr = static_cast<char*>(m_line_vertex_buffer.m_cpu.
        GetMappedRange());
    m_triangle_vertex_buffer.m_cpu.MapAsync();
    m_triangle_vertex_buffer.m_ptr = static_cast<char*>(m_triangle_vertex_buffer
        .m_cpu.GetMappedRange());

    m_project_buffer.MapAsync();
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
    if (NeedDraw()) {
        void* ptr = m_project_buffer.GetMappedRange();
        Mat44 proj = camera.GetProject();
        memcpy(ptr, proj.Ptr(), sizeof(proj));
    }

    Mat44 model_view[] = {
        camera.GetView(),
        Mat44::Identity(),
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

void PrimitiveRenderPass::DrawLineStrip(std::span<Vertex> vertices) {
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
    entry.shader_stage = ShaderStage::Vertex;
    entry.type = BindGroupEntryType::UniformBuffer;
    entry.arraySize = 1;
    desc.entries[0] = entry;

    m_bind_group_layout = device.CreateBindGroupLayout(desc);
}

void PrimitiveRenderPass::initBindGroup() {
    BindGroup::Descriptor desc;
    BindGroup::Entry entry;
    BindGroup::BufferBinding binding;
    binding.buffer = m_project_buffer;
    binding.type = BindGroup::BufferBinding::Type::Uniform;
    entry.binding.entry = binding;
    entry.shader_stage = ShaderStage::Vertex;
    entry.arraySize = 1;
    desc.entries[0] = entry;

    m_bind_group = m_bind_group_layout.RequireBindGroup(desc);
}

void PrimitiveRenderPass::initPipelineLayout(Device& device) {
    PipelineLayout::Descriptor desc;
    desc.layouts.push_back(m_bind_group_layout);
    PipelineLayout::Descriptor::PushConstantRange range;
    range.offset = 0;
    range.shader_stage = ShaderStage::Vertex;
    range.size = sizeof(Mat44) * 2;
    desc.push_contants.push_back(range);

    m_pipeline_layout = device.CreatePipelineLayout(desc);
}

void PrimitiveRenderPass::initLinePipeline(Device& device,
                                           ShaderModule& vertex_shader,
                                           ShaderModule& frag_shader,
                                           RenderPass& render_pass) {
    GraphicsPipeline::Descriptor desc =
        getPipelineDescTmpl(vertex_shader, frag_shader, render_pass);
    desc.primitive.topology = Topology::LineList;
    desc.primitive.cullMode = CullMode::None;
    desc.primitive.frontFace = FrontFace::CCW;
    desc.primitive.polygonMode = PolygonMode::Line;
    m_line_pipeline = device.CreateGraphicPipeline(desc);
}

void PrimitiveRenderPass::initTrianglePipeline(Device& device,
                                               ShaderModule& vertex_shader,
                                               ShaderModule& frag_shader,
                                               RenderPass& render_pass) {
    GraphicsPipeline::Descriptor desc =
        getPipelineDescTmpl(vertex_shader, frag_shader, render_pass);
    desc.primitive.topology = Topology::TriangleList;
    desc.primitive.cullMode = CullMode::Back;
    desc.primitive.frontFace = FrontFace::CCW;
    desc.primitive.polygonMode = PolygonMode::Line;
    m_triangle_wire_pipeline = device.CreateGraphicPipeline(desc);
}

void PrimitiveRenderPass::initTriangleSolidPipeline(Device& device,
                                                    ShaderModule& vertex_shader,
                                                    ShaderModule& frag_shader,
                                                    RenderPass& render_pass) {
    GraphicsPipeline::Descriptor desc =
        getPipelineDescTmpl(vertex_shader, frag_shader, render_pass);
    desc.primitive.topology = Topology::TriangleList;
    desc.primitive.cullMode = CullMode::Back;
    desc.primitive.frontFace = FrontFace::CCW;
    desc.primitive.polygonMode = PolygonMode::Fill;
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

void PrimitiveRenderPass::initProjectBuffer(Device& device) {
    Buffer::Descriptor desc;
    desc.m_memory_type = MemoryType::Coherence;
    desc.m_size = sizeof(Mat44);
    desc.m_usage = BufferUsage::Uniform;
    m_project_buffer = device.CreateBuffer(desc);
}

GraphicsPipeline::Descriptor PrimitiveRenderPass::getPipelineDescTmpl(
    ShaderModule& vertex_shader, ShaderModule& frag_shader,
    RenderPass& render_pass) {
    GraphicsPipeline::Descriptor desc;

    GraphicsPipeline::Descriptor::ShaderStage vertex_stage{vertex_shader,
                                                           "main"};
    GraphicsPipeline::Descriptor::ShaderStage frag_stage{frag_shader, "main"};

    desc.m_shader_stages[ShaderStage::Vertex] = vertex_stage;
    desc.m_shader_stages[ShaderStage::Fragment] = frag_stage;

    desc.primitive.topology = Topology::LineStrip;
    desc.primitive.cullMode = CullMode::None;
    desc.primitive.frontFace = FrontFace::CCW;
    desc.primitive.unclippedDepth = false;
    desc.primitive.stripIndexFormat = StripIndexFormat::Uint32;

    // input vertex state
    GraphicsPipeline::Descriptor::BufferState buffer_state;
    {
        GraphicsPipeline::Descriptor::BufferState::Attribute attr;
        attr.format = VertexFormat::Float32x3;
        attr.offset = 0;
        attr.shaderLocation = 0;
        buffer_state.attributes.push_back(attr);
    }

    {
        GraphicsPipeline::Descriptor::BufferState::Attribute attr;
        attr.format = VertexFormat::Float32x4;
        attr.offset = sizeof(float) * 3;
        attr.shaderLocation = 1;
        buffer_state.attributes.push_back(attr);
    }

    buffer_state.arrayStride = sizeof(float) * 7;
    buffer_state.stepMode =
        GraphicsPipeline::Descriptor::BufferState::StepMode::Vertex;
    desc.vertex.buffers.push_back(buffer_state);

    // blend state
    GraphicsPipeline::Descriptor::BlendState blend_state;
    desc.blend_state.push_back(blend_state);

    {
        GraphicsPipeline::Descriptor::DepthStencilState depth_stencil_state;
        depth_stencil_state.depthFormat = Format::D32_SFLOAT_S8_UINT;
        desc.depthStencil = depth_stencil_state;
    }

    desc.m_render_pass = render_pass;
    desc.layout = m_pipeline_layout;

    return desc;
}
}  // namespace nickel::graphics
