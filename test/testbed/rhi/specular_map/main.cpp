#include "../common/camera.hpp"
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

struct MVP {
    nickel::cgmath::Mat44 model, view, proj;
} mvp;

struct Context final {
    PipelineLayout layout;
    RenderPipeline pipeline;
    Buffer MVPBuffer;
    Buffer vertexBuffer;
    Buffer eyePosBuffer;
    BindGroupLayout bindGroupLayout;
    BindGroup bindGroup;
    Texture depth;
    TextureView depthView;

    TextureBundle colorTexture;
    TextureBundle defaultNormalTexture;

    ~Context() {
        layout.Destroy();
        pipeline.Destroy();
        MVPBuffer.Destroy();
        eyePosBuffer.Destroy();
        depth.Destroy();
        depthView.Destroy();
        colorTexture.view.Destroy();
        colorTexture.texture.Destroy();
        colorTexture.sampler.Destroy();
        defaultNormalTexture.view.Destroy();
        defaultNormalTexture.texture.Destroy();
        defaultNormalTexture.sampler.Destroy();
        bindGroup.Destroy();
        bindGroupLayout.Destroy();
    }
};

void initShaders(APIPreference api, Device device,
                 RenderPipeline::Descriptor& desc) {
    ShaderModule::Descriptor shaderDesc;

    if (api == APIPreference::Vulkan) {
        shaderDesc.code =
            nickel::ReadWholeFile<std::vector<char>>(
                "test/testbed/rhi/specular_map/vert.spv", std::ios::binary)
                .value();
        desc.vertex.module = device.CreateShaderModule(shaderDesc);

        shaderDesc.code =
            nickel::ReadWholeFile<std::vector<char>>(
                "test/testbed/rhi/specular_map/frag.spv", std::ios::binary)
                .value();
        desc.fragment.module = device.CreateShaderModule(shaderDesc);
    } else if (api == APIPreference::GL) {
        shaderDesc.code = nickel::ReadWholeFile<std::vector<char>>(
                              "test/testbed/rhi/specular_map/shader.glsl.vert")
                              .value();
        desc.vertex.module = device.CreateShaderModule(shaderDesc);

        shaderDesc.code = nickel::ReadWholeFile<std::vector<char>>(
                              "test/testbed/rhi/specular_map/shader.glsl.frag")
                              .value();
        desc.fragment.module = device.CreateShaderModule(shaderDesc);
    }
}

void initUniformBuffer(Context& ctx, Adapter adapter, Device device,
                       nickel::Window& window, const Camera& camera) {
    Buffer::Descriptor bufferDesc;
    bufferDesc.usage = BufferUsage::Uniform;
    bufferDesc.mappedAtCreation = true;
    bufferDesc.size = 4 * 4 * 4 * 3;
    ctx.MVPBuffer = device.CreateBuffer(bufferDesc);

    char* data = (char*)ctx.MVPBuffer.GetMappedRange();
    auto model = nickel::cgmath::Mat44::Identity();
    uint32_t matSize = sizeof(nickel::cgmath::Mat44);
    memcpy(data, model.data, matSize);
    memcpy(data + matSize, camera.View().data, matSize);
    memcpy(data + matSize * 2, camera.Proj().data, matSize);
    if (!ctx.MVPBuffer.IsMappingCoherence()) {
        ctx.MVPBuffer.Flush();
    }
}

