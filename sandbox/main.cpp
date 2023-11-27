#include "mirrow/serd/dynamic/backends/tomlplusplus.hpp"
#include "misc/project.hpp"
#include "nickel.hpp"
#include "refl/anim.hpp"

using namespace nickel;

void TestUpdateSystem(
    gecs::querier<gecs::mut<Transform>, gecs::mut<AnimationPlayer>> querier, gecs::registry reg) {
    for (auto&& [entity, trans, animPlayer] : querier) {
        animPlayer.Step(1);
        animPlayer.Sync(entity, reg);
    }
}

void TestInitSystem(gecs::commands cmds,
                    gecs::resource<gecs::mut<TextureManager>> textureMgr,
                    gecs::resource<gecs::mut<AnimationManager>> animMgr,
                    gecs::registry reg) {
    auto entity = cmds.create();
    auto handle = textureMgr->Load("./sandbox/resources/role.png",
                                   gogl::Sampler::CreateNearestRepeat());

    SpriteBundle bundle;
    bundle.sprite = Sprite::FromTexture(handle);
    bundle.transform = Transform::FromTranslation({100, 200});
    cmds.emplace_bundle<SpriteBundle>(entity, std::move(bundle));

    std::unique_ptr<AnimationTrack> posTrack =
        std::make_unique<BasicAnimationTrack<cgmath::Vec2>>(std::vector{
            KeyFrame<cgmath::Vec2>::Create(cgmath::Vec2{100.0f, 200.0f}, 0),
            KeyFrame<cgmath::Vec2>::Create(cgmath::Vec2{300.0f, 500.0f}, 100)});
    posTrack->ChangeApplyTarget(mirrow::drefl::typeinfo<Transform>());

    std::unique_ptr<AnimationTrack> rotTrack =
        std::make_unique<BasicAnimationTrack<float>>(
            std::vector{KeyFrame<float>::Create(0.0, 0),
                        KeyFrame<float>::Create(360, 200)});
    rotTrack->ChangeApplyTarget(mirrow::drefl::typeinfo<Transform>());

    Animation::container_type tracks;
    tracks["translation"] = std::move(posTrack);
    tracks["rotation"] = std::move(rotTrack);
    auto anim = animMgr->CreateFromTracks(std::move(tracks));

    toml::table tbl;
    mirrow::serd::srefl::serialize<Animation>(animMgr->Get(anim), tbl);
    toml::toml_formatter fmt{tbl};

    std::stringstream ss;

    ss << fmt;

    std::string tomlData = ss.str();

    std::ofstream file("anim.res.toml");
    file << fmt;
    file.close();

    Animation serdAnim;
    mirrow::serd::srefl::deserialize<Animation>(toml::parse(tomlData).table(),
                                                serdAnim);

    auto& animPlayer = cmds.emplace<AnimationPlayer>(entity, AnimationPlayer(animMgr.get()));
    animPlayer.ChangeAnim(anim);
    animPlayer.Play();

    tbl = SaveAsPrefab(entity, reg);
    std::ofstream file2("ent1.prefab.toml");
    file2 << tbl;
    file2.close();
}

void BootstrapSystem(gecs::world& world,
                     typename gecs::world::registry_type& reg) {
    ProjectInitInfo info;
    info.windowData.title = "sandbox";
    info.windowData.size.Set(1024, 720);
    info.projectPath = "./sandbox";

    InitSystem(world, info, reg.commands());

    reg.regist_update_system<TestUpdateSystem>()
        .regist_startup_system<TestInitSystem>();
}
