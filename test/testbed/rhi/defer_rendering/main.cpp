#include "../common/camera.hpp"
#include "graphics/rhi/rhi.hpp"
#include "nickel.hpp"
#include "stb_image.h"
#include "vertex.hpp"

using namespace nickel::rhi;

APIPreference API = APIPreference::GL;

struct TextureBundle {
    Texture texture;
    TextureView view;
    Sampler sampler;
};

struct GBuffer {
    TextureBundle position;
    TextureBundle normal;
};

struct Context {
    PipelineLayout layout;
    PipelineLayout gBufferLayout;
    RenderPipeline gBufferPipeline;
    RenderPipeline renderPipeline;
    Buffer cubeVertexBuffer;
    Buffer renderVertexBuffer;
    Buffer uniformBuffer;
    Buffer lightBuffer;
    BindGroupLayout renderBindGroupLayout;
    BindGroupLayout gbufferBindGroupLayout;
    BindGroup gbufferBindGroup;
    BindGroup cubeBindGroup;
    BindGroup floorBindGroup;
    BindGroup renderBindGroup;
    Texture depth;
    TextureView depthView;
    TextureBundle imageBundle;
    TextureBundle whiteImageBundle;
    GBuffer gBuffer;
};

struct Light {
    nickel::cgmath::Vec3 pos;
    float padding1;
    nickel::cgmath::Vec3 color;
    float padding2;
};

constexpr size_t LightMaxNum = 32;
std::array<Light, LightMaxNum> gLights;

void initGBufferShaders(APIPreference api, Device device,
                 RenderPipeline::Descriptor& desc) {
    ShaderModule::Descriptor shaderDesc;

    if (api == APIPreference::Vulkan) {
        shaderDesc.code =
            nickel::ReadWholeFile<std::vector<char>>(
                "test/testbed/rhi/defer_rendering/gbuffer.vert.spv", std::ios::binary)
                .value();
        desc.vertex.module = device.CreateShaderModule(shaderDesc);

        shaderDesc.code =
            nickel::ReadWholeFile<std::vector<char>>(
                "test/testbed/rhi/defer_rendering/gbuffer.frag.spv", std::ios::binary)
                .value();
        desc.fragment.module = device.CreateShaderModule(shaderDesc);
    } else if (api == APIPreference::GL) {
        shaderDesc.code =
            nickel::ReadWholeFile<std::vector<char>>(
                "test/testbed/rhi/defer_rendering/gbuffer.glsl.vert")
                .value();
        desc.vertex.module = device.CreateShaderModule(shaderDesc);

        shaderDesc.code =
            nickel::ReadWholeFile<std::vector<char>>(
                "test/testbed/rhi/defer_rendering/gbuffer.glsl.frag")
                .value();
        desc.fragment.module = device.CreateShaderModule(shaderDesc);
    }
}

void initRenderShaders(APIPreference api, Device device,
                 RenderPipeline::Descriptor& desc) {
    ShaderModule::Descriptor shaderDesc;

    if (api == APIPreference::Vulkan) {
        shaderDesc.code =
            nickel::ReadWholeFile<std::vector<char>>(
                "test/testbed/rhi/defer_rendering/vert.spv", std::ios::binary)
                .value();
        desc.vertex.module = device.CreateShaderModule(shaderDesc);

        shaderDesc.code =
            nickel::ReadWholeFile<std::vector<char>>(
                "test/testbed/rhi/defer_rendering/frag.spv", std::ios::binary)
                .value();
        desc.fragment.module = device.CreateShaderModule(shaderDesc);
    } else if (api == APIPreference::GL) {
        shaderDesc.code =
            nickel::ReadWholeFile<std::vector<char>>(
                "test/testbed/rhi/defer_rendering/shader.glsl.vert")
                .value();
        desc.vertex.module = device.CreateShaderModule(shaderDesc);

        shaderDesc.code =
            nickel::ReadWholeFile<std::vector<char>>(
                "test/testbed/rhi/defer_rendering/shader.glsl.frag")
                .value();
        desc.fragment.module = device.CreateShaderModule(shaderDesc);
    }
}

