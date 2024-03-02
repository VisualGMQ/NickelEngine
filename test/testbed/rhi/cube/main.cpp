#include "graphics/rhi/rhi.hpp"
#include "nickel.hpp"
#include "vertex.hpp"

using namespace nickel::rhi;

APIPreference API = APIPreference::GL;

struct Context {
    PipelineLayout layout;
    RenderPipeline pipeline;
    Buffer vertexBuffer;
    Buffer indicesBuffer;
    Buffer uniformBuffer;
    BindGroupLayout bindGroupLayout;
    BindGroup bindGroup;
    Texture depth;
    TextureView depthView;
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
                "test/testbed/rhi/cube/vert.spv", std::ios::binary)
                .value();
        desc.vertex.module = device.CreateShaderModule(shaderDesc);

        shaderDesc.code =
            nickel::ReadWholeFile<std::vector<char>>(
                "test/testbed/rhi/cube/frag.spv", std::ios::binary)
                .value();
        desc.fragment.module = device.CreateShaderModule(shaderDesc);
    } else if (api == APIPreference::GL) {
        shaderDesc.code = nickel::ReadWholeFile<std::vector<char>>(
                              "test/testbed/rhi/cube/shader.glsl.vert")
                              .value();
        desc.vertex.module = device.CreateShaderModule(shaderDesc);

        shaderDesc.code = nickel::ReadWholeFile<std::vector<char>>(
                              "test/testbed/rhi/cube/shader.glsl.frag")
                              .value();
        desc.fragment.module = device.CreateShaderModule(shaderDesc);
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

void initIndicesBuffer(Context& ctx, Device& device) {
    Buffer::Descriptor bufferDesc;
    bufferDesc.size = sizeof(gIndices);
    bufferDesc.mappedAtCreation = true;
    bufferDesc.usage = BufferUsage::Index;
    ctx.indicesBuffer = device.CreateBuffer(bufferDesc);
    auto data = ctx.indicesBuffer.GetMappedRange();
    memcpy(data, gIndices.data(), sizeof(gIndices));
    ctx.indicesBuffer.Unmap();
}

void initUniformBuffer(Context& ctx, Device& device, nickel::Window& window) {
    mvp.proj = nickel::cgmath::CreatePersp(nickel::cgmath::Deg2Rad(45.0f),
                                           window.Size().w / window.Size().h,
                                           0.1, 100);
    mvp.view =
        nickel::cgmath::CreateTranslation(nickel::cgmath::Vec3{0, 0, -5});
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

void initPipelineLayout(Context& ctx, Device& device) {
    PipelineLayout::Descriptor layoutDesc;
    layoutDesc.layouts.emplace_back(ctx.bindGroupLayout);
    ctx.layout = device.CreatePipelineLayout(layoutDesc);
}

void initBindGroupLayout(Context& ctx, Device& device) {
    BindGroupLayout::Descriptor bindGroupLayoutDesc;

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
    ctx.bindGroupLayout = device.CreateBindGroupLayout(bindGroupLayoutDesc);

    BindGroup::Descriptor bindGroupDesc;
    bindGroupDesc.layout = ctx.bindGroupLayout;
    ctx.bindGroup = device.CreateBindGroup(bindGroupDesc);
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

void StartupSystem(gecs::commands cmds,
                   gecs::resource<gecs::mut<nickel::Window>> window) {
    auto& adapter = cmds.emplace_resource<Adapter>(
        window->Raw(), Adapter::Option{API});
    auto& device = cmds.emplace_resource<Device>(adapter.RequestDevice());
    auto& ctx = cmds.emplace_resource<Context>();

    RenderPipeline::Descriptor desc;

    RenderPipeline::VertexState vertexState;
    RenderPipeline::BufferState bufferState;

    bufferState.attributes.push_back({VertexFormat::Float32x3, 0, 0});
    bufferState.attributes.push_back({VertexFormat::Float32x3, 12, 1});
    bufferState.arrayStride = 6 * 4;
    desc.vertex.buffers.emplace_back(bufferState);

    desc.viewport.viewport.x = 0;
    desc.viewport.viewport.y = 0;
    desc.viewport.viewport.w = window->Size().w;
    desc.viewport.viewport.h = window->Size().h;
    desc.viewport.scissor.offset.x = 0;
    desc.viewport.scissor.offset.y = 0;
    desc.viewport.scissor.extent.width = window->Size().w;
    desc.viewport.scissor.extent.height = window->Size().h;

    initShaders(adapter.RequestAdapterInfo().api, device, desc);

    initVertexBuffer(ctx, device);
    initIndicesBuffer(ctx, device);
    initUniformBuffer(ctx, device, window.get());
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
    RenderPass::Descriptor desc;
    RenderPass::Descriptor::ColorAttachment colorAtt;
    colorAtt.loadOp = AttachmentLoadOp::Clear;
    colorAtt.storeOp = AttachmentStoreOp::Store;
    colorAtt.clearValue.fill(1);

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
    renderPass.SetIndexBuffer(ctx->indicesBuffer, IndexType::Uint32, 0, sizeof(gIndices));
    renderPass.SetBindGroup(ctx->bindGroup);
    renderPass.DrawIndexed(gIndices.size(), 1, 0, 0, 0);
    renderPass.End();
    auto cmd = encoder.Finish();

    Queue queue = device->GetQueue();

    queue.Submit({cmd});
    device->SwapContext();

    encoder.Destroy();
    view.Destroy();
    texture.Destroy();
}

void LogicUpdate(gecs::resource<gecs::mut<Context>> ctx) {
    static float x = 0, y = 0;

    void* data = ctx->uniformBuffer.GetMappedRange();
    mvp.model = nickel::cgmath::CreateXYZRotation({x, y, 0});
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
    ctx->bindGroup.Destroy();
    ctx->bindGroupLayout.Destroy();
    ctx->uniformBuffer.Destroy();
    ctx->vertexBuffer.Destroy();
    ctx->indicesBuffer.Destroy();
    ctx->layout.Destroy();
    ctx->pipeline.Destroy();
    cmds.remove_resource<Device>();
    cmds.remove_resource<Adapter>();
}

void BootstrapSystem(gecs::world& world,
                     typename gecs::world::registry_type& reg) {
    auto& args = reg.res<nickel::CmdLineArgs>()->Args();
    bool isVulkanBackend = args.size() == 1 ? true : (args[1] == "--api=gl" ? false : true);
    if (isVulkanBackend) {
        API = APIPreference::Vulkan;
    } else {
        API = APIPreference::GL;
    }
    nickel::Window& window = reg.commands().emplace_resource<nickel::Window>(
        "03 cube", 1024, 720, API == APIPreference::Vulkan);

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