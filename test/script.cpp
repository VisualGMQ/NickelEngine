#include "nickel.hpp"
#include "common/ecs.hpp"

void BootstrapSystem(gecs::world& world,
                     typename gecs::world::registry_type& reg) {
    nickel::ProjectInitInfo info;
    info.windowData.title = "lua script";
    info.windowData.size.Set(1024, 720);

    InitSystem(world, info, reg.commands());

    auto cmd = reg.commands();
    auto entity = cmd.create();
    nickel::SpriteBundle bundle;
    auto textureMgr = reg.res<gecs::mut<nickel::TextureManager>>();
    bundle.sprite =
        nickel::Sprite::FromTexture(textureMgr->Load("test/knight.png"));
    cmd.emplace_bundle<nickel::SpriteBundle>(entity, std::move(bundle));

    auto handle = world.res_mut<nickel::ScriptManager>()->Load("test/test.luau");
    nickel::Script script{handle, entity};
    cmd.emplace<nickel::Script>(entity, std::move(script));
}