void initCubeVertexBuffer(Context& ctx, Device& device) {
    Buffer::Descriptor bufferDesc;
    bufferDesc.size = sizeof(gVertices);
    bufferDesc.mappedAtCreation = true;
    bufferDesc.usage = BufferUsage::Vertex;
    ctx.cubeVertexBuffer = device.CreateBuffer(bufferDesc);
    auto data = ctx.cubeVertexBuffer.GetMappedRange();
    memcpy(data, gVertices.data(), sizeof(gVertices));
    ctx.cubeVertexBuffer.Unmap();
}

void initRenderVertexBuffer(Context& ctx, Device& device) {
    Buffer::Descriptor bufferDesc;
    bufferDesc.size = sizeof(gScreenVertices);
    bufferDesc.mappedAtCreation = true;
    bufferDesc.usage = BufferUsage::Vertex;

    ctx.renderVertexBuffer = device.CreateBuffer(bufferDesc);
    auto data = ctx.renderVertexBuffer.GetMappedRange();
    memcpy(data, gScreenVertices.data(), sizeof(gScreenVertices));
    ctx.renderVertexBuffer.Unmap();
}

void initUniformBuffer(Context& ctx, Adapter adapter, Device device,
                       nickel::Window& window, const Camera& camera) {
    Buffer::Descriptor bufferDesc;
    bufferDesc.usage = BufferUsage::Uniform;
    bufferDesc.mappedAtCreation = true;
    bufferDesc.size = 4 * 4 * 4 * 3;
    ctx.uniformBuffer = device.CreateBuffer(bufferDesc);
    auto data = (nickel::cgmath::Mat44*)ctx.uniformBuffer.GetMappedRange();
    auto& view = camera.View();
    auto& proj = camera.Proj();
    memcpy(data, &view, sizeof(view));
    memcpy(data + 1, &proj, sizeof(proj));
    if (!ctx.uniformBuffer.IsMappingCoherence()) {
        ctx.uniformBuffer.Flush();
    }
}

void initLightData() {
    std::uniform_real_distribution<float> dist1(0.3, 1);
    std::uniform_real_distribution<float> dist2(-9, 9);
    std::uniform_real_distribution<float> dist3(-20, 20);
    std::random_device e;
    for (auto& light : gLights) {
        light.color.Set(dist1(e), dist1(e), dist1(e));
        light.pos.Set(dist2(e), dist3(e), dist2(e));
    }
}

void changeLightData(Context& ctx) {
    constexpr float top = 5;
    constexpr float bottom = -5;
    for (auto& light : gLights) {
        light.pos.y = light.pos.y < bottom ? top : (light.pos.y - 0.01);
    }

    void* data = ctx.lightBuffer.GetMappedRange();
    memcpy(data, gLights.data(), sizeof(gLights));
    if (!ctx.lightBuffer.IsMappingCoherence()) {
       ctx.lightBuffer.Flush();
    }
}

void initLightUniformBuffer(Context& ctx, Device device) {
    Buffer::Descriptor bufferDesc;
    bufferDesc.usage = BufferUsage::Uniform;
    bufferDesc.mappedAtCreation = true;
    bufferDesc.size = sizeof(gLights);
    ctx.lightBuffer = device.CreateBuffer(bufferDesc);
    auto data = ctx.lightBuffer.GetMappedRange();
    memcpy(data, gLights.data(), sizeof(gLights));
    if (!ctx.lightBuffer.IsMappingCoherence()) {
        ctx.lightBuffer.Flush();
    }
}

void initRenderPipelineLayout(Context& ctx, Device& device) {
    PipelineLayout::Descriptor layoutDesc;
    layoutDesc.layouts.emplace_back(ctx.renderBindGroupLayout);
    ctx.layout = device.CreatePipelineLayout(layoutDesc);
}

