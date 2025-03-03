#pragma once
#include "nickel/common/transform.hpp"
#include "nickel/graphics/common_resource.hpp"
#include "nickel/graphics/gltf.hpp"
#include "nickel/graphics/lowlevel/device.hpp"
#include "nickel/graphics/lowlevel/graphics_pipeline.hpp"
#include "nickel/graphics/mesh.hpp"

namespace nickel::graphics {

class GLTFRenderPass {
public:
    GLTFRenderPass(Device device, CommonResource&);

    void RenderModel(const Transform&, const GLTFModel&);
    void ApplyDrawCall(RenderPassEncoder&, bool wireframe);
    bool NeedDraw() const noexcept;

    void End();

    BindGroupLayout GetBindGroupLayout();

private:
    struct GLTFModelData {
        Transform m_transform;
        GLTFModel m_model;
    };
    GraphicsPipeline m_solid_pipeline;
    GraphicsPipeline m_line_frame_pipeline;
    PipelineLayout m_pipeline_layout;
    BindGroupLayout m_bind_group_layout;
    std::vector<GLTFModelData> m_models;

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

    void visitGPUMesh(RenderPassEncoder& encoder, const Mat44& transform,
                      GLTFModelImpl& model);
};

}  // namespace nickel::graphics