void initEyePosBuffer(Context& ctx, Device device, const Camera& camera) {
    Buffer::Descriptor bufferDesc;
    bufferDesc.usage = BufferUsage::Uniform;
    bufferDesc.mappedAtCreation = true;
    bufferDesc.size = sizeof(nickel::cgmath::Vec3);
    ctx.eyePosBuffer = device.CreateBuffer(bufferDesc);

    char* data = (char*)ctx.eyePosBuffer.GetMappedRange();
    memcpy(data, camera.Position().data, bufferDesc.size);
    if (!ctx.eyePosBuffer.IsMappingCoherence()) {
        ctx.eyePosBuffer.Flush();
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
    bufferBinding1.buffer = ctx.MVPBuffer;
    bufferBinding1.hasDynamicOffset = false;
    bufferBinding1.type = BufferType::Uniform;

    Entry entry;
    entry.arraySize = 1;
    entry.binding.binding = 0;
    entry.binding.entry = bufferBinding1;
    entry.visibility = ShaderStage::Vertex;
    bindGroupLayoutDesc.entries.emplace_back(entry);

    // color texture sampler
    SamplerBinding colorTextureBinding;
    colorTextureBinding.type = SamplerBinding::SamplerType::Filtering;
    colorTextureBinding.name = "mySampler";
    colorTextureBinding.sampler = ctx.colorTexture.sampler;
    colorTextureBinding.view = ctx.colorTexture.view;

    entry.arraySize = 1;
    entry.binding.binding = 1;
    entry.binding.entry = colorTextureBinding;
    entry.visibility = ShaderStage::Fragment;
    bindGroupLayoutDesc.entries.emplace_back(entry);

    // specular map sampler
    SamplerBinding specularTextureBinding;
    specularTextureBinding.type = SamplerBinding::SamplerType::Filtering;
    specularTextureBinding.name = "specularSampler";
    specularTextureBinding.sampler = ctx.defaultNormalTexture.sampler;
    specularTextureBinding.view = ctx.defaultNormalTexture.view;

    entry.arraySize = 1;
    entry.binding.binding = 2;
    entry.binding.entry = specularTextureBinding;
    entry.visibility = ShaderStage::Fragment;
    bindGroupLayoutDesc.entries.emplace_back(entry);

    // EyePos uniform buffer
    BufferBinding bufferBinding2;
    bufferBinding2.buffer = ctx.eyePosBuffer;
    bufferBinding2.hasDynamicOffset = false;
    bufferBinding2.type = BufferType::Uniform;

    entry.arraySize = 1;
    entry.binding.binding = 3;
    entry.binding.entry = bufferBinding2;
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

TextureBundle loadTexture(const std::filesystem::path& filename, Context& ctx,
                          Device& dev, TextureFormat fmt) {
    int w, h;
    void* data =
        stbi_load(filename.string().c_str(), &w, &h, nullptr, STBI_rgb_alpha);

    Texture::Descriptor desc;
    desc.format = fmt;
    desc.size.width = w;
    desc.size.height = h;
    desc.size.depthOrArrayLayers = 1;
    desc.usage = static_cast<uint32_t>(TextureUsage::TextureBinding) |
                 static_cast<uint32_t>(TextureUsage::CopyDst);
    auto texture = dev.CreateTexture(desc);

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
    dst.texture = texture;
    dst.aspect = TextureAspect::All;
    dst.miplevel = 0;
    encoder.CopyBufferToTexture(src, dst,
                                Extent3D{(uint32_t)w, (uint32_t)h, 1});
    auto buf = encoder.Finish();
    dev.GetQueue().Submit({buf});
    dev.WaitIdle();
    encoder.Destroy();

    auto view = texture.CreateView();

    stbi_image_free(data);
    copyBuffer.Destroy();

    return {texture, view, dev.CreateSampler({})};
}

void StartupSystem(gecs::commands cmds,
                   gecs::resource<gecs::mut<nickel::Window>> window) {
    auto& adapter =
        cmds.emplace_resource<Adapter>(window->Raw(), Adapter::Option{API});
    auto& device = cmds.emplace_resource<Device>(adapter.RequestDevice());
    auto& ctx = cmds.emplace_resource<Context>();
    auto& camera = cmds.emplace_resource<Camera>(
        adapter.RequestAdapterInfo().api, window->Size());
    camera.Move({0, 0, 50});
    SphericalCoordCameraProxy proxy(camera, {});
    proxy.Update2Camera();

    RenderPipeline::Descriptor desc;
    initShaders(adapter.RequestAdapterInfo().api, device, desc);
    initMeshData(device, ctx);
    ctx.colorTexture =
        loadTexture("test/testbed/rhi/specular_map/crate.png", ctx, device,
                    TextureFormat::RGBA8_UNORM_SRGB);
    ctx.defaultNormalTexture =
        loadTexture("test/testbed/rhi/specular_map/specular_map.png", ctx,
                    device, TextureFormat::RGBA8_UNORM);
    initUniformBuffer(ctx, adapter, device, window.get(), camera);
    initEyePosBuffer(ctx, device, camera);
    initBindGroupLayout(ctx, device);
    BindGroup::Descriptor bindGroupDesc;
    bindGroupDesc.layout = ctx.bindGroupLayout;
    ctx.bindGroup = device.CreateBindGroup(bindGroupDesc);
    initPipelineLayout(ctx, device);
    initDepthTexture(ctx, device, window.get());

    RenderPipeline::VertexState vertexState;
    RenderPipeline::BufferState bufferState;

    bufferState.attributes.push_back({VertexFormat::Float32x3, 0, 0});
    bufferState.attributes.push_back({VertexFormat::Float32x3, 3 * 4, 1});
    bufferState.attributes.push_back({VertexFormat::Float32x2, 6 * 4, 2});
    bufferState.arrayStride = 8 * 4;
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

    // desc.primitive.cullMode = CullMode::Back;

    ctx.pipeline = device.CreateRenderPipeline(desc);
}

void HandleEvent(gecs::resource<gecs::mut<Context>> ctx,
                 gecs::resource<nickel::Mouse> mouse,
                 gecs::resource<nickel::Keyboard> keyboard,
                 gecs::resource<gecs::mut<Camera>> camera) {
    constexpr float offset = 0.01;
    constexpr float rStep = 0.5;

    SphericalCoordCameraProxy proxy(camera.get(), {});
    float x = proxy.GetPhi(), y = proxy.GetTheta();
    if (mouse->LeftBtn().IsPress()) {
        y -= mouse->Offset().y * offset;
        x += mouse->Offset().x * offset;
    }

    if (auto y = mouse->WheelOffset().y; y != 0) {
        proxy.SetRadius(proxy.GetRadius() - rStep * y);
    }

    proxy.SetTheta(y);
    proxy.SetPhi(x);

    proxy.Update2Camera();
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
    renderPass.SetVertexBuffer(0, ctx->vertexBuffer, 0,
                               ctx->vertexBuffer.Size());
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

void LogicUpdate(gecs::resource<gecs::mut<Context>> ctx,
                 gecs::resource<Camera> camera) {
    char* data = (char*)ctx->MVPBuffer.GetMappedRange();
    uint32_t matSize = sizeof(nickel::cgmath::Mat44);
    memcpy(data + matSize, camera->View().data, matSize);
    memcpy(data + matSize * 2, camera->Proj().data, matSize);
    if (!ctx->MVPBuffer.IsMappingCoherence()) {
        ctx->MVPBuffer.Flush(matSize, matSize * 2);
    }

    data = (char*)ctx->eyePosBuffer.GetMappedRange();
    memcpy(data, camera->Position().data, sizeof(nickel::cgmath::Vec3));
    if (!ctx->eyePosBuffer.IsMappingCoherence()) {
        ctx->eyePosBuffer.Flush();
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
        "specular", 1024, 720, API == APIPreference::Vulkan);

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