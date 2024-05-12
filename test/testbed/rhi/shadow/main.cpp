#include "graphics/rhi/rhi.hpp"
#include "graphics/rhi/util.hpp"
#include "nickel.hpp"
#include "vertex.hpp"

using namespace nickel::rhi;

APIPreference API = APIPreference::GL;

struct Context final {
    PipelineLayout renderPipelineLayout;
    PipelineLayout shadowPipelineLayout;
    RenderPipeline pipeline;
    RenderPipeline shadowPipeline;
    Buffer uniformBuffer;
    Buffer cubeVertexBuffer;
    Buffer planeVertexBuffer;
    BindGroupLayout renderBindGroupLayout;
    BindGroupLayout shadowBindGroupLayout;
    BindGroup bindGroup;
    BindGroup shadowBindGroup;
    Texture depth;
    TextureView depthView;
    Texture shadowDepth;
    TextureView shadowDepthView;
    Sampler shadowDepthSampler;

    ~Context() {
        renderPipelineLayout.Destroy();
        shadowPipelineLayout.Destroy();
        pipeline.Destroy();
        shadowPipeline.Destroy();
        uniformBuffer.Destroy();
        cubeVertexBuffer.Destroy();
        planeVertexBuffer.Destroy();
        depth.Destroy();
        depthView.Destroy();
        shadowDepth.Destroy();
        shadowDepthView.Destroy();
        shadowDepthSampler.Destroy();
        bindGroup.Destroy();
        shadowBindGroup.Destroy();
        renderBindGroupLayout.Destroy();
        shadowBindGroupLayout.Destroy();
    }
};

struct MVP {
    nickel::cgmath::Mat44 model, view, proj, lightMatrix;
} mvp;

const auto LightDir =
    nickel::cgmath::Normalize(nickel::cgmath::Vec3{-1, -1, -0.4});

void initShaders(APIPreference api, Device device,
                 RenderPipeline::Descriptor& desc) {
    ShaderModule::Descriptor shaderDesc;

    if (api == APIPreference::Vulkan) {
        shaderDesc.code =
            nickel::ReadWholeFile<std::vector<char>>(
                "test/testbed/rhi/shadow/vert.spv", std::ios::binary)
                .value();
        desc.vertex.module = device.CreateShaderModule(shaderDesc);

        shaderDesc.code =
            nickel::ReadWholeFile<std::vector<char>>(
                "test/testbed/rhi/shadow/frag.spv", std::ios::binary)
                .value();
        desc.fragment.module = device.CreateShaderModule(shaderDesc);
    } else if (api == APIPreference::GL) {
        shaderDesc.code = nickel::ReadWholeFile<std::vector<char>>(
                              "test/testbed/rhi/shadow/shader.glsl.vert")
                              .value();
        desc.vertex.module = device.CreateShaderModule(shaderDesc);

        shaderDesc.code = nickel::ReadWholeFile<std::vector<char>>(
                              "test/testbed/rhi/shadow/shader.glsl.frag")
                              .value();
        desc.fragment.module = device.CreateShaderModule(shaderDesc);
    }
}

void initShadowShaders(APIPreference api, Device device,
                 RenderPipeline::Descriptor& desc) {
    ShaderModule::Descriptor shaderDesc;

    if (api == APIPreference::Vulkan) {
        shaderDesc.code =
            nickel::ReadWholeFile<std::vector<char>>(
                "test/testbed/rhi/shadow/vert.shadow.spv", std::ios::binary)
                .value();
        desc.vertex.module = device.CreateShaderModule(shaderDesc);

        shaderDesc.code =
            nickel::ReadWholeFile<std::vector<char>>(
                "test/testbed/rhi/shadow/frag.shadow.spv", std::ios::binary)
                .value();
        desc.fragment.module = device.CreateShaderModule(shaderDesc);
    } else if (api == APIPreference::GL) {
        shaderDesc.code = nickel::ReadWholeFile<std::vector<char>>(
                              "test/testbed/rhi/shadow/shader.shadow.vert")
                              .value();
        desc.vertex.module = device.CreateShaderModule(shaderDesc);

        shaderDesc.code = nickel::ReadWholeFile<std::vector<char>>(
                              "test/testbed/rhi/shadow/shader.shadow.frag")
                              .value();
        desc.fragment.module = device.CreateShaderModule(shaderDesc);
    }
}

