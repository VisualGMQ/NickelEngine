#include "nickel/graphics/gltf_draw.hpp"

#include "nickel/common/common.hpp"
#include "nickel/common/macro.hpp"
#include "nickel/graphics/internal/gltf_model_impl.hpp"
#include "nickel/graphics/internal/material3d_impl.hpp"
#include "nickel/graphics/internal/mesh_impl.hpp"
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

void GLTFRenderPass::RenderModel(const Transform& transform,
                                 const GLTFModel& model) {
    NICKEL_RETURN_IF_FALSE(model);

    m_models.push_back({transform, model});
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

    for (auto& [transform, model] : m_models) {
        GLTFModelImpl* impl = model.GetImpl();
        visitGPUMesh(encoder, transform.ToMat(), *impl);
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

    desc.m_primitive.m_topology = Topology::LineStrip;
    desc.m_primitive.m_cull_mode = CullMode::None;
    desc.m_primitive.m_front_face = FrontFace::CCW;
    desc.m_primitive.m_unclipped_depth = false;
    desc.m_primitive.m_strip_index_format = StripIndexFormat::Uint32;

    // input vertex state
    // Position
    {
        GraphicsPipeline::Descriptor::BufferState buffer_state;
        GraphicsPipeline::Descriptor::BufferState::Attribute attr;
        attr.m_format = VertexFormat::Float32x3;
        attr.m_offset = 0;
        attr.m_shader_location = 0;
        buffer_state.m_attributes.push_back(attr);

        buffer_state.m_array_stride = sizeof(float) * 3;
        buffer_state.m_step_mode =
            GraphicsPipeline::Descriptor::BufferState::StepMode::Vertex;
        desc.m_vertex.m_buffers.push_back(buffer_state);
    }

    // UV
    {
        GraphicsPipeline::Descriptor::BufferState buffer_state;
        GraphicsPipeline::Descriptor::BufferState::Attribute attr;
        attr.m_format = VertexFormat::Float32x2;
        attr.m_offset = 0;
        attr.m_shader_location = 1;
        buffer_state.m_attributes.push_back(attr);

        buffer_state.m_array_stride = sizeof(float) * 2;
        buffer_state.m_step_mode =
            GraphicsPipeline::Descriptor::BufferState::StepMode::Vertex;
        desc.m_vertex.m_buffers.push_back(buffer_state);
    }

    // normal
    {
        GraphicsPipeline::Descriptor::BufferState buffer_state;
        GraphicsPipeline::Descriptor::BufferState::Attribute attr;
        attr.m_format = VertexFormat::Float32x3;
        attr.m_offset = 0;
        attr.m_shader_location = 2;
        buffer_state.m_attributes.push_back(attr);

        buffer_state.m_array_stride = sizeof(float) * 3;
        buffer_state.m_step_mode =
            GraphicsPipeline::Descriptor::BufferState::StepMode::Vertex;
        desc.m_vertex.m_buffers.push_back(buffer_state);
    }

    // tangent
    {
        GraphicsPipeline::Descriptor::BufferState buffer_state;
        GraphicsPipeline::Descriptor::BufferState::Attribute attr;
        attr.m_format = VertexFormat::Float32x4;
        attr.m_offset = 0;
        attr.m_shader_location = 3;
        buffer_state.m_attributes.push_back(attr);

        buffer_state.m_array_stride = sizeof(float) * 4;
        buffer_state.m_step_mode =
            GraphicsPipeline::Descriptor::BufferState::StepMode::Vertex;
        desc.m_vertex.m_buffers.push_back(buffer_state);
    }

    // blend state
    GraphicsPipeline::Descriptor::BlendState blend_state;
    desc.m_blend_state.push_back(blend_state);

    {
        GraphicsPipeline::Descriptor::DepthStencilState depth_stencil_state;
        depth_stencil_state.m_depth_format = Format::D32_SFLOAT_S8_UINT;
        desc.m_depth_stencil = depth_stencil_state;
    }

    desc.m_render_pass = render_pass;
    desc.m_layout = layout;

    return desc;
}

void GLTFRenderPass::initSolidPipeline(Device& device,
                                       ShaderModule& vertex_shader,
                                       ShaderModule& frag_shader,
                                       RenderPass& render_pass) {
    GraphicsPipeline::Descriptor desc = getPipelineDescTmpl(
        vertex_shader, frag_shader, render_pass, m_pipeline_layout);
    desc.m_subpass = 0;
    desc.m_primitive.m_topology = Topology::TriangleList;
    desc.m_primitive.m_cull_mode = CullMode::Back;
    desc.m_primitive.m_front_face = FrontFace::CCW;
    desc.m_primitive.m_polygon_mode = PolygonMode::Fill;
    m_solid_pipeline = device.CreateGraphicPipeline(desc);
}

void GLTFRenderPass::initLineFramePipeline(Device& device,
                                           ShaderModule& vertex_shader,
                                           ShaderModule& frag_shader,
                                           RenderPass& render_pass) {
    GraphicsPipeline::Descriptor desc = getPipelineDescTmpl(
        vertex_shader, frag_shader, render_pass, m_pipeline_layout);
    desc.m_primitive.m_topology = Topology::TriangleList;
    desc.m_primitive.m_cull_mode = CullMode::None;
    desc.m_primitive.m_front_face = FrontFace::CCW;
    desc.m_primitive.m_polygon_mode = PolygonMode::Line;
    m_line_frame_pipeline = device.CreateGraphicPipeline(desc);
}

void GLTFRenderPass::initPipelineLayout(Device& device) {
    PipelineLayout::Descriptor desc;

    {
        PipelineLayout::Descriptor::PushConstantRange range;
        range.m_offset = 0;
        range.m_shader_stage = ShaderStage::Vertex;
        range.m_size = sizeof(Mat44) * 2;
        desc.m_push_contants.push_back(range);
    }

    desc.m_layouts.push_back(m_bind_group_layout);
    m_pipeline_layout = device.CreatePipelineLayout(desc);
}

void GLTFRenderPass::initBindGroupLayout(Device& device) {
    BindGroupLayout::Descriptor desc;

    {
        BindGroupLayout::Entry entry;
        entry.m_shader_stage = ShaderStage::Vertex;
        entry.m_array_size = 1;
        entry.m_type = BindGroupEntryType::UniformBuffer;
        desc.m_entries[0] = entry;
    }

    {
        BindGroupLayout::Entry entry;
        entry.m_shader_stage = ShaderStage::Fragment;
        entry.m_array_size = 1;
        entry.m_type = BindGroupEntryType::UniformBufferDynamic;
        desc.m_entries[1] = entry;
    }

    {
        BindGroupLayout::Entry entry;
        entry.m_shader_stage = ShaderStage::Fragment;
        entry.m_array_size = 1;
        entry.m_type = BindGroupEntryType::SampledImage;
        desc.m_entries[2] = entry;
    }

    {
        BindGroupLayout::Entry entry;
        entry.m_shader_stage = ShaderStage::Fragment;
        entry.m_array_size = 1;
        entry.m_type = BindGroupEntryType::SampledImage;
        desc.m_entries[3] = entry;
    }

    {
        BindGroupLayout::Entry entry;
        entry.m_shader_stage = ShaderStage::Fragment;
        entry.m_array_size = 1;
        entry.m_type = BindGroupEntryType::SampledImage;
        desc.m_entries[4] = entry;
    }

    {
        BindGroupLayout::Entry entry;
        entry.m_shader_stage = ShaderStage::Fragment;
        entry.m_array_size = 1;
        entry.m_type = BindGroupEntryType::SampledImage;
        desc.m_entries[5] = entry;
    }

    {
        BindGroupLayout::Entry entry;
        entry.m_shader_stage = ShaderStage::Fragment;
        entry.m_array_size = 1;
        entry.m_type = BindGroupEntryType::Sampler;
        desc.m_entries[6] = entry;
    }

    {
        BindGroupLayout::Entry entry;
        entry.m_shader_stage = ShaderStage::Fragment;
        entry.m_array_size = 1;
        entry.m_type = BindGroupEntryType::Sampler;
        desc.m_entries[7] = entry;
    }

    {
        BindGroupLayout::Entry entry;
        entry.m_shader_stage = ShaderStage::Fragment;
        entry.m_array_size = 1;
        entry.m_type = BindGroupEntryType::Sampler;
        desc.m_entries[8] = entry;
    }

    {
        BindGroupLayout::Entry entry;
        entry.m_shader_stage = ShaderStage::Fragment;
        entry.m_array_size = 1;
        entry.m_type = BindGroupEntryType::Sampler;
        desc.m_entries[9] = entry;
    }

    // view buffer
    {
        BindGroupLayout::Entry entry;
        entry.m_shader_stage = ShaderStage::Fragment;
        entry.m_array_size = 1;
        entry.m_type = BindGroupEntryType::UniformBuffer;
        desc.m_entries[10] = entry;
    }

    m_bind_group_layout = device.CreateBindGroupLayout(desc);
}

void GLTFRenderPass::visitGPUMesh(RenderPassEncoder& encoder,
                                  const Mat44& transform,
                                  GLTFModelImpl& model) {
    Mat44 model_mat = transform * model.m_transform;
    if (model.m_mesh) {
        for (auto& prim : model.m_mesh.GetImpl()->m_primitives) {
            auto& mtl = prim.m_material;

            encoder.SetPushConstant(ShaderStage::Vertex, model_mat.Ptr(), 0,
                                    sizeof(Mat44));

            encoder.SetBindGroup(0, mtl.GetImpl()->m_bind_group);

            // position
            auto& pos_buffer_view = prim.m_pos_buf_view;
            encoder.BindVertexBuffer(0, pos_buffer_view.m_buffer,
                                     pos_buffer_view.m_offset);

            // uv
            auto& uv_buffer_view = prim.m_uv_buf_view;
            encoder.BindVertexBuffer(1, uv_buffer_view.m_buffer,
                                     uv_buffer_view.m_offset);

            // normal
            auto& normal_buffer_view = prim.m_norm_buf_view;
            encoder.BindVertexBuffer(2, normal_buffer_view.m_buffer,
                                     normal_buffer_view.m_offset);

            // tangent
            auto& tangent_buffer_view = prim.m_tan_buf_view;
            encoder.BindVertexBuffer(3, tangent_buffer_view.m_buffer,
                                     tangent_buffer_view.m_offset);

            if (prim.m_indices_buf_view) {
                auto& indices_buffer_view = prim.m_indices_buf_view;
                encoder.BindIndexBuffer(indices_buffer_view.m_buffer,
                                        prim.m_index_type,
                                        indices_buffer_view.m_offset);
                encoder.DrawIndexed(indices_buffer_view.m_count, 1, 0, 0, 0);
            } else {
                encoder.Draw(pos_buffer_view.m_count, 1, 0, 0);
            }
        }
    }

    for (auto& child : model.m_children) {
        visitGPUMesh(encoder, model_mat, *child.GetImpl());
    }
}

}  // namespace nickel::graphics