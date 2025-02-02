#include "nickel/graphics/gltf_draw.hpp"

#include "nickel/common/common.hpp"
#include "nickel/common/macro.hpp"
#include "nickel/graphics/internal/gltf_model_impl.hpp"
#include "nickel/graphics/internal/material3d_impl.hpp"
#include "nickel/nickel.hpp"

namespace nickel::graphics {

GLTFRenderPass::GLTFRenderPass(Device device, CommonResource& res) {
    initBindGroupLayout(device);
    initPipelineLayout(device);

    ShaderModule vertex_shader, frag_shader;
    {
        auto content =
            ReadWholeFile("engine/assets/shaders/shader_pbr.vert.spv");
        vertex_shader = device.CreateShaderModule((uint32_t*)content.data(),
                                                  content.size());
    }
    {
        auto content =
            ReadWholeFile("engine/assets/shaders/shader_pbr.frag.spv");
        frag_shader = device.CreateShaderModule((uint32_t*)content.data(),
                                                content.size());
    }

    initSolidPipeline(device, vertex_shader, frag_shader, res.m_render_pass);
    initLineFramePipeline(device, vertex_shader, frag_shader,
                          res.m_render_pass);
}

void GLTFRenderPass::RenderModel(const GLTFModel& model) {
    NICKEL_RETURN_IF_FALSE(model);
    
    m_models.push_back(model);
}

void GLTFRenderPass::ApplyDrawCall(RenderPassEncoder& encoder, bool wireframe) {
    auto& camera = nickel::Context::GetInst().GetCamera();

    if (wireframe) {
        encoder.BindGraphicsPipeline(m_line_frame_pipeline);
    } else {
        encoder.BindGraphicsPipeline(m_solid_pipeline);
    }

    encoder.SetPushConstant(ShaderStage::Vertex, camera.GetView().Ptr(),
                            sizeof(Mat44), sizeof(Mat44));

    for (auto& model : m_models) {
        GLTFModelImpl* impl = model.GetImpl();

        for (auto& scene : impl->scenes) {
            for (auto& node : scene.nodes) {
                visitGPUMesh(encoder, *model.GetImpl(), *node);
            }
        }
    }
}

bool GLTFRenderPass::NeedDraw() const noexcept {
    return !m_models.empty();
}

void GLTFRenderPass::End() {
    m_models.clear();
}

BindGroupLayout GLTFRenderPass::GetBindGroupLayout() {
    return m_bind_group_layout;
}

GraphicsPipeline::Descriptor GLTFRenderPass::getPipelineDescTmpl(
    ShaderModule& vertex_shader, ShaderModule& frag_shader,
    RenderPass& render_pass, PipelineLayout& layout) {
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
    // Position
    {
        GraphicsPipeline::Descriptor::BufferState buffer_state;
        GraphicsPipeline::Descriptor::BufferState::Attribute attr;
        attr.format = VertexFormat::Float32x3;
        attr.offset = 0;
        attr.shaderLocation = 0;
        buffer_state.attributes.push_back(attr);

        buffer_state.arrayStride = sizeof(float) * 3;
        buffer_state.stepMode =
            GraphicsPipeline::Descriptor::BufferState::StepMode::Vertex;
        desc.vertex.buffers.push_back(buffer_state);
    }

    // UV
    {
        GraphicsPipeline::Descriptor::BufferState buffer_state;
        GraphicsPipeline::Descriptor::BufferState::Attribute attr;
        attr.format = VertexFormat::Float32x2;
        attr.offset = 0;
        attr.shaderLocation = 1;
        buffer_state.attributes.push_back(attr);

        buffer_state.arrayStride = sizeof(float) * 2;
        buffer_state.stepMode =
            GraphicsPipeline::Descriptor::BufferState::StepMode::Vertex;
        desc.vertex.buffers.push_back(buffer_state);
    }

    // normal
    {
        GraphicsPipeline::Descriptor::BufferState buffer_state;
        GraphicsPipeline::Descriptor::BufferState::Attribute attr;
        attr.format = VertexFormat::Float32x3;
        attr.offset = 0;
        attr.shaderLocation = 2;
        buffer_state.attributes.push_back(attr);

        buffer_state.arrayStride = sizeof(float) * 3;
        buffer_state.stepMode =
            GraphicsPipeline::Descriptor::BufferState::StepMode::Vertex;
        desc.vertex.buffers.push_back(buffer_state);
    }

    // tangent
    {
        GraphicsPipeline::Descriptor::BufferState buffer_state;
        GraphicsPipeline::Descriptor::BufferState::Attribute attr;
        attr.format = VertexFormat::Float32x4;
        attr.offset = 0;
        attr.shaderLocation = 3;
        buffer_state.attributes.push_back(attr);

        buffer_state.arrayStride = sizeof(float) * 4;
        buffer_state.stepMode =
            GraphicsPipeline::Descriptor::BufferState::StepMode::Vertex;
        desc.vertex.buffers.push_back(buffer_state);
    }

    // blend state
    GraphicsPipeline::Descriptor::BlendState blend_state;
    desc.blend_state.push_back(blend_state);

    {
        GraphicsPipeline::Descriptor::DepthStencilState depth_stencil_state;
        depth_stencil_state.depthFormat = Format::D32_SFLOAT_S8_UINT;
        desc.depthStencil = depth_stencil_state;
    }

    desc.m_render_pass = render_pass;
    desc.layout = layout;

    return desc;
}

void GLTFRenderPass::initSolidPipeline(Device& device,
                                       ShaderModule& vertex_shader,
                                       ShaderModule& frag_shader,
                                       RenderPass& render_pass) {
    GraphicsPipeline::Descriptor desc = getPipelineDescTmpl(
        vertex_shader, frag_shader, render_pass, m_pipeline_layout);
    desc.subpass = 0;
    desc.primitive.topology = Topology::TriangleList;
    desc.primitive.cullMode = CullMode::Back;
    desc.primitive.frontFace = FrontFace::CCW;
    desc.primitive.polygonMode = PolygonMode::Fill;
    m_solid_pipeline = device.CreateGraphicPipeline(desc);
}

void GLTFRenderPass::initLineFramePipeline(Device& device,
                                           ShaderModule& vertex_shader,
                                           ShaderModule& frag_shader,
                                           RenderPass& render_pass) {
    GraphicsPipeline::Descriptor desc = getPipelineDescTmpl(
        vertex_shader, frag_shader, render_pass, m_pipeline_layout);
    desc.primitive.topology = Topology::TriangleList;
    desc.primitive.cullMode = CullMode::None;
    desc.primitive.frontFace = FrontFace::CCW;
    desc.primitive.polygonMode = PolygonMode::Line;
    m_line_frame_pipeline = device.CreateGraphicPipeline(desc);
}

void GLTFRenderPass::initPipelineLayout(Device& device) {
    PipelineLayout::Descriptor desc;

    {
        PipelineLayout::Descriptor::PushConstantRange range;
        range.offset = 0;
        range.shader_stage = ShaderStage::Vertex;
        range.size = sizeof(Mat44) * 2;
        desc.push_contants.push_back(range);
    }

    desc.layouts.push_back(m_bind_group_layout);
    m_pipeline_layout = device.CreatePipelineLayout(desc);
}

void GLTFRenderPass::initBindGroupLayout(Device& device) {
    BindGroupLayout::Descriptor desc;

    {
        BindGroupLayout::Entry entry;
        entry.shader_stage = ShaderStage::Vertex;
        entry.arraySize = 1;
        entry.type = BindGroupEntryType::UniformBuffer;
        desc.entries[0] = entry;
    }

    {
        BindGroupLayout::Entry entry;
        entry.shader_stage = ShaderStage::Fragment;
        entry.arraySize = 1;
        entry.type = BindGroupEntryType::UniformBufferDynamic;
        desc.entries[1] = entry;
    }

    {
        BindGroupLayout::Entry entry;
        entry.shader_stage = ShaderStage::Fragment;
        entry.arraySize = 1;
        entry.type = BindGroupEntryType::SampledImage;
        desc.entries[2] = entry;
    }

    {
        BindGroupLayout::Entry entry;
        entry.shader_stage = ShaderStage::Fragment;
        entry.arraySize = 1;
        entry.type = BindGroupEntryType::SampledImage;
        desc.entries[3] = entry;
    }

    {
        BindGroupLayout::Entry entry;
        entry.shader_stage = ShaderStage::Fragment;
        entry.arraySize = 1;
        entry.type = BindGroupEntryType::SampledImage;
        desc.entries[4] = entry;
    }

    {
        BindGroupLayout::Entry entry;
        entry.shader_stage = ShaderStage::Fragment;
        entry.arraySize = 1;
        entry.type = BindGroupEntryType::SampledImage;
        desc.entries[5] = entry;
    }

    {
        BindGroupLayout::Entry entry;
        entry.shader_stage = ShaderStage::Fragment;
        entry.arraySize = 1;
        entry.type = BindGroupEntryType::Sampler;
        desc.entries[6] = entry;
    }

    {
        BindGroupLayout::Entry entry;
        entry.shader_stage = ShaderStage::Fragment;
        entry.arraySize = 1;
        entry.type = BindGroupEntryType::Sampler;
        desc.entries[7] = entry;
    }

    {
        BindGroupLayout::Entry entry;
        entry.shader_stage = ShaderStage::Fragment;
        entry.arraySize = 1;
        entry.type = BindGroupEntryType::Sampler;
        desc.entries[8] = entry;
    }

    {
        BindGroupLayout::Entry entry;
        entry.shader_stage = ShaderStage::Fragment;
        entry.arraySize = 1;
        entry.type = BindGroupEntryType::Sampler;
        desc.entries[9] = entry;
    }

    {
        BindGroupLayout::Entry entry;
        entry.shader_stage = ShaderStage::Fragment;
        entry.arraySize = 1;
        entry.type = BindGroupEntryType::UniformBuffer;
        desc.entries[10] = entry;
    }

    m_bind_group_layout = device.CreateBindGroupLayout(desc);
}

void GLTFRenderPass::visitGPUMesh(RenderPassEncoder& encoder,
                                  GLTFModelImpl& model, GPUMesh& mesh) {
    for (auto& prim : mesh.primitives) {
        auto& mtl = model.materials[prim.material.value()];

        encoder.SetPushConstant(ShaderStage::Vertex, mesh.modelMat.Ptr(), 0,
                                sizeof(Mat44));

        encoder.SetBindGroup(0, mtl.GetImpl()->bindGroup);

        // position
        auto& pos_buffer_view = prim.posBufView;
        encoder.BindVertexBuffer(
            0, model.dataBuffers[pos_buffer_view.buffer.value()],
            pos_buffer_view.offset);

        // uv
        auto& uv_buffer_view = prim.uvBufView;
        encoder.BindVertexBuffer(
            1, model.dataBuffers[uv_buffer_view.buffer.value()],
            uv_buffer_view.offset);

        // normal
        auto& normal_buffer_view = prim.normBufView;
        encoder.BindVertexBuffer(
            2, model.dataBuffers[normal_buffer_view.buffer.value()],
            normal_buffer_view.offset);

        // tangent
        auto& tangent_buffer_view = prim.tanBufView;
        encoder.BindVertexBuffer(
            3, model.dataBuffers[tangent_buffer_view.buffer.value()],
            tangent_buffer_view.offset);

        if (prim.indicesBufView) {
            auto& indices_buffer_view = prim.indicesBufView;
            encoder.BindIndexBuffer(
                model.dataBuffers[indices_buffer_view.buffer.value()],
                prim.index_type, indices_buffer_view.offset);
            encoder.DrawIndexed(indices_buffer_view.count, 1, 0, 0, 0);
        } else {
            encoder.Draw(pos_buffer_view.count, 1, 0, 0);
        }
    }

    for (auto& child : mesh.children) {
        visitGPUMesh(encoder, model, *child);
    }
}

}  // namespace nickel::graphics