void initGBufferPipelineLayout(Context& ctx, Device& device) {
    PipelineLayout::Descriptor layoutDesc;
    PushConstantRange pushConstant;
    pushConstant.size = sizeof(nickel::cgmath::Mat44);
    pushConstant.stage = ShaderStage::Vertex;
    layoutDesc.layouts.emplace_back(ctx.gbufferBindGroupLayout);
    layoutDesc.pushConstRanges.push_back(pushConstant);
    ctx.gBufferLayout = device.CreatePipelineLayout(layoutDesc);
}

void initRenderBindGroupAndLayout(Context& ctx, Device& device) {
    BindGroupLayout::Descriptor bindGroupLayoutDesc;

    // position sampler
    {
        Entry entry;
        entry.arraySize = 1;
        entry.binding.binding = 0;
        entry.visibility = ShaderStage::Fragment;
        SamplerBinding samplerBinding;
        samplerBinding.type = SamplerBinding::SamplerType::Filtering;
        samplerBinding.name = "positionSampler";
        samplerBinding.sampler = ctx.gBuffer.position.sampler;
        samplerBinding.view = ctx.gBuffer.position.view;
        entry.binding.entry = samplerBinding;
        bindGroupLayoutDesc.entries.emplace_back(entry);
    }

    // normal sampler
    {
        Entry entry;
        entry.arraySize = 1;
        entry.binding.binding = 1;
        entry.visibility = ShaderStage::Fragment;
        SamplerBinding samplerBinding;
        samplerBinding.type = SamplerBinding::SamplerType::Filtering;
        samplerBinding.name = "normalSampler";
        samplerBinding.sampler = ctx.gBuffer.normal.sampler;
        samplerBinding.view = ctx.gBuffer.normal.view;
        entry.binding.entry = samplerBinding;
        bindGroupLayoutDesc.entries.emplace_back(entry);
    }

    // uniform buffer
    {
        Entry entry;
        entry.arraySize = 1;
        entry.binding.binding = 2;
        entry.visibility = ShaderStage::Fragment;
        BufferBinding binding;
        binding.buffer = ctx.lightBuffer;
        binding.minBindingSize = ctx.lightBuffer.Size();
        binding.type = BufferType::Uniform;
        entry.binding.entry = binding;
        bindGroupLayoutDesc.entries.emplace_back(entry);
    }

    ctx.renderBindGroupLayout = device.CreateBindGroupLayout(bindGroupLayoutDesc);

    BindGroup::Descriptor bindGroupDesc;
    bindGroupDesc.layout = ctx.renderBindGroupLayout;
    ctx.renderBindGroup = device.CreateBindGroup(bindGroupDesc);

    // {
    //     BindGroup::Descriptor bindGroupDesc;
    //     bindGroupDesc.layout = ctx.renderBindGroupLayout;
    //     ctx.cubeBindGroup = device.CreateBindGroup(bindGroupDesc);
    // }

    // {
    //     BindGroup::Descriptor bindGroupDesc;
    //     BindingPoint bindingPoint;
    //     bindingPoint.binding = 1;
    //     SamplerBinding entry;
    //     entry.sampler = ctx.whiteImageBundle.sampler;
    //     entry.name = "mySampler";
    //     entry.view = ctx.whiteImageBundle.view;
    //     bindingPoint.entry = entry;
    //     bindGroupDesc.entries.push_back(bindingPoint);
    //     bindGroupDesc.layout = ctx.renderBindGroupLayout;
    //     ctx.floorBindGroup = device.CreateBindGroup(bindGroupDesc);
    // }
}

