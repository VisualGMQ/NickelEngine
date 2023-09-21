#include "nickel.hpp"

using namespace nickel;

void TestUpdateSystem(gecs::querier<gecs::mut<Transform>, gecs::mut<AnimationPlayer>> querier) {
    for (auto& [_, trans, animPlayer] : querier) {
        animPlayer.Step(1);
        animPlayer.AsyncTo(trans);
    }
}

void TestInitSystem(gecs::commands cmds, gecs::resource<gecs::mut<TextureManager>> textureMgr, gecs::resource<gecs::mut<AnimationManager>> animMgr) {
    auto entity = cmds.create();
    auto handle = textureMgr->Load("./sandbox/resources/role.png", gogl::Sampler::CreateNearestRepeat());

    cmds.emplace<SpriteBundle>(entity, SpriteBundle{
        Sprite::Default(),
        handle,
        Flip::Vertical,
    });

    cmds.emplace<Transform>(entity, Transform::FromTranslation({100, 200}));

    std::unique_ptr<AnimationTrack> posTrack =
        std::make_unique<BasicAnimationTrack<cgmath::Vec2>>(
            std::vector{KeyFrame<cgmath::Vec2>::Create(cgmath::Vec2{100.0f, 200.0f}, 0),
             KeyFrame<cgmath::Vec2>::Create(cgmath::Vec2{300.0f, 500.0f}, 100)});

    std::unique_ptr<AnimationTrack> rotTrack =
        std::make_unique<BasicAnimationTrack<float>>(
            std::vector{KeyFrame<float>::Create(0.0, 0),
             KeyFrame<float>::Create(360, 200)});

    Animation::container_type tracks;
    tracks["translation"] = std::move(posTrack);
    tracks["rotation"] = std::move(rotTrack);
    auto anim = animMgr->CreateFromTracks(std::move(tracks));

    cmds.emplace<AnimationPlayer>(entity, AnimationPlayer(anim, animMgr.get())).Play();
}

void BootstrapSystem(gecs::world& world, typename gecs::world::registry_type& reg) {
    reg.regist_update_system<TestUpdateSystem>()
    .regist_startup_system<TestInitSystem>();
}