void initUniformBuffer(Context& ctx, Adapter adapter, Device device,
                       nickel::Window& window) {
    mvp.proj = nickel::cgmath::CreatePersp(
        nickel::cgmath::Deg2Rad(45.0f), window.Size().w / window.Size().h, 0.1,
        1000, adapter.RequestAdapterInfo().api == APIPreference::GL);
    mvp.view = nickel::cgmath::CreateTranslation({0, 0, -5});
    mvp.model = nickel::cgmath::Mat44::Identity();
    mvp.lightMatrix =
        nickel::cgmath::CreateOrtho(-10, 10, -10, 10, 5.5, -20.5,
                                    adapter.RequestAdapterInfo().api ==
                                        nickel::rhi::APIPreference::GL) *
        nickel::cgmath::LookAt({}, -LightDir, {0, 1, 0});
    uint32_t offset = offsetof(MVP, lightMatrix);

    Buffer::Descriptor bufferDesc;
    bufferDesc.usage = BufferUsage::Uniform;
    bufferDesc.mappedAtCreation = true;
    bufferDesc.size = sizeof(mvp);
    ctx.uniformBuffer = device.CreateBuffer(bufferDesc);
    void* data = ctx.uniformBuffer.GetMappedRange();
    memcpy(data, &mvp, sizeof(mvp));
    if (!ctx.uniformBuffer.IsMappingCoherence()) {
        ctx.uniformBuffer.Flush();
    }
}

PipelineLayout initPipelineLayout(BindGroupLayout layout, Device& device) {
    PipelineLayout::Descriptor layoutDesc;
    layoutDesc.layouts.emplace_back(layout);
    return device.CreatePipelineLayout(layoutDesc);
}

BindGroupLayout initShadowPipelineBindGroupLayout(Context& ctx, Device& device) {
    BindGroupLayout::Descriptor bindGroupLayoutDesc;

    // MVP uniform buffer
    {
        BufferBinding binding;
        binding.buffer = ctx.uniformBuffer;
        binding.hasDynamicOffset = false;
        binding.type = BufferType::Uniform;

        Entry entry;
        entry.arraySize = 1;
        entry.binding.binding = 0;
        entry.binding.entry = binding;
        entry.visibility = ShaderStage::Vertex;
        bindGroupLayoutDesc.entries.emplace_back(entry);
    }

    return device.CreateBindGroupLayout(bindGroupLayoutDesc);
}

BindGroupLayout initPipelineBindGroupLayout(Context& ctx, Device& device) {
    BindGroupLayout::Descriptor bindGroupLayoutDesc;

    // MVP uniform buffer
    {
        BufferBinding binding;
        binding.buffer = ctx.uniformBuffer;
        binding.hasDynamicOffset = false;
        binding.type = BufferType::Uniform;

        Entry entry;
        entry.arraySize = 1;
        entry.binding.binding = 0;
        entry.binding.entry = binding;
        entry.visibility = ShaderStage::Vertex;
        bindGroupLayoutDesc.entries.emplace_back(entry);
    }

    // sampler uniform buffer
    {
        SamplerBinding binding;
        binding.sampler = ctx.shadowDepthSampler;
        binding.view = ctx.shadowDepthView;
        binding.name = "mySampler";

        Entry entry;
        entry.arraySize = 1;
        entry.binding.binding = 1;
        entry.binding.entry = binding;
        entry.visibility = ShaderStage::Fragment;
        bindGroupLayoutDesc.entries.emplace_back(entry);
    }

    return device.CreateBindGroupLayout(bindGroupLayoutDesc);
}