void initGBufferBindGroupAndLayout(Context& ctx, Device& device) {
    BindGroupLayout::Descriptor bindGroupLayoutDesc;

    // uniform buffer
    Entry entry;
    entry.arraySize = 1;
    entry.binding.binding = 0;
    entry.visibility = ShaderStage::Vertex;
    BufferBinding bufferBinding;
    bufferBinding.buffer = ctx.uniformBuffer;
    bufferBinding.hasDynamicOffset = false;
    bufferBinding.type = BufferType::Uniform;
    entry.binding.entry = bufferBinding;
    bindGroupLayoutDesc.entries.emplace_back(entry);

    ctx.gbufferBindGroupLayout = device.CreateBindGroupLayout(bindGroupLayoutDesc);

    BindGroup::Descriptor bindGroupDesc;
    bindGroupDesc.layout = ctx.gbufferBindGroupLayout;
    ctx.gbufferBindGroup = device.CreateBindGroup(bindGroupDesc);
}


void initDepthTexture(Context& ctx, Device& dev, nickel::Window& window) {
    Texture::Descriptor desc;
    desc.format = TextureFormat::DEPTH24_PLUS_STENCIL8;
    desc.size.width = window.Size().w;
    desc.size.height = window.Size().h;
    desc.size.depthOrArrayLayers = 1;
    desc.usage = TextureUsage::RenderAttachment;
    ctx.depth = dev.CreateTexture(desc);

    ctx.depthView = ctx.depth.CreateView();
}

void initGBuffer(Context& ctx, Device& dev, nickel::Window& window) {
    Texture::Descriptor desc;
    desc.size.width = window.Size().w;
    desc.size.height = window.Size().h;
    desc.size.depthOrArrayLayers = 1;
    desc.usage = Flags<TextureUsage>(TextureUsage::RenderAttachment) |
                 TextureUsage::TextureBinding;

    {
        desc.format = TextureFormat::RGBA32_FLOAT;
        ctx.gBuffer.position.texture = dev.CreateTexture(desc);
        ctx.gBuffer.position.view = ctx.gBuffer.position.texture.CreateView();
        ctx.gBuffer.position.sampler = dev.CreateSampler({});
    }

    {
        desc.format = TextureFormat::RGBA32_FLOAT;
        ctx.gBuffer.normal.texture = dev.CreateTexture(desc);
        ctx.gBuffer.normal.view = ctx.gBuffer.normal.texture.CreateView();
        ctx.gBuffer.normal.sampler = dev.CreateSampler({});
    }
}


void loadImage(Context& ctx, Device& dev) {
    int w, h;
    void* data = stbi_load("test/testbed/rhi/texture_cube/texture.jpg", &w, &h,
                           nullptr, STBI_rgb_alpha);

    Texture::Descriptor desc;
    desc.format = TextureFormat::RGBA8_UNORM_SRGB;
    desc.size.width = w;
    desc.size.height = h;
    desc.size.depthOrArrayLayers = 1;
    desc.usage = static_cast<uint32_t>(TextureUsage::TextureBinding) |
                 static_cast<uint32_t>(TextureUsage::CopyDst);
    ctx.imageBundle.texture = dev.CreateTexture(desc);

    Buffer::Descriptor bufferDesc;
    bufferDesc.mappedAtCreation = true;
    bufferDesc.usage = BufferUsage::CopySrc;
    bufferDesc.size = 4 * w * h;
    Buffer copyBuffer = dev.CreateBuffer(bufferDesc);

    void* bufData = copyBuffer.GetMappedRange();
    memcpy(bufData, data, bufferDesc.size);
    if (!copyBuffer.IsMappingCoherence()) {
        copyBuffer.Flush();
    }
    copyBuffer.Unmap();

    auto encoder = dev.CreateCommandEncoder();
    CommandEncoder::BufTexCopySrc src;
    src.buffer = copyBuffer;
    src.offset = 0;
    src.rowLength = w;
    src.rowsPerImage = h;
    CommandEncoder::BufTexCopyDst dst;
    dst.texture = ctx.imageBundle.texture;
    dst.aspect = TextureAspect::All;
    dst.miplevel = 0;
    encoder.CopyBufferToTexture(src, dst,
                                Extent3D{(uint32_t)w, (uint32_t)h, 1});
    auto buf = encoder.Finish();
    dev.GetQueue().Submit({buf});
    dev.WaitIdle();
    encoder.Destroy();

    ctx.imageBundle.view = ctx.imageBundle.texture.CreateView();
    ctx.imageBundle.sampler = dev.CreateSampler({});

    stbi_image_free(data);
    copyBuffer.Destroy();
}

