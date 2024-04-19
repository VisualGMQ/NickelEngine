#pragma once

#include "common/cgmath.hpp"
#include "common/log_tag.hpp"
#include "graphics/material.hpp"
#include "graphics/texture.hpp"
#include "graphics/vertex.hpp"

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

    std::unique_ptr<GPUMesh2D> identityRectMesh_;
    std::unique_ptr<Material2D> defaultMaterial_;

    Render2DContext(rhi::APIPreference, rhi::Device,
                    const cgmath::Rect& viewport, RenderContext&);
    ~Render2DContext();

    // get sampler(will create when not exists)
    rhi::Sampler GetSampler(rhi::SamplerAddressMode u,
                            rhi::SamplerAddressMode v, rhi::Filter min,
                            rhi::Filter mag);

private:
    rhi::Device device_;
    std::unordered_map<uint32_t, rhi::Sampler> samplers_;

    rhi::PipelineLayout createPipelineLayout();
    void initPipelineShader(rhi::RenderPipeline::Descriptor&,
                            rhi::APIPreference);
    rhi::BindGroupLayout createBindGroupLayout(RenderContext& ctx);
    rhi::RenderPipeline createPipeline(rhi::APIPreference,
                                       const nickel::cgmath::Rect& viewport,
                                       RenderContext&);
    void initSamplers();
    std::unique_ptr<Material2D> createDefaultMaterial();
    std::unique_ptr<GPUMesh2D> createIdentityRectMesh();
};

struct Render3DContext {
    rhi::RenderPipeline pipeline;
    rhi::PipelineLayout pipelineLayout;
    rhi::BindGroupLayout bindGroupLayout;

    Render3DContext(rhi::APIPreference, rhi::Device,
                    const cgmath::Rect& viewport, RenderContext&);
    ~Render3DContext();

private:
    rhi::Device device_;

    rhi::PipelineLayout createPipelineLayout();
    void initPipelineShader(rhi::RenderPipeline::Descriptor&,
                            rhi::APIPreference);
    rhi::BindGroupLayout createBindGroupLayout(RenderContext& ctx);
    rhi::RenderPipeline createPipeline(rhi::APIPreference,
                                       const nickel::cgmath::Rect& viewport,
                                       RenderContext&);
};

/**
 * @brief [resource][inner] render context
 */
struct RenderContext final {
    RenderContext(rhi::APIPreference, rhi::Device,
                  const cgmath::Vec2& windowSize);
    ~RenderContext();

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

private:
    void initDepthTexture(rhi::Device, const nickel::cgmath::Vec2& size);
    void initMVPBuffer(rhi::Device);
    rhi::Buffer createCameraBuffer(rhi::Device);
    rhi::Sampler createDefaultSampler(rhi::Device);
    rhi::Texture createSingleValueTexture(rhi::Device, uint32_t color);
};

}  // namespace nickel