std::tuple<Texture, TextureView> initDepthTexture(Device& dev, nickel::Window& window) {
    Texture::Descriptor desc;
    desc.format = TextureFormat::DEPTH32_FLOAT;
    desc.size.width = window.Size().w;
    desc.size.height = window.Size().h;
    desc.size.depthOrArrayLayers = 1;
    desc.usage = Flags<TextureUsage>(TextureUsage::RenderAttachment) | TextureUsage::TextureBinding;
    auto texture = dev.CreateTexture(desc);
    return {texture, texture.CreateView()};
}

void initMeshData(Device device, Context& ctx) {
    Buffer::Descriptor desc;
    desc.mappedAtCreation = true;
    desc.usage = BufferUsage::Vertex;
    desc.size = sizeof(gCubeVertices);
    ctx.cubeVertexBuffer= device.CreateBuffer(desc);
    void* data = ctx.cubeVertexBuffer.GetMappedRange();
    memcpy(data, gCubeVertices.data(), sizeof(gCubeVertices));
    ctx.cubeVertexBuffer.Unmap();

    desc.size = sizeof(gPlaneVertices);
    ctx.planeVertexBuffer = device.CreateBuffer(desc);
    data = ctx.planeVertexBuffer.GetMappedRange();
    memcpy(data, gPlaneVertices.data(), sizeof(gPlaneVertices));
    ctx.planeVertexBuffer.Unmap();
}

RenderPipeline createRenderPipeline(Device device, APIPreference api,
                                    nickel::Window& window,
                                    PipelineLayout layout,
                                    Texture depthTexture) {
    RenderPipeline::Descriptor desc;

    initShaders(api, device, desc);

    RenderPipeline::VertexState vertexState;
    RenderPipeline::BufferState bufferState;

    bufferState.attributes.push_back({VertexFormat::Float32x3, 0, 0});
    bufferState.attributes.push_back(
        {VertexFormat::Float32x3, 3 * sizeof(float), 1});
    bufferState.attributes.push_back(
        {VertexFormat::Float32x2, 6 * sizeof(float), 2});
    bufferState.arrayStride = 8 * 4;
    desc.vertex.buffers.emplace_back(bufferState);

    RenderPipeline::FragmentTarget target;
    target.format = TextureFormat::Presentation;
    desc.layout = layout;
    desc.fragment.targets.emplace_back(target);

    RenderPipeline::DepthStencilState depthStencilState;
    depthStencilState.depthFormat = depthTexture.Format();
    depthStencilState.depthWriteEnabled = true;
    depthStencilState.depthCompare = CompareOp::Greater;
    desc.depthStencil = depthStencilState;
    desc.primitive.cullMode = CullMode::Back;

    return device.CreateRenderPipeline(desc);
}

RenderPipeline createShadowRenderPipeline(Device device, APIPreference api,
                                    nickel::Window& window,
                                    PipelineLayout layout,
                                    Texture depthTexture) {
    RenderPipeline::Descriptor desc;

    initShadowShaders(api, device, desc);

    RenderPipeline::VertexState vertexState;
    RenderPipeline::BufferState bufferState;

    bufferState.attributes.push_back({VertexFormat::Float32x3, 0, 0});
    bufferState.attributes.push_back(
        {VertexFormat::Float32x3, 3 * sizeof(float), 1});
    bufferState.attributes.push_back( {VertexFormat::Float32x2, 6 * sizeof(float), 2});
    bufferState.arrayStride = 8 * 4;
    desc.vertex.buffers.emplace_back(bufferState);

    desc.layout = layout;

    RenderPipeline::DepthStencilState depthStencilState;
    depthStencilState.depthFormat = depthTexture.Format();
    depthStencilState.depthWriteEnabled = true;
    depthStencilState.depthCompare = CompareOp::Greater;
    desc.depthStencil = depthStencilState;
    desc.primitive.cullMode = CullMode::Front;

    return device.CreateRenderPipeline(desc);
}