void initWhiteImage(Context& ctx, Device& dev) {
    Texture::Descriptor desc;
    desc.format = TextureFormat::RGBA8_UNORM;
    desc.size.width = 1;
    desc.size.height = 1;
    desc.size.depthOrArrayLayers = 1;
    desc.usage = static_cast<uint32_t>(TextureUsage::TextureBinding) |
                 static_cast<uint32_t>(TextureUsage::CopyDst);
    ctx.whiteImageBundle.texture = dev.CreateTexture(desc);

    Buffer::Descriptor bufferDesc;
    bufferDesc.mappedAtCreation = true;
    bufferDesc.usage = BufferUsage::CopySrc;
    bufferDesc.size = 4;
    Buffer copyBuffer = dev.CreateBuffer(bufferDesc);

    void* bufData = copyBuffer.GetMappedRange();
    uint32_t color = 0xFFFFFFFF;
    memcpy(bufData, &color, bufferDesc.size);
    if (!copyBuffer.IsMappingCoherence()) {
        copyBuffer.Flush();
    }
    copyBuffer.Unmap();

    auto encoder = dev.CreateCommandEncoder();
    CommandEncoder::BufTexCopySrc src;
    src.buffer = copyBuffer;
    src.offset = 0;
    src.rowLength = 1;
    src.rowsPerImage = 1;
    CommandEncoder::BufTexCopyDst dst;
    dst.texture = ctx.whiteImageBundle.texture;
    dst.aspect = TextureAspect::All;
    dst.miplevel = 0;
    encoder.CopyBufferToTexture(src, dst, Extent3D{1, 1, 1});
    auto buf = encoder.Finish();
    dev.GetQueue().Submit({buf});
    dev.WaitIdle();
    encoder.Destroy();

    ctx.whiteImageBundle.view = ctx.whiteImageBundle.texture.CreateView();
    Sampler::Descriptor samplerDesc;
    samplerDesc.u = SamplerAddressMode::Repeat;
    samplerDesc.v = SamplerAddressMode::Repeat;
    samplerDesc.w = SamplerAddressMode::Repeat;
    ctx.whiteImageBundle.sampler = dev.CreateSampler(samplerDesc);

    copyBuffer.Destroy();
}

void initRenderPipeline(APIPreference api, Device device, Context& ctx, const nickel::Window& window) {
    RenderPipeline::Descriptor desc;

    desc.viewport.viewport.x = 0;
    desc.viewport.viewport.y = 0;
    desc.viewport.viewport.w = window.Size().w;
    desc.viewport.viewport.h = window.Size().h;
    desc.viewport.scissor.offset.x = 0;
    desc.viewport.scissor.offset.y = 0;
    desc.viewport.scissor.extent.width = window.Size().w;
    desc.viewport.scissor.extent.height = window.Size().h;

    initRenderShaders(api, device, desc);

    RenderPipeline::VertexState vertexState;
    RenderPipeline::BufferState bufferState;

    bufferState.attributes.push_back({VertexFormat::Float32x3, 0, 0});
    bufferState.attributes.push_back({VertexFormat::Float32x2, 12, 1});
    bufferState.arrayStride = 5 * 4;
    desc.vertex.buffers.emplace_back(bufferState);

    RenderPipeline::FragmentTarget target;
    target.format = TextureFormat::Presentation;
    desc.layout = ctx.layout;
    desc.fragment.targets.emplace_back(target);

    RenderPipeline::DepthStencilState depthStencilState;
    depthStencilState.depthFormat = ctx.depth.Format();
    depthStencilState.depthWriteEnabled = true;
    depthStencilState.depthCompare = CompareOp::Greater;
    desc.depthStencil = depthStencilState;

    ctx.renderPipeline = device.CreateRenderPipeline(desc);
}

