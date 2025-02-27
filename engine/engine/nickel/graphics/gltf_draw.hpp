#pragma once
#include "nickel/graphics/mesh.hpp"
#include "nickel/graphics/common_resource.hpp"
#include "nickel/graphics/gltf.hpp"
#include "nickel/graphics/lowlevel/device.hpp"
#include "nickel/graphics/lowlevel/graphics_pipeline.hpp"

namespace nickel::graphics {

class GLTFRenderPass {
public:
    GLTFRenderPass(Device device, CommonResource&);

    void RenderModel(const GLTFModel&);
    void ApplyDrawCall(RenderPassEncoder&, bool wireframe);
    bool NeedDraw() const noexcept;

    void End();

    BindGroupLayout GetBindGroupLayout();

private:
    GraphicsPipeline m_solid_pipeline;
    GraphicsPipeline m_line_frame_pipeline;
    PipelineLayout m_pipeline_layout;
    BindGroupLayout m_bind_group_layout;
    std::vector<GLTFModel> m_models;

    GraphicsPipeline::Descriptor getPipelineDescTmpl(
        ShaderModule& vertex_shader, ShaderModule& frag_shader,
        RenderPass& render_pass, PipelineLayout& layout);
    void initSolidPipeline(Device& device, ShaderModule& vertex_shader,
                           ShaderModule& frag_shader, RenderPass& render_pass);
    void initLineFramePipeline(Device& device, ShaderModule& vertex_shader,
                               ShaderModule& frag_shader,
                               RenderPass& render_pass);
    void initPipelineLayout(Device& device);
    void initBindGroupLayout(Device& device);

    void visitGPUMesh(RenderPassEncoder& encoder, GLTFModelImpl& model,
                      Mesh& mesh);
};

}  // namespace nickel::graphics