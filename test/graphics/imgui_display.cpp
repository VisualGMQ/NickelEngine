#include "graphics/rhi/vk/texture_view.hpp"
#include "graphics/system.hpp"
#include "nickel.hpp"
#include "imgui_plugin.hpp"

nickel::TextureHandle handle1;
nickel::TextureHandle handle2;

void StartupSystem(gecs::resource<gecs::mut<nickel::TextureManager>> textureMgr,
                    gecs::resource<plugin::ImGuiVkContext> ctx,
                    gecs::resource<nickel::rhi::Device> device) {
    handle1 = textureMgr->Load("test/graphics/assets/demon.png");
    handle2 = textureMgr->Load("test/graphics/assets/chest.png");
}

void UpdateSystem(gecs::resource<gecs::mut<nickel::TextureManager>> textureMgr,
                  gecs::resource<nickel::rhi::Device> device) {
    ImGui::ShowDemoWindow();

    auto vkDevice =
        static_cast<nickel::rhi::vulkan::DeviceImpl*>(device->Impl());

    auto& texture1 = textureMgr->Get(handle1);
    auto& texture2 = textureMgr->Get(handle2);

    ImGui::Image(texture1, ImVec2(texture1.Width(), texture1.Height()));
    ImGui::Image(texture2, ImVec2(texture2.Width(), texture2.Height()));
}

void BootstrapSystem(gecs::world& world,
                     typename gecs::world::registry_type& reg) {
    nickel::ProjectInitInfo initInfo;
    initInfo.windowData.title = "imgui demo";
    initInfo.windowData.size.Set(720, 480);
    InitSystem(world, initInfo, reg.commands());

    reg
        // startup systems
        .regist_startup_system<nickel::VideoSystemInit>()
        .regist_startup_system<nickel::RenderSystemInit>()
        .regist_startup_system<nickel::FontSystemInit>()
        .regist_startup_system<nickel::EventPollerInit>()
        .regist_startup_system<nickel::InputSystemInit>()
        // .regist_startup_system<ui::InitSystem>()
        .regist_startup_system<nickel::InitAudioSystem>()
        .regist_startup_system<plugin::ImGuiInit>()
        .regist_startup_system<StartupSystem>()
        // shutdown systems
        .regist_shutdown_system<plugin::ImGuiShutdown>()
        .regist_shutdown_system<nickel::EngineShutdown>()
        .regist_shutdown_system<nickel::InitAudioSystem>()
        // update systems
        .regist_update_system<nickel::VideoSystemUpdate>()
        // other input handle event must put here(after mouse/keyboard update)
        .regist_update_system<nickel::Mouse::Update>()
        .regist_update_system<nickel::Keyboard::Update>()
        .regist_update_system<nickel::HandleInputEvents>()
        .regist_update_system<nickel::UpdateGlobalTransform>()
        .regist_update_system<nickel::UpdateGLTFModelTransform>()
        .regist_update_system<nickel::UpdateCamera2GPU>()
        // .regist_update_system<ui::UpdateGlobalPosition>()
        // .regist_update_system<ui::HandleEventSystem>()
        // start render pipeline
        .regist_update_system<nickel::BeginRender>()
        .regist_update_system<nickel::RenderGLTFModel>()
        .regist_update_system<nickel::RenderSprite2D>()
        .regist_update_system<plugin::ImGuiStart>()
        .regist_update_system<UpdateSystem>()
        .regist_update_system<plugin::ImGuiEnd>()
        .regist_update_system<nickel::EndRender>()
        .regist_update_system<nickel::SwapContext>()
        // 2D UI render
        // .regist_update_system<ui::RenderUI>()
        // time update
        .regist_update_system<nickel::Time::Update>();
}