void initGBufferPipeline(APIPreference api, Device device, Context& ctx, const nickel::Window& window) {
    RenderPipeline::Descriptor desc;

    initGBufferShaders(api, device, desc);

    desc.viewport.viewport.x = 0;
    desc.viewport.viewport.y = 0;
    desc.viewport.viewport.w = window.Size().w;
    desc.viewport.viewport.h = window.Size().h;
    desc.viewport.scissor.offset.x = 0;
    desc.viewport.scissor.offset.y = 0;
    desc.viewport.scissor.extent.width = window.Size().w;
    desc.viewport.scissor.extent.height = window.Size().h;

    RenderPipeline::VertexState vertexState;
    RenderPipeline::BufferState bufferState;

    bufferState.attributes.push_back({VertexFormat::Float32x3, 0, 0});
    bufferState.attributes.push_back({VertexFormat::Float32x2, 12, 1});
    bufferState.attributes.push_back({VertexFormat::Float32x3, 20, 2});
    bufferState.attributes.push_back({VertexFormat::Float32x3, 32, 3});
    bufferState.arrayStride = 11 * 4;
    desc.vertex.buffers.emplace_back(bufferState);
    desc.layout = ctx.gBufferLayout;

    RenderPipeline::FragmentTarget target;
    target.format = TextureFormat::RGBA32_FLOAT;
    desc.fragment.targets.emplace_back(target);
    target.format = TextureFormat::RGBA32_FLOAT;
    desc.fragment.targets.emplace_back(target);

    RenderPipeline::DepthStencilState depthStencilState;
    depthStencilState.depthFormat = ctx.depth.Format();
    depthStencilState.depthWriteEnabled = true;
    depthStencilState.depthCompare = CompareOp::Greater;
    desc.depthStencil = depthStencilState;

    ctx.gBufferPipeline = device.CreateRenderPipeline(desc);
}

void initNormalTangentMeshData() {
    uint32_t count = gVertices.size() / 3;
    for (int i = 0; i < count; i++) {
        auto& v1 = gVertices[i * 3];
        auto& v2 = gVertices[i * 3 + 1];
        auto& v3 = gVertices[i * 3 + 2];

        auto normal = nickel::cgmath::Normalize(
            (v2.position - v1.position).Cross(v3.position - v1.position));
        v1.normal = v2.normal = v3.normal = normal;

        auto tan = nickel::cgmath::GetNormalMapTangent(v1.position, v2.position, v3.position, v1.uv, v2.uv, v3.uv);
        v1.tangent = v2.tangent = v3.tangent = tan;
    }
}

void StartupSystem(gecs::commands cmds,
                   gecs::resource<gecs::mut<nickel::Window>> window) {
    auto& adapter =
        cmds.emplace_resource<Adapter>(window->Raw(), Adapter::Option{API});
    auto& device = cmds.emplace_resource<Device>(adapter.RequestDevice());
    auto& ctx = cmds.emplace_resource<Context>();
    auto& camera = cmds.emplace_resource<Camera>(
        adapter.RequestAdapterInfo().api, window->Size());

    camera.Move({0, 0, 20});
    SphericalCoordCameraProxy cameraProxy{camera, {}};
    cameraProxy.SetTheta(nickel::cgmath::PI / 4.0);
    cameraProxy.Update2Camera();

    initLightData();
    initNormalTangentMeshData();
    loadImage(ctx, device);
    initWhiteImage(ctx, device);
    initCubeVertexBuffer(ctx, device);
    initRenderVertexBuffer(ctx, device);
    initUniformBuffer(ctx, adapter, device, window.get(), camera);
    initLightUniformBuffer(ctx, device);
    initDepthTexture(ctx, device, window.get());
    initGBuffer(ctx, device, window.get());

    // init gbuffer pipeline
    initGBufferBindGroupAndLayout(ctx, device);
    initGBufferPipelineLayout(ctx, device);
    initGBufferPipeline(adapter.RequestAdapterInfo().api, device, ctx, window.get());

    // init render pipeline
    initRenderBindGroupAndLayout(ctx, device);
    initRenderPipelineLayout(ctx, device);
    initRenderPipeline(adapter.RequestAdapterInfo().api, device, ctx, window.get());
}

