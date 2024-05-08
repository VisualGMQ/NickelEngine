#include "nickel.hpp"

void StartupSystem(gecs::commands cmds,
                   gecs::resource<gecs::mut<nickel::TextureManager>> mgr,
                   gecs::resource<gecs::mut<nickel::Material2DManager>> mtl2dMgr,
                   gecs::resource<gecs::mut<nickel::GLTFManager>> modelMgr) {

    auto&& [tilesheetHandle, tilesheet] = mgr->LoadAndGet("test/graphics/assets/tileset.png");
    auto tilesheetMtl = mtl2dMgr->Create("test.mtl2d", tilesheetHandle);
    /*
    {
        auto ent = cmds.create();

        nickel::SpriteBundle bundle;
        bundle.transform.translation.Set(0, 0);
        bundle.transform.SetScale({0.5, 0.5});
        bundle.material.material = tilesheetMtl;
        cmds.emplace_bundle<nickel::SpriteBundle>(ent, std::move(bundle));
    }

    {
        auto ent = cmds.create();

        nickel::SpriteBundle bundle;
        bundle.transform.translation.Set(0, 0);
        bundle.transform.SetScale({0.2, 0.2});
        bundle.material.material = tilesheetMtl;
        cmds.emplace_bundle<nickel::SpriteBundle>(ent, std::move(bundle));
    }

    {
        auto ent = cmds.create();
        nickel::GLTFBundle bundle;
        bundle.transform.scale.Set(10, 10);
        bundle.gltf = modelMgr->Load("external/glTF-Sample-Models/2.0/CesiumMilkTruck/glTF/CesiumMilkTruck.gltf");
        cmds.emplace_bundle(ent, std::move(bundle));
    }
    */

    {
        auto ent = cmds.create();
        nickel::ui::Style style;
        style.center.Set(400, 400);
        cmds.emplace<nickel::ui::Style>(ent, style);

        nickel::ui::Button btn;
        btn.hoverColor.Set(0, 1, 0, 1);
        btn.pressColor.Set(0, 0, 1, 1);
        nickel::cgmath::Vec2 uv0{16, 16}, uv1{32, 32};
        uv0 /= tilesheet.Size();
        uv1 /= tilesheet.Size();
        btn.texture.handle = tilesheetHandle;
        btn.texture.uv0 = uv0;
        btn.texture.uv1 = uv1;

        btn.pressTexture = btn.texture;
        btn.hoverTexture = btn.texture;
        cmds.emplace<nickel::ui::Button>(ent, btn);
    }
}

void BootstrapSystem(gecs::world& world,
                     typename gecs::world::registry_type& reg) {
    nickel::ProjectInitInfo initInfo;
    initInfo.windowData.title = "renderer2d";
    initInfo.windowData.size.Set(720, 480);
    InitSystem(world, initInfo, reg.commands());
    nickel::RegistEngineSystem(*world.cur_registry());

    reg.regist_startup_system<StartupSystem>();
}
