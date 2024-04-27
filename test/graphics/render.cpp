#include "nickel.hpp"

void StartupSystem(gecs::commands cmds,
                   gecs::resource<gecs::mut<nickel::TextureManager>> mgr,
                   gecs::resource<gecs::mut<nickel::Material2DManager>> mtl2dMgr,
                   gecs::resource<gecs::mut<nickel::GLTFManager>> modelMgr) {
    auto ent = cmds.create();

    {
        auto texture = mgr->Load("test/graphics/assets/tileset.png");

        nickel::SpriteBundle bundle;
        bundle.transform.translation.Set(0, 0);
        bundle.transform.SetScale({0.5, 0.5});
        bundle.material.material = mtl2dMgr->Create("test.mtl2d", texture);
        cmds.emplace_bundle<nickel::SpriteBundle>(ent, std::move(bundle));
    }

    {
        ent = cmds.create();
        nickel::GLTFBundle bundle;
        bundle.transform.scale.Set(10, 10);
        bundle.gltf = modelMgr->Load("external/glTF-Sample-Models/2.0/CesiumMilkTruck/glTF/CesiumMilkTruck.gltf");
        cmds.emplace_bundle(ent, std::move(bundle));
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