void UpdateGBuffer(gecs::resource<gecs::mut<nickel::rhi::Device>> device,
                  gecs::resource<gecs::mut<Context>> ctx) {
    RenderPass::Descriptor desc;
    RenderPass::Descriptor::ColorAttachment colorAtt;
    colorAtt.loadOp = AttachmentLoadOp::Clear;
    colorAtt.storeOp = AttachmentStoreOp::Store;
    colorAtt.clearValue = {0, 0, 0, 0};

    colorAtt.view = ctx->gBuffer.position.view;
    desc.colorAttachments.emplace_back(colorAtt);
    colorAtt.view = ctx->gBuffer.normal.view;
    desc.colorAttachments.emplace_back(colorAtt);

    desc.depthStencilAttachment =
        RenderPass::Descriptor::DepthStencilAttachment{};
    desc.depthStencilAttachment->view = ctx->depthView;
    desc.depthStencilAttachment->depthLoadOp = AttachmentLoadOp::Clear;
    desc.depthStencilAttachment->depthStoreOp = AttachmentStoreOp::Discard;
    desc.depthStencilAttachment->depthReadOnly = false;
    desc.depthStencilAttachment->depthClearValue = 0.0;

    auto encoder = device->CreateCommandEncoder();
    auto renderPass = encoder.BeginRenderPass(desc);
    renderPass.SetPipeline(ctx->gBufferPipeline);
    renderPass.SetVertexBuffer(0, ctx->cubeVertexBuffer, 0,
                               ctx->cubeVertexBuffer.Size());
    renderPass.SetBindGroup(ctx->gbufferBindGroup);
    constexpr float padding = 2;
    constexpr float cubeSize = 2;

    // render cubes
    for (int x = -2; x <= 2; x++) {
        for (int z = -2; z <= 2; z++) {
            auto model = nickel::cgmath::CreateTranslation(
                nickel::cgmath::Vec3(cubeSize * x, 0, cubeSize * z) +
                nickel::cgmath::Vec3{padding * x, 0, padding * z});
            renderPass.SetPushConstant(ShaderStage::Vertex, model.data, 0,
                                       sizeof(model.data));
            renderPass.Draw(gVertices.size(), 1, 0, 0);
        }
    }

    // render floor
    // renderPass.SetBindGroup(ctx->gbufferBindGroup);
    auto model = nickel::cgmath::CreateTranslation({0, -1.5, 0}) *
                 nickel::cgmath::CreateScale({10, 0.5, 10});
    renderPass.SetPushConstant(ShaderStage::Vertex, model.data, 0,
                               sizeof(model.data));
    renderPass.Draw(gVertices.size(), 1, 0, 0);

    renderPass.End();
    auto cmd = encoder.Finish();

    Queue queue = device->GetQueue();

    queue.Submit({cmd});
    device->WaitIdle();

    encoder.Destroy();
}

void RenderByGBuffer(gecs::resource<gecs::mut<nickel::rhi::Device>> device,
                  gecs::resource<gecs::mut<Context>> ctx) {
    RenderPass::Descriptor desc;
    RenderPass::Descriptor::ColorAttachment colorAtt;
    colorAtt.loadOp = AttachmentLoadOp::Clear;
    colorAtt.storeOp = AttachmentStoreOp::Store;
    colorAtt.clearValue = {0, 0, 0, 1};

    Texture::Descriptor textureDesc;
    textureDesc.format = TextureFormat::Presentation;
    auto texture = device->CreateTexture(textureDesc);
    auto view = texture.CreateView();
    colorAtt.view = view;
    desc.colorAttachments.emplace_back(colorAtt);

    auto encoder = device->CreateCommandEncoder();
    auto renderPass = encoder.BeginRenderPass(desc);
    renderPass.SetPipeline(ctx->renderPipeline);
    renderPass.SetVertexBuffer(0, ctx->renderVertexBuffer, 0,
                               ctx->renderVertexBuffer.Size());
    renderPass.SetBindGroup(ctx->renderBindGroup);
    renderPass.Draw(6, 1, 0, 0);
    renderPass.End();
    auto cmd = encoder.Finish();

    Queue queue = device->GetQueue();

    queue.Submit({cmd});
    device->SwapContext();

    encoder.Destroy();
    view.Destroy();
    texture.Destroy();
}

