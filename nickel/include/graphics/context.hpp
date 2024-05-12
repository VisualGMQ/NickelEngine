#pragma once

#include "common/cgmath.hpp"
#include "common/log_tag.hpp"
#include "graphics/material.hpp"
#include "graphics/texture.hpp"
#include "graphics/vertex.hpp"
#include "video/event.hpp"
#include <stack>

namespace nickel {

class RenderContext;

struct GPUMesh2D final {
    rhi::Buffer verticesBuffer;
    rhi::Buffer indicesBuffer;
    uint32_t elemCount;

    ~GPUMesh2D();
};

struct Mesh2D final {
    std::vector<Vertex2D> vertices;
    std::vector<uint32_t> indices;

    // std::unique_ptr<GPUMesh2D> CreateGPUMesh2D(rhi::Device);
};

struct Render2DContext {
    rhi::RenderPipeline pipeline;
    rhi::PipelineLayout pipelineLayout;
    rhi::BindGroupLayout bindGroupLayout;
    rhi::ShaderModule vertexShader;
    rhi::ShaderModule fragmentShader;
    rhi::BindGroup defaultBindGroup;    // bind a white texture
    rhi::Buffer vertexBuffer;           // for 2D texture vertices
    rhi::Buffer indexBuffer;            // for 2D texture vertices

    Render2DContext(rhi::Adapter, rhi::Device,
                    const cgmath::Rect& viewport, RenderContext&);
    ~Render2DContext();

    // get sampler(will create when not exists)
    rhi::Sampler GetSampler(rhi::SamplerAddressMode u,
                            rhi::SamplerAddressMode v, rhi::Filter min,
                            rhi::Filter mag);

    void RecreatePipeline(rhi::APIPreference api, RenderContext& ctx);

    uint32_t GenVertexSlot();
    void ReuseVertexSlot(uint32_t);

private:
    static constexpr size_t MaxRectCount = 1024;
    static constexpr size_t VertexBufferSize = sizeof(Vertex2D) * MaxRectCount * 4;

    rhi::Device device_;
    std::unordered_map<uint32_t, rhi::Sampler> samplers_;
    std::stack<uint32_t> usableVertexSlots_;

    rhi::PipelineLayout createPipelineLayout();
    void initPipelineShader(rhi::APIPreference);
    rhi::BindGroupLayout createBindGroupLayout(bool supportSeparateSampler,
                                               RenderContext& ctx);
    rhi::RenderPipeline createPipeline(rhi::APIPreference, RenderContext&);
    void initSamplers();
    rhi::BindGroup createDefaultBindGroup();
    void initBuffers();
    void initUsableVertexSlots();
};

struct Render3DContext {
    rhi::RenderPipeline pipeline;
    rhi::PipelineLayout pipelineLayout;
    rhi::BindGroupLayout bindGroupLayout;
    rhi::ShaderModule vertexShader;
    rhi::ShaderModule fragmentShader;

    Render3DContext(rhi::Adapter, rhi::Device, RenderContext&);
    ~Render3DContext();

    void RecreatePipeline(rhi::APIPreference api, RenderContext& ctx);

private:
    rhi::Device device_;

    rhi::PipelineLayout createPipelineLayout();
    void initPipelineShader(rhi::APIPreference);
    rhi::BindGroupLayout createBindGroupLayout(bool supportSeparateSampler,
                                               RenderContext& ctx);
    rhi::RenderPipeline createPipeline(rhi::APIPreference, RenderContext&);
};

/**
 * @brief [resource][inner] render context
 */
struct RenderContext final {
    static void OnWindowResize(const WindowResizeEvent&,
                               gecs::resource<gecs::mut<RenderContext>>);

    RenderContext(rhi::Adapter, rhi::Device, const cgmath::Vec2& windowSize);
    ~RenderContext();

    void RecreatePipeline(const cgmath::Vec2& size);

    std::unique_ptr<Render2DContext> ctx2D;
    std::unique_ptr<Render3DContext> ctx3D;

    // common resources
    rhi::Buffer mvpBuffer;
    rhi::Buffer cameraBuffer;
    rhi::Texture depthTexture;
    rhi::TextureView depthTextureView;
    rhi::Texture whiteTexture;
    rhi::TextureView whiteTextureView;
    rhi::Texture blackTexture;
    rhi::TextureView blackTextureView;
    rhi::Texture defaultNormalTexture;
    rhi::TextureView defaultNormalTextureView;
    rhi::Sampler defaultSampler;  // for default textures and glTF

    // temporary vars during rendering
    rhi::Texture presentTexture;
    rhi::TextureView presentTextureView;
    rhi::CommandEncoder encoder;
    rhi::CommandBuffer cmd;

private:
    void initDepthTexture(rhi::Device, const nickel::cgmath::Vec2& size);
    void initMVPBuffer(rhi::Device);
    rhi::Buffer createCameraBuffer(rhi::Device);
    rhi::Sampler createDefaultSampler(rhi::Device);
    rhi::Texture createSingleValueTexture(rhi::Device, uint32_t color);
};

}  // namespace nickel