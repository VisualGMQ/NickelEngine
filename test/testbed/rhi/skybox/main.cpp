#include "graphics/rhi/rhi.hpp"
#include "graphics/rhi/util.hpp"
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

struct Context final {
    PipelineLayout layout;
    RenderPipeline pipeline;
    Buffer uniformBuffer;
    Buffer vertexBuffer;
    BindGroupLayout bindGroupLayout;
    BindGroup bindGroup;
    Texture depth;
    TextureView depthView;

    TextureBundle skyboxTexture;

    ~Context() {
        layout.Destroy();
        pipeline.Destroy();
        uniformBuffer.Destroy();
        depth.Destroy();
        depthView.Destroy();
        skyboxTexture.view.Destroy();
        skyboxTexture.texture.Destroy();
        skyboxTexture.sampler.Destroy();
        bindGroup.Destroy();
        bindGroupLayout.Destroy();
    }
};

struct MVP {
    nickel::cgmath::Mat44 view, proj;
} mvp;

void initShaders(APIPreference api, Device device,
                 RenderPipeline::Descriptor& desc) {
    ShaderModule::Descriptor shaderDesc;

    if (api == APIPreference::Vulkan) {
        shaderDesc.code =
            nickel::ReadWholeFile<std::vector<char>>(
                "test/testbed/rhi/skybox/vert.spv", std::ios::binary)
                .value();
        desc.vertex.module = device.CreateShaderModule(shaderDesc);

        shaderDesc.code =
            nickel::ReadWholeFile<std::vector<char>>(
                "test/testbed/rhi/skybox/frag.spv", std::ios::binary)
                .value();
        desc.fragment.module = device.CreateShaderModule(shaderDesc);
    } else if (api == APIPreference::GL) {
        shaderDesc.code = nickel::ReadWholeFile<std::vector<char>>(
                              "test/testbed/rhi/skybox/shader.glsl.vert")
                              .value();
        desc.vertex.module = device.CreateShaderModule(shaderDesc);

        shaderDesc.code = nickel::ReadWholeFile<std::vector<char>>(
                              "test/testbed/rhi/skybox/shader.glsl.frag")
                              .value();
        desc.fragment.module = device.CreateShaderModule(shaderDesc);
    }
}