void LogicUpdate(gecs::resource<gecs::mut<Context>> ctx,
                 gecs::resource<gecs::mut<Camera>> camera,
                 gecs::resource<gecs::mut<Device>> device) {
    SphericalCoordCameraProxy cameraProxy{camera.get(), {}};
    constexpr float phiStep = 0.001;
    cameraProxy.SetPhi(cameraProxy.GetPhi() + phiStep);
    cameraProxy.Update2Camera();

    auto& view = camera->View();
    auto data = (nickel::cgmath::Mat44*)ctx->uniformBuffer.GetMappedRange();
    memcpy(data, &view, sizeof(view));
    if (!ctx->uniformBuffer.IsMappingCoherence()) {
        ctx->uniformBuffer.Flush();
    }

    changeLightData(ctx.get());
}

void ShutdownSystem(gecs::commands cmds,
                    gecs::resource<gecs::mut<Context>> ctx) {
    ctx->whiteImageBundle.sampler.Destroy();
    ctx->whiteImageBundle.view.Destroy();
    ctx->whiteImageBundle.texture.Destroy();
    ctx->imageBundle.sampler.Destroy();
    ctx->imageBundle.view.Destroy();
    ctx->imageBundle.texture.Destroy();
    ctx->depthView.Destroy();
    ctx->depth.Destroy();
    ctx->cubeBindGroup.Destroy();
    ctx->floorBindGroup.Destroy();
    ctx->renderBindGroupLayout.Destroy();
    ctx->uniformBuffer.Destroy();
    ctx->cubeVertexBuffer.Destroy();
    ctx->layout.Destroy();
    ctx->renderPipeline.Destroy();
    cmds.remove_resource<Device>();
    cmds.remove_resource<Adapter>();
}

void BootstrapSystem(gecs::world& world,
                     typename gecs::world::registry_type& reg) {
    auto& args = reg.res<nickel::CmdLineArgs>()->Args();
#ifdef NICKEL_HAS_VULKAN
    bool isVulkanBackend =
        args.size() == 1 ? true : (args[1] == "--api=gl" ? false : true);
    if (isVulkanBackend) {
        API = APIPreference::Vulkan;
    } else {
        API = APIPreference::GL;
    }
#else
    bool isVulkanBackend = true;
#endif
    nickel::Window& window = reg.commands().emplace_resource<nickel::Window>(
        "defered rendering", 1024, 720, API == APIPreference::Vulkan);

    reg
        // startup systems
        .regist_startup_system<nickel::VideoSystemInit>()
        .regist_startup_system<nickel::EventPollerInit>()
        .regist_startup_system<nickel::InputSystemInit>()
        .regist_startup_system<StartupSystem>()
        // shutdown systems
        .regist_shutdown_system<ShutdownSystem>()
        .regist_shutdown_system<nickel::EngineShutdown>()
        // update systems
        .regist_update_system<nickel::VideoSystemUpdate>()
        // other input handle event must put here(after mouse/keyboard update)
        .regist_update_system<nickel::Mouse::Update>()
        .regist_update_system<nickel::Keyboard::Update>()
        .regist_update_system<nickel::HandleInputEvents>()
        .regist_update_system<UpdateGBuffer>()
        .regist_update_system<RenderByGBuffer>()
        .regist_update_system<LogicUpdate>();
}