void StartupSystem(gecs::commands cmds,
                   gecs::resource<gecs::mut<nickel::Window>> window) {
    auto& adapter =
        cmds.emplace_resource<Adapter>(window->Raw(), Adapter::Option{API});
    auto& device = cmds.emplace_resource<Device>(adapter.RequestDevice());
    auto& ctx = cmds.emplace_resource<Context>();

    initMeshData(device, ctx);
    initUniformBuffer(ctx, adapter, device, window.get());
    {
        auto [texture, view] = initDepthTexture(device, window.get());
        ctx.depth = texture;
        ctx.depthView = view;
    }
    {
        auto [texture, view] = initDepthTexture(device, window.get());
        ctx.shadowDepth = texture;
        ctx.shadowDepthView = view;
        Sampler::Descriptor desc;
        ctx.shadowDepthSampler = device.CreateSampler({});
    }
    ctx.renderBindGroupLayout = initPipelineBindGroupLayout(ctx, device);
    ctx.shadowBindGroupLayout = initShadowPipelineBindGroupLayout(ctx, device);
    BindGroup::Descriptor bindGroupDesc;
    bindGroupDesc.layout = ctx.renderBindGroupLayout;
    ctx.bindGroup = device.CreateBindGroup(bindGroupDesc);
    bindGroupDesc.layout = ctx.shadowBindGroupLayout;
    ctx.shadowBindGroup = device.CreateBindGroup(bindGroupDesc);
    ctx.renderPipelineLayout = initPipelineLayout(ctx.renderBindGroupLayout, device);
    ctx.shadowPipelineLayout = initPipelineLayout(ctx.shadowBindGroupLayout, device);

    ctx.pipeline =
        createRenderPipeline(device, adapter.RequestAdapterInfo().api,
                             window.get(), ctx.renderPipelineLayout, ctx.shadowDepth);
    ctx.shadowPipeline =
        createShadowRenderPipeline(device, adapter.RequestAdapterInfo().api,
                             window.get(), ctx.shadowPipelineLayout, ctx.depth);
}

void HandleEvent(gecs::resource<gecs::mut<Context>> ctx,
                 gecs::resource<nickel::Mouse> mouse,
                 gecs::resource<nickel::Keyboard> keyboard) {
    constexpr float offset = 0.01;
    constexpr float scaleStep = 0.01;
    static float scale = 1;
    static float x = 0, y = 0;
    if (mouse->LeftBtn().IsPress()) {
        y += mouse->Offset().x * offset;
        x += mouse->Offset().y * offset;
    }

    if (auto y = mouse->WheelOffset().y; y != 0) {
        scale += scaleStep * nickel::cgmath::Sign(y) *
                 (keyboard->Key(nickel::Key::Lctrl).IsPress() ? 100 : 1);
    }

    if (scale < 0) {
        scale = 0.001;
    }

    mvp.view = nickel::cgmath::CreateTranslation({0, 0, -5}) *
               nickel::cgmath::CreateXYZRotation({x, y, 0}) *
               nickel::cgmath::CreateScale({scale, scale, scale});
}

