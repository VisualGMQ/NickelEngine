#include "graphics/system.hpp"
#include "imgui_plugin.hpp"
#include "nickel.hpp"


nickel::TextureHandle handle1;
nickel::TextureHandle handle2;

void StartupSystem(gecs::resource<gecs::mut<nickel::TextureManager>> textureMgr,
                   gecs::resource<nickel::rhi::Device> device) {
    handle1 = textureMgr->Load("test/graphics/assets/demon.png");
    handle2 = textureMgr->Load("test/graphics/assets/chest.png");
}

void UpdateSystem(gecs::resource<gecs::mut<nickel::TextureManager>> textureMgr,
                  gecs::resource<nickel::rhi::Device> device) {
    ImGui::ShowDemoWindow();

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
    nickel::RegistEngineSystem(reg);

    reg.regist_startup_system<plugin::ImGuiInit>()
        .regist_startup_system<StartupSystem>()
        .regist_shutdown_system_before<plugin::ImGuiShutdown,
                                       nickel::EngineShutdown>()
        .regist_update_system_after<plugin::ImGuiStart,
                                    nickel::RenderSprite2D>()
        .regist_update_system_after<UpdateSystem, plugin::ImGuiStart>()
        .regist_update_system_after<plugin::ImGuiEnd, UpdateSystem>();
}
