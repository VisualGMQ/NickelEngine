#include "graphics/rhi/rhi.hpp"
#include "nickel.hpp"
#include "stb_image.h"
#include "vertex.hpp"

using namespace nickel::rhi;

APIPreference API = APIPreference::GL;

struct Context {
    PipelineLayout layout;
    RenderPipeline pipeline;
    Buffer vertexBuffer;
    Buffer uniformBuffer;
    BindGroupLayout bindGroupLayout;
    BindGroup debugBindGroup;
    Texture depth;
    TextureView depthView;
    Texture image;
    TextureView imageView;
    Sampler sampler;
};

struct MVP {
    nickel::cgmath::Mat44 model, view, proj;
} mvp;

void initShaders(APIPreference api, Device device,
                 RenderPipeline::Descriptor& desc) {
    ShaderModule::Descriptor shaderDesc;

    if (api == APIPreference::Vulkan) {
        shaderDesc.code =
            nickel::ReadWholeFile<std::vector<char>>(
                "test/testbed/rhi/texture_cube/resources/vert.spv", std::ios::binary)
                .value();
        desc.vertex.module = device.CreateShaderModule(shaderDesc);

        shaderDesc.code =
            nickel::ReadWholeFile<std::vector<char>>(
                "test/testbed/rhi/texture_cube/resources/frag.spv", std::ios::binary)
                .value();
        desc.fragment.module = device.CreateShaderModule(shaderDesc);
    } else if (api == APIPreference::GL) {
#ifdef NICKEL_HAS_GL
        shaderDesc.code =
            nickel::ReadWholeFile<std::vector<char>>(
                "test/testbed/rhi/texture_cube/resources/shader.glsl.vert")
                .value();
        desc.vertex.module = device.CreateShaderModule(shaderDesc);

        shaderDesc.code =
            nickel::ReadWholeFile<std::vector<char>>(
                "test/testbed/rhi/texture_cube/resources/shader.glsl.frag")
                .value();
        desc.fragment.module = device.CreateShaderModule(shaderDesc);
#else
        shaderDesc.code =
            nickel::ReadWholeFile<std::vector<char>>(
                "test/testbed/rhi/texture_cube/resources/shader.es2.glsl.vert")
                .value();
        desc.vertex.module = device.CreateShaderModule(shaderDesc);

        shaderDesc.code =
            nickel::ReadWholeFile<std::vector<char>>(
                "test/testbed/rhi/texture_cube/resources/shader.es2.glsl.frag")
                .value();
        desc.fragment.module = device.CreateShaderModule(shaderDesc);
#endif
    }
}

void initVertexBuffer(Context& ctx, Device& device) {
    Buffer::Descriptor bufferDesc;
    bufferDesc.size = sizeof(gVertices);
    bufferDesc.mappedAtCreation = true;
    bufferDesc.usage = BufferUsage::Vertex;
    ctx.vertexBuffer = device.CreateBuffer(bufferDesc);
    auto data = ctx.vertexBuffer.GetMappedRange();
    memcpy(data, gVertices.data(), sizeof(gVertices));
    ctx.vertexBuffer.Unmap();
}

void initUniformBuffer(Context& ctx, Adapter adapter, Device device,
                       nickel::Window& window) {
    mvp.proj = nickel::cgmath::CreatePersp(
        nickel::cgmath::Deg2Rad(45.0f), window.Size().w / window.Size().h, 0.1,
        100, adapter.RequestAdapterInfo().api == APIPreference::GL);
    mvp.view = nickel::cgmath::CreateTranslation(nickel::cgmath::Vec3{0, 0, 0});
    mvp.model = nickel::cgmath::Mat44::Identity();

    Buffer::Descriptor bufferDesc;
    bufferDesc.usage = BufferUsage::Uniform;
    bufferDesc.mappedAtCreation = true;
    bufferDesc.size = 4 * 4 * 4 * 3;
    ctx.uniformBuffer = device.CreateBuffer(bufferDesc);
    void* data = ctx.uniformBuffer.GetMappedRange();
    memcpy(data, &mvp, sizeof(mvp));
    if (!ctx.uniformBuffer.IsMappingCoherence()) {
        ctx.uniformBuffer.Flush();
    }
}

