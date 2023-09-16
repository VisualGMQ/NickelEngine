#include "nickel.hpp"

using namespace nickel;

void TestUpdateSystem(gecs::querier<gecs::mut<Transform>> transforms) {
    for (auto& [_, trans] : transforms) {
        trans.rotation += 2.0;
    }
}

void TestInitSystem(gecs::commands cmds, gecs::resource<gecs::mut<TextureManager>> textureMgr) {
    auto entity = cmds.create();
    auto handle = textureMgr->Load("./sandbox/resources/role.png", gogl::Sampler::CreateNearestRepeat());
    cmds.emplace<SpriteBundle>(entity, SpriteBundle{
        Sprite::Default(),
        handle,
        Flip::Vertical,
    });
    cmds.emplace<Transform>(entity, Transform::FromTranslation({100, 200}).SetRotation(30.0f));
}

void BootstrapSystem(gecs::world& world, typename gecs::world::registry_type& reg) {
    reg.regist_update_system<TestUpdateSystem>()
    .regist_startup_system<TestInitSystem>();
}