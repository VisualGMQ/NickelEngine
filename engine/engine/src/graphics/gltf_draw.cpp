#include "nickel/graphics/gltf_draw.hpp"

#include "nickel/common/common.hpp"

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
        attr.format = VertexFormat::Float32x2;
        attr.offset = sizeof(float) * 3;
        attr.shaderLocation = 1;
        buffer_state.attributes.push_back(attr);
    }
    {
        GraphicsPipeline::Descriptor::BufferState::Attribute attr;
        attr.format = VertexFormat::Float32x3;
        attr.offset = sizeof(float) * 5;
        attr.shaderLocation = 2;
        buffer_state.attributes.push_back(attr);
    }
    {
        GraphicsPipeline::Descriptor::BufferState::Attribute attr;
        attr.format = VertexFormat::Float32x4;
        attr.offset = sizeof(float) * 8;
        attr.shaderLocation = 3;
        buffer_state.attributes.push_back(attr);
    }

    buffer_state.arrayStride = sizeof(float) * 12;
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
    desc.layout = layout;

    return desc;
}

void GLTFRenderPass::initSolidPipeline(Device& device,
                                       ShaderModule& vertex_shader,
                                       ShaderModule& frag_shader,
                                       RenderPass& render_pass) {
    GraphicsPipeline::Descriptor desc = getPipelineDescTmpl(
        vertex_shader, frag_shader, render_pass, m_pipeline_layout);
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
    m_solid_pipeline = device.CreateGraphicPipeline(desc);
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
        entry.type = BindGroupEntryType::UniformBuffer;
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

}  // namespace nickel::graphics