void initSampler(Context& ctx, Device& device) {
    Sampler::Descriptor desc;
    ctx.sampler = device.CreateSampler(desc);
}

void initPipelineLayout(Context& ctx, Device& device) {
    PipelineLayout::Descriptor layoutDesc;
    layoutDesc.layouts.emplace_back(ctx.bindGroupLayout);
    ctx.layout = device.CreatePipelineLayout(layoutDesc);
}

void initBindGroupLayout(Context& ctx, Device& device) {
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

    // sampler
    entry.arraySize = 1;
    entry.binding.binding = 1;
    entry.visibility = ShaderStage::Fragment;
    SamplerBinding samplerBinding;
    samplerBinding.type = SamplerBinding::SamplerType::Filtering;
    samplerBinding.name = "mySampler";
    samplerBinding.sampler = ctx.sampler;
    samplerBinding.view = ctx.imageView;
    entry.binding.entry = samplerBinding;
    bindGroupLayoutDesc.entries.emplace_back(entry);

    ctx.bindGroupLayout = device.CreateBindGroupLayout(bindGroupLayoutDesc);

    BindGroup::Descriptor bindGroupDesc;
    bindGroupDesc.layout = ctx.bindGroupLayout;
    ctx.debugBindGroup = device.CreateBindGroup(bindGroupDesc);
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

void initImage(Context& ctx, Device& dev) {
    int w, h;
    void* data =
        stbi_load("test/testbed/rhi/texture_cube/resources/texture.jpg", &w, &h,
                  nullptr, STBI_rgb_alpha);

    Texture::Descriptor desc;
    desc.format = TextureFormat::RGBA8_UNORM;
    desc.size.width = w;
    desc.size.height = h;
    desc.size.depthOrArrayLayers = 1;
    desc.usage = static_cast<uint32_t>(TextureUsage::TextureBinding) |
                 static_cast<uint32_t>(TextureUsage::CopyDst);
    ctx.image = dev.CreateTexture(desc);

    Buffer::Descriptor bufferDesc;
    bufferDesc.mappedAtCreation = true;
    bufferDesc.usage = BufferUsage::CopySrc;
    bufferDesc.size = 4 * w * h;
    Buffer copyBuffer = dev.CreateBuffer(bufferDesc);

    void* bufData = copyBuffer.GetMappedRange();
    memcpy(bufData, data, bufferDesc.size);
    copyBuffer.Unmap();

    auto encoder = dev.CreateCommandEncoder();
    CommandEncoder::BufTexCopySrc src;
    src.buffer = copyBuffer;
    src.offset = 0;
    src.rowLength = w;
    src.rowsPerImage = h;
    CommandEncoder::BufTexCopyDst dst;
    dst.texture = ctx.image;
    dst.aspect = TextureAspect::ColorOnly;
    dst.miplevel = 0;
    encoder.CopyBufferToTexture(src, dst,
                                Extent3D{(uint32_t)w, (uint32_t)h, 1});
    auto buf = encoder.Finish();
    dev.GetQueue().Submit({buf});
    dev.WaitIdle();
    encoder.Destroy();

    ctx.imageView = ctx.image.CreateView();

    stbi_image_free(data);
    copyBuffer.Destroy();
}

void StartupSystem(gecs::commands cmds,
                   gecs::resource<gecs::mut<nickel::Window>> window) {
    auto& adapter =
        cmds.emplace_resource<Adapter>(window->Raw(), Adapter::Option{API});
    auto& device = cmds.emplace_resource<Device>(adapter.RequestDevice());
    auto& ctx = cmds.emplace_resource<Context>();

    RenderPipeline::Descriptor desc;

    RenderPipeline::VertexState vertexState;
    RenderPipeline::BufferState bufferState;

    bufferState.attributes.push_back({VertexFormat::Float32x3, 0, 0});
    bufferState.attributes.push_back({VertexFormat::Float32x3, 12, 1});
    bufferState.attributes.push_back({VertexFormat::Float32x2, 24, 2});
    bufferState.arrayStride = 8 * 4;
    desc.vertex.buffers.emplace_back(bufferState);

    initShaders(adapter.RequestAdapterInfo().api, device, desc);

    initSampler(ctx, device);
    initImage(ctx, device);
    initVertexBuffer(ctx, device);
    initUniformBuffer(ctx, adapter, device, window.get());
    initBindGroupLayout(ctx, device);
    initPipelineLayout(ctx, device);
    initDepthTexture(ctx, device, window.get());

    RenderPipeline::FragmentTarget target;
    target.format = TextureFormat::Presentation;
    desc.layout = ctx.layout;
    desc.fragment.targets.emplace_back(target);

    RenderPipeline::DepthStencilState depthStencilState;
    depthStencilState.depthFormat = ctx.depth.Format();
    depthStencilState.depthWriteEnabled = true;
    depthStencilState.depthCompare = CompareOp::Greater;
    desc.depthStencil = depthStencilState;

    ctx.pipeline = device.CreateRenderPipeline(desc);
}

void UpdateSystem(gecs::resource<gecs::mut<nickel::rhi::Device>> device,
                  gecs::resource<gecs::mut<Context>> ctx) {
    device->BeginFrame();

    RenderPass::Descriptor desc;
    RenderPass::Descriptor::ColorAttachment colorAtt;
    colorAtt.loadOp = AttachmentLoadOp::Clear;
    colorAtt.storeOp = AttachmentStoreOp::Store;
    colorAtt.clearValue.fill(1);

    Texture::Descriptor textureDesc;
    textureDesc.format = TextureFormat::Presentation;
    auto [texture, view] = device->GetPresentationTexture();

    colorAtt.view = view;
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
    renderPass.SetPipeline(ctx->pipeline);
    renderPass.SetViewport(0, 0, 1024, 720);
    renderPass.SetVertexBuffer(0, ctx->vertexBuffer, 0,
                               ctx->vertexBuffer.Size());
    renderPass.SetBindGroup(ctx->debugBindGroup);
    renderPass.Draw(gVertices.size(), 1, 0, 0);
    renderPass.End();
    auto cmd = encoder.Finish();

    Queue queue = device->GetQueue();

    queue.Submit({cmd});
    device->EndFrame();

    encoder.Destroy();
}

void LogicUpdate(gecs::resource<gecs::mut<Context>> ctx) {
    static float x = 0, y = 0;

    void* data = ctx->uniformBuffer.GetMappedRange();
    mvp.model = nickel::cgmath::CreateTranslation({0, 0, -5}) *
                nickel::cgmath::CreateXYZRotation({x, y, 0});
    memcpy(data, mvp.model.data, sizeof(mvp.model));
    if (!ctx->uniformBuffer.IsMappingCoherence()) {
        ctx->uniformBuffer.Flush();
    }
    x += 0.001;
    y += 0.002;
}

void ShutdownSystem(gecs::commands cmds,
                    gecs::resource<gecs::mut<Context>> ctx) {
    ctx->depthView.Destroy();
    ctx->depth.Destroy();
    ctx->sampler.Destroy();
    ctx->imageView.Destroy();
    ctx->image.Destroy();

    ctx->debugBindGroup.Destroy();
    ctx->bindGroupLayout.Destroy();
    ctx->uniformBuffer.Destroy();
    ctx->vertexBuffer.Destroy();
    ctx->layout.Destroy();
    ctx->pipeline.Destroy();
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
        "04 texture cube", 1024, 720, API == APIPreference::Vulkan);

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
        .regist_update_system<UpdateSystem>()
        .regist_update_system<LogicUpdate>();
}