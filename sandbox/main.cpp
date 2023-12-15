#include "core/cgmath.hpp"
#include "mirrow/serd/dynamic/backends/tomlplusplus.hpp"
#include "misc/project.hpp"
#include "nickel.hpp"
#include "renderer/font.hpp"

using namespace nickel;

FontHandle fhandle;

void TestUpdateSystem(
    gecs::querier<gecs::mut<Transform>, gecs::mut<AnimationPlayer>> querier,
    gecs::registry reg) {
    for (auto&& [entity, trans, animPlayer] : querier) {
        animPlayer.Step(1);
        animPlayer.Sync(entity, reg);
    }
}

void TestRenderFontSystem(gecs::resource<gecs::mut<Renderer2D>> renderer,
                          gecs::resource<FontManager> fontMgr,
                          gecs::resource<Camera> camera) {
    auto& font = fontMgr->Get(fhandle);
    auto glyph = font.GetGlyph('x', 16);

    if (glyph) {
        Character c(glyph);
        renderer->BeginRenderTexture(camera.get());
        renderer->DrawTexture(*c.texture,
                              cgmath::Rect{0, 0, c.size.w, c.size.h}, c.size,
                              cgmath::Color{1, 1, 1, 1}, {}, {},
                              cgmath::CreateTranslation({100, 100, 0}));
        renderer->EndRender();
    }
}

void TestInitSystem(gecs::commands cmds,
                    gecs::resource<gecs::mut<AssetManager>> assetMgr,
                    gecs::registry reg) {
    auto entity = cmds.create();
    auto handle = assetMgr->LoadTexture("./sandbox/resources/role.png",
                                        gogl::Sampler::CreateNearestRepeat());

    SpriteBundle bundle;
    bundle.sprite = Sprite::FromTexture(handle);
    bundle.transform = Transform::FromTranslation({100, 200});
    cmds.emplace_bundle<SpriteBundle>(entity, std::move(bundle));

    auto transformType = mirrow::drefl::typeinfo<Transform>();
    std::unique_ptr<BasicAnimationTrack> posTrack =
        std::unique_ptr<AnimationTrack<cgmath::Vec2>>(
            new AnimationTrack<cgmath::Vec2>{
                {KeyFrame<cgmath::Vec2>::Create(cgmath::Vec2{100.0f, 200.0f},
                 0),
                 KeyFrame<cgmath::Vec2>::Create(cgmath::Vec2{300.0f, 500.0f},
                 100)},
                transformType,
                {"position"}
    });

    std::unique_ptr<BasicAnimationTrack> rotTrack =
        std::unique_ptr<AnimationTrack<float>>(new AnimationTrack<float>{
            {KeyFrame<float>::Create(0.0, 0),
             KeyFrame<float>::Create(360, 200)},
            transformType,
            {"rotation"}
    });

    typename Animation::container_type tracks;
    tracks.emplace_back(std::move(posTrack));
    tracks.emplace_back(std::move(rotTrack));
    Animation anim{std::move(tracks)};

    anim.Save("test.anim");

    Animation anim2("", "test.anim");
}

void BootstrapSystem(gecs::world& world,
                     typename gecs::world::registry_type& reg) {
    ProjectInitInfo info;
    info.windowData.title = "sandbox";
    info.windowData.size.Set(1024, 720);
    info.projectPath = "./sandbox";

    InitSystem(world, info, reg.commands());

    reg.regist_update_system<TestUpdateSystem>()
        .regist_startup_system<TestInitSystem>()
        .regist_update_system<TestRenderFontSystem>();
}
