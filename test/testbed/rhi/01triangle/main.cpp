#include "graphics/rhi/rhi.hpp"
#include "nickel.hpp"

using namespace nickel::rhi;

struct Context {
    PipelineLayout layout;
    RenderPipeline pipeline;
};

void initShaders(APIPreference api, Device device,
                 RenderPipeline::Descriptor& desc) {
    ShaderModule::Descriptor shaderDesc;
    if (api == APIPreference::Vulkan) {
        shaderDesc.code =
            nickel::ReadWholeFile<std::vector<char>>(
                "test/testbed/rhi/01triangle/vert.spv", std::ios::binary)
                .value();
        desc.vertex.module = device.CreateShaderModule(shaderDesc);

        shaderDesc.code =
            nickel::ReadWholeFile<std::vector<char>>(
                "test/testbed/rhi/01triangle/frag.spv", std::ios::binary)
                .value();
        desc.fragment.module = device.CreateShaderModule(shaderDesc);
    } else if (api == APIPreference::GL) {
        shaderDesc.code =
            nickel::ReadWholeFile<std::vector<char>>(
                "test/testbed/rhi/01triangle/shader.glsl.vert")
                .value();
        desc.vertex.module = device.CreateShaderModule(shaderDesc);

        shaderDesc.code =
            nickel::ReadWholeFile<std::vector<char>>(
                "test/testbed/rhi/01triangle/shader.glsl.frag")
                .value();
        desc.fragment.module = device.CreateShaderModule(shaderDesc);
    }
}

void StartupSystem(gecs::commands cmds,
                   gecs::resource<gecs::mut<nickel::Window>> window) {
    auto& adapter = cmds.emplace_resource<Adapter>(
        window->Raw(), Adapter::Option{APIPreference::Vulkan});
    auto& device = cmds.emplace_resource<Device>(adapter.RequestDevice());
    auto& ctx = cmds.emplace_resource<Context>();

    RenderPipeline::Descriptor desc;
    desc.viewport.viewport.x = 0;
    desc.viewport.viewport.y = 0;
    desc.viewport.viewport.w = window->Size().w;
    desc.viewport.viewport.h = window->Size().h;
    desc.viewport.scissor.offset.x = 0;
    desc.viewport.scissor.offset.y = 0;
    desc.viewport.scissor.extent.width = window->Size().w;
    desc.viewport.scissor.extent.height = window->Size().h;

    initShaders(adapter.RequestAdapterInfo().api, device, desc);

    ctx.layout = device.CreatePipelineLayout({});
    desc.layout = ctx.layout;
    RenderPipeline::FragmentTarget target;
    target.format = TextureFormat::Presentation;
    desc.fragment.targets.emplace_back(target);

    ctx.pipeline = device.CreateRenderPipeline(desc);
}

void UpdateSystem(gecs::resource<gecs::mut<nickel::rhi::Device>> device,
                  gecs::resource<gecs::mut<Context>> ctx) {
    auto encoder = device->CreateCommandEncoder();
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
    auto renderPass = encoder.BeginRenderPass(desc);
    renderPass.SetPipeline(ctx->pipeline);
    renderPass.Draw(3, 1, 0, 0);
    renderPass.End();
    auto cmd = encoder.Finish();

    Queue queue = device->GetQueue();

    queue.Submit({cmd});
    device->SwapContext();

    cmd.Destroy();
    encoder.Destroy();
    view.Destroy();
    texture.Destroy();
}

void ShutdownSystem(gecs::commands cmds,
                    gecs::resource<gecs::mut<Context>> ctx) {
    ctx->layout.Destroy();
    ctx->pipeline.Destroy();
    cmds.remove_resource<Device>();
    cmds.remove_resource<Adapter>();
}

void BootstrapSystem(gecs::world& world,
                     typename gecs::world::registry_type& reg) {
    nickel::Window& window = reg.commands().emplace_resource<nickel::Window>(
        "01 triangle", 1024, 720);

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
        .regist_update_system<UpdateSystem>();
}