void UpdateSystem(gecs::resource<gecs::mut<nickel::rhi::Device>> device,
                  gecs::resource<gecs::mut<Context>> ctx) {
    device->BeginFrame();

    RenderPass::Descriptor desc;

    auto [texture, view] = device->GetPresentationTexture();

    desc.depthStencilAttachment =
        RenderPass::Descriptor::DepthStencilAttachment{};
    desc.depthStencilAttachment->view = ctx->shadowDepthView;
    desc.depthStencilAttachment->depthLoadOp = AttachmentLoadOp::Clear;
    desc.depthStencilAttachment->depthStoreOp = AttachmentStoreOp::Store;
    desc.depthStencilAttachment->depthReadOnly = false;
    desc.depthStencilAttachment->depthClearValue = 0.0;

    // render shadow map
    auto encoder1 = device->CreateCommandEncoder();
    {
        auto renderPass = encoder1.BeginRenderPass(desc);
        renderPass.SetPipeline(ctx->shadowPipeline);
        renderPass.SetViewport(0, 0, 1024, 720);
        renderPass.SetBindGroup(ctx->shadowBindGroup);
        renderPass.SetVertexBuffer(0, ctx->cubeVertexBuffer, 0,
                                ctx->cubeVertexBuffer.Size());
        renderPass.Draw(gCubeVertices.size(), 1, 0, 0);
        renderPass.SetVertexBuffer(0, ctx->planeVertexBuffer, 0,
                                ctx->planeVertexBuffer.Size());
        renderPass.Draw(gPlaneVertices.size(), 1, 0, 0);
        renderPass.End();
    }
    auto cmd1 = encoder1.Finish();
    Queue queue = device->GetQueue();
    queue.Submit({cmd1});

    RenderPass::Descriptor::ColorAttachment colorAtt;
    colorAtt.loadOp = AttachmentLoadOp::Clear;
    colorAtt.storeOp = AttachmentStoreOp::Store;
    colorAtt.clearValue = {0.0, 0.0, 0.0, 1};
    colorAtt.view = view;

    // render scene
    auto encoder2 = device->CreateCommandEncoder();
    {
        desc.colorAttachments.emplace_back(colorAtt);

        desc.depthStencilAttachment =
            RenderPass::Descriptor::DepthStencilAttachment{};
        desc.depthStencilAttachment->view = ctx->depthView;
        desc.depthStencilAttachment->depthLoadOp = AttachmentLoadOp::Clear;
        desc.depthStencilAttachment->depthStoreOp = AttachmentStoreOp::Store;

        auto renderPass = encoder2.BeginRenderPass(desc);
        renderPass.SetPipeline(ctx->pipeline);
        renderPass.SetViewport(0, 0, 1024, 720);
        renderPass.SetBindGroup(ctx->bindGroup);
        renderPass.SetVertexBuffer(0, ctx->cubeVertexBuffer, 0,
                                ctx->cubeVertexBuffer.Size());
        renderPass.Draw(gCubeVertices.size(), 1, 0, 0);
        renderPass.SetVertexBuffer(0, ctx->planeVertexBuffer, 0,
                                ctx->planeVertexBuffer.Size());
        renderPass.Draw(gPlaneVertices.size(), 1, 0, 0);
        renderPass.End();
    }

    auto cmd2 = encoder2.Finish();

    queue.Submit({cmd2});
    device->EndFrame();

    encoder1.Destroy();
    encoder2.Destroy();
}

void LogicUpdate(gecs::resource<gecs::mut<Context>> ctx) {
    void* data = ctx->uniformBuffer.GetMappedRange();
    auto offset = sizeof(nickel::cgmath::Mat44);
    memcpy((char*)data + offset, mvp.view.data, sizeof(mvp.view));
    if (!ctx->uniformBuffer.IsMappingCoherence()) {
        ctx->uniformBuffer.Flush(offset, sizeof(nickel::cgmath::Mat44));
    }
}

void ShutdownSystem(gecs::commands cmds,
                    gecs::resource<gecs::mut<Context>> ctx) {
    cmds.remove_resource<Context>();
}

void BootstrapSystem(gecs::world& world,
                     typename gecs::world::registry_type& reg) {
    auto& args = reg.res<nickel::CmdLineArgs>()->Args();
#ifdef NICKEL_HAS_VULKAN
    bool isVulkanBackend =
        args.size() == 1 ? true : (args[1] == "--api=gl" ? false : true);
#else
    bool isVulkanBackend = false;
#endif
    if (isVulkanBackend) {
        API = APIPreference::Vulkan;
    } else {
        API = APIPreference::GL;
    }
    nickel::Window& window = reg.commands().emplace_resource<nickel::Window>(
        "shadow", 1024, 720, API == APIPreference::Vulkan);

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
        // other input handle event must put here(after mouse/keyboard
        // update)
        .regist_update_system<nickel::Mouse::Update>()
        .regist_update_system<nickel::Keyboard::Update>()
        .regist_update_system<nickel::HandleInputEvents>()
        .regist_update_system<UpdateSystem>()
        .regist_update_system<HandleEvent>()
        .regist_update_system<LogicUpdate>();
}