void initUniformBuffer(Context& ctx, Adapter adapter, Device device,
                       nickel::Window& window) {
    mvp.proj = nickel::cgmath::CreatePersp(
        nickel::cgmath::Deg2Rad(45.0f), window.Size().w / window.Size().h, 0.1,
        10000, adapter.RequestAdapterInfo().api == APIPreference::GL);
    mvp.view =
        nickel::cgmath::CreateScale(nickel::cgmath::Vec3{10, 10, 10});

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

void initPipelineLayout(Context& ctx, Device& device) {
    PipelineLayout::Descriptor layoutDesc;
    layoutDesc.layouts.emplace_back(ctx.bindGroupLayout);
    ctx.layout = device.CreatePipelineLayout(layoutDesc);
}

void initBindGroupLayout(Context& ctx, Device& device) {
    BindGroupLayout::Descriptor bindGroupLayoutDesc;

    // MVP uniform buffer
    BufferBinding bufferBinding1;
    bufferBinding1.buffer = ctx.uniformBuffer;
    bufferBinding1.hasDynamicOffset = false;
    bufferBinding1.type = BufferType::Uniform;

    Entry entry;
    entry.arraySize = 1;
    entry.binding.binding = 0;
    entry.binding.entry = bufferBinding1;
    entry.visibility = ShaderStage::Vertex;
    bindGroupLayoutDesc.entries.emplace_back(entry);

    // skybox sampler
    SamplerBinding skyboxTextureBinding;
    skyboxTextureBinding.type = SamplerBinding::SamplerType::Filtering;
    skyboxTextureBinding.name = "mySampler";
    skyboxTextureBinding.sampler = ctx.skyboxTexture.sampler;
    skyboxTextureBinding.view = ctx.skyboxTexture.view;

    entry.arraySize = 1;
    entry.binding.binding = 1;
    entry.binding.entry = skyboxTextureBinding;
    entry.visibility = ShaderStage::Fragment;
    bindGroupLayoutDesc.entries.emplace_back(entry);

    ctx.bindGroupLayout = device.CreateBindGroupLayout(bindGroupLayoutDesc);
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

void initMeshData(Device device, Context& ctx) {
    Buffer::Descriptor desc;
    desc.mappedAtCreation = true;
    desc.usage = BufferUsage::Vertex;
    desc.size = sizeof(gVertices);
    ctx.vertexBuffer = device.CreateBuffer(desc);
    void* data = ctx.vertexBuffer.GetMappedRange();
    memcpy(data, gVertices.data(), sizeof(gVertices));
    ctx.vertexBuffer.Unmap();
}


TextureBundle loadTextures(
    const std::array<std::filesystem::path, 6>& filenames, Context& ctx,
    Device& dev) {
    Texture::Descriptor desc;
    constexpr uint32_t width = 2048, height = 2048;
    desc.format = TextureFormat::RGBA8_UNORM;
    desc.dimension = TextureType::Dim2;
    desc.size.width = width;
    desc.size.height = height;
    desc.size.depthOrArrayLayers = 6;
    desc.usage = static_cast<uint32_t>(TextureUsage::TextureBinding) |
                 static_cast<uint32_t>(TextureUsage::CopyDst);
    desc.flags = TextureFlagBits::CubeCompatible;
    auto texture = dev.CreateTexture(desc);

    Buffer::Descriptor bufferDesc;
    bufferDesc.mappedAtCreation = true;
    bufferDesc.usage = BufferUsage::CopySrc;
    bufferDesc.size = 4 * width * height;
    Buffer copyBuffer = dev.CreateBuffer(bufferDesc);

    void* bufData = copyBuffer.GetMappedRange();
    for (int i = 0; i < filenames.size(); i++) {
        int x, y;
        void* data = stbi_load(filenames[i].string().c_str(), &x, &y,
                               nullptr, STBI_rgb_alpha);

        memcpy(bufData, data, bufferDesc.size);
        if (!copyBuffer.IsMappingCoherence()) {
            copyBuffer.Flush();
        }

        auto encoder = dev.CreateCommandEncoder();
        CommandEncoder::BufTexCopySrc src;
        src.buffer = copyBuffer;
        src.offset = 0;
        src.rowLength = width;
        src.rowsPerImage = height;
        CommandEncoder::BufTexCopyDst dst;
        dst.texture = texture;
        dst.aspect = TextureAspect::All;
        dst.miplevel = 0;
        dst.origin.x = 0;
        dst.origin.y = 0;
        dst.origin.z = i;
        encoder.CopyBufferToTexture(src, dst,
                                    Extent3D{width, height, 1});
        auto buf = encoder.Finish();
        dev.GetQueue().Submit({buf});
        dev.WaitIdle();
        encoder.Destroy();
        stbi_image_free(data);
    }
    copyBuffer.Unmap();

    TextureView::Descriptor viewDesc;
    viewDesc.dimension = TextureViewType::Cube;
    auto view = texture.CreateView(viewDesc);

    copyBuffer.Destroy();

    return {texture, view, dev.CreateSampler({})};
}

void StartupSystem(gecs::commands cmds,
                   gecs::resource<gecs::mut<nickel::Window>> window) {
    auto& adapter =
        cmds.emplace_resource<Adapter>(window->Raw(), Adapter::Option{API});
    auto& device = cmds.emplace_resource<Device>(adapter.RequestDevice());
    auto& ctx = cmds.emplace_resource<Context>();

    RenderPipeline::Descriptor desc;
    initShaders(adapter.RequestAdapterInfo().api, device, desc);
    ctx.skyboxTexture = loadTextures({
        "test/testbed/rhi/skybox/skybox/right.jpg",
        "test/testbed/rhi/skybox/skybox/left.jpg",
        "test/testbed/rhi/skybox/skybox/top.jpg",
        "test/testbed/rhi/skybox/skybox/bottom.jpg",
        "test/testbed/rhi/skybox/skybox/front.jpg",
        "test/testbed/rhi/skybox/skybox/back.jpg",
    }, ctx, device);
    initMeshData(device, ctx);
    initUniformBuffer(ctx, adapter, device, window.get());
    initBindGroupLayout(ctx, device);
    BindGroup::Descriptor bindGroupDesc;
    bindGroupDesc.layout = ctx.bindGroupLayout;
    ctx.bindGroup = device.CreateBindGroup(bindGroupDesc);
    initPipelineLayout(ctx, device);
    initDepthTexture(ctx, device, window.get());

    RenderPipeline::VertexState vertexState;
    RenderPipeline::BufferState bufferState;

    bufferState.attributes.push_back({VertexFormat::Float32x3, 0, 0});
    bufferState.arrayStride = 3 * 4;
    desc.vertex.buffers.emplace_back(bufferState);

    desc.viewport.viewport.x = 0;
    desc.viewport.viewport.y = 0;
    desc.viewport.viewport.w = window->Size().w;
    desc.viewport.viewport.h = window->Size().h;
    desc.viewport.scissor.offset.x = 0;
    desc.viewport.scissor.offset.y = 0;
    desc.viewport.scissor.extent.width = window->Size().w;
    desc.viewport.scissor.extent.height = window->Size().h;

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

    mvp.view = nickel::cgmath::CreateXYZRotation({x, y, 0});
}

void UpdateSystem(gecs::resource<gecs::mut<nickel::rhi::Device>> device,
                  gecs::resource<gecs::mut<Context>> ctx) {
    device->BeginFrame();

    RenderPass::Descriptor desc;
    RenderPass::Descriptor::ColorAttachment colorAtt;
    colorAtt.loadOp = AttachmentLoadOp::Clear;
    colorAtt.storeOp = AttachmentStoreOp::Store;
    colorAtt.clearValue = {0.3, 0.3, 0.3, 1};

    Texture::Descriptor textureDesc;
    textureDesc.format = TextureFormat::Presentation;
    auto texture = device->CreateTexture(textureDesc);

    auto view = texture.CreateView();
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
    renderPass.SetVertexBuffer(0, ctx->vertexBuffer, 0, ctx->vertexBuffer.Size());
    renderPass.SetBindGroup(ctx->bindGroup);
    renderPass.Draw(gVertices.size(), 1, 0, 0);
    renderPass.End();
    auto cmd = encoder.Finish();

    Queue queue = device->GetQueue();

    queue.Submit({cmd});
    device->EndFrame();

    encoder.Destroy();
    view.Destroy();
    texture.Destroy();
}

void LogicUpdate(gecs::resource<gecs::mut<Context>> ctx) {
    void* data = ctx->uniformBuffer.GetMappedRange();
    memcpy(data, mvp.view.data, sizeof(mvp.view));
    if (!ctx->uniformBuffer.IsMappingCoherence()) {
        ctx->uniformBuffer.Flush();
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
        "skybox", 1024, 720, API == APIPreference::Vulkan);

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