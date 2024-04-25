#include "misc/serd.hpp"
#include "anim/anim.hpp"
#include "audio/audio.hpp"
#include "graphics/sprite.hpp"
#include "graphics/texture.hpp"
#include "misc/asset_manager.hpp"

namespace nickel {

toml::table SerializeSoundPlayer(const mirrow::drefl::any& payload) {
    Assert(payload.type_info() == mirrow::drefl::typeinfo<SoundPlayer>(),
           "payload type must be SoundPlayer");

    auto mgr = ECS::Instance().World().res<AudioManager>();

    auto player = mirrow::drefl::try_cast_const<SoundPlayer>(payload);
    if (mgr->Has(player->Handle())) {
        toml::table tbl;

        auto handleTbl = SerializeMethodRegistrar::Instance().Serialize(
            mirrow::drefl::any_make_copy(player->Handle()));
        tbl.emplace("sound", handleTbl);
        return tbl;
    }
    return {};
}

mirrow::drefl::any DeserializeSoundPlayer(const toml::node& node) {
    Assert(node.is_table(), "node type must be table");

    auto& tbl = *node.as_table();

    if (auto sound = tbl.get("sound"); sound && sound->is_table()) {
        auto soundHandle = SerializeMethodRegistrar::Instance().Deserialize(
            mirrow::drefl::typeinfo<SoundHandle>(), *sound);

        SoundPlayer player;
        player.ChangeSound(
            *mirrow::drefl::try_cast_const<SoundHandle>(soundHandle));

        return mirrow::drefl::any_make_copy(std::move(player));
    }

    return {};
}

toml::table SerializeAnimationPlayer(const mirrow::drefl::any& payload) {
    Assert(payload.type_info() == mirrow::drefl::typeinfo<SoundPlayer>(),
           "payload type must be AnimationPlayer");

    auto mgr = ECS::Instance().World().res<AnimationManager>();

    auto player = mirrow::drefl::try_cast_const<AnimationPlayer>(payload);
    if (mgr->Has(player->Anim())) {
        toml::table tbl;

        auto handleTbl = SerializeMethodRegistrar::Instance().Serialize(
            mirrow::drefl::any_make_copy(player->Anim()));
        tbl.emplace("animation", handleTbl);
        return tbl;
    }
    return {};
}

mirrow::drefl::any DeserializeAnimationPlayer(const toml::node& node) {
    Assert(node.is_table(), "node type must be table");

    auto& tbl = *node.as_table();

    if (auto anim = tbl.get("animation"); anim && anim->is_table()) {
        auto animHandle = SerializeMethodRegistrar::Instance().Deserialize(
            mirrow::drefl::typeinfo<AnimationHandle>(), *anim);

        AnimationPlayer player;
        player.ChangeAnim(
            *mirrow::drefl::try_cast_const<AnimationHandle>(animHandle));

        return mirrow::drefl::any_make_copy(std::move(player));
    }

    return {};
}

template <typename T>
toml::table SerializeHandle(const mirrow::drefl::any& payload) {
    using HandleT = Handle<T>;
    Assert(payload.type_info() == mirrow::drefl::typeinfo<HandleT>(),
           "payload type must be Handle<>");

    auto mgr = ECS::Instance().World().res<AssetManager>();
    auto handle = *mirrow::drefl::try_cast_const<HandleT>(payload);

    if (mgr->Has(handle)) {
        toml::table tbl;

        auto& elem = mgr->Get(handle);

        tbl.emplace("path", elem.RelativePath().string());
        return tbl;
    }
    return {};
}

template <typename T>
mirrow::drefl::any DeserializeHandle(const toml::node& node) {
    using HandleT = Handle<T>;
    Assert(node.is_table(), "node type must be table");

    auto& tbl = *node.as_table();
    auto mgr = ECS::Instance().World().res<AssetManager>();

    if (auto anim = tbl.get("path"); anim && anim->is_string()) {
        auto handle = mgr->SwitchManager<T>().Load(anim->as_string()->get());
        return mirrow::drefl::any_make_copy(handle);
    }

    return {};
}

// toml::table SerializeSpriteMaterial(const mirrow::drefl::any& payload) {
//     Assert(payload.type_info() == mirrow::drefl::typeinfo<SpriteMaterial>(),
//            "payload type must be SpriteMaterial");
//
//     auto mgr = ECS::Instance().World().res<Material2DManager>();
//
//     auto mtl = mirrow::drefl::try_cast_const<SpriteMaterial>(payload);
//     if (mgr->Has(mtl->material)) {
//         toml::table tbl;
//
//         auto handleTbl = SerializeMethodRegistrar::Instance().Serialize(
//             mirrow::drefl::any_make_copy(mtl->material));
//         tbl.emplace("material2d", handleTbl);
//         return tbl;
//     }
//     return {};
// }
//
// mirrow::drefl::any DeserializeSpriteMaterial(const toml::node& node) {
//     Assert(node.is_table(), "node type must be table");
//
//     auto& tbl = *node.as_table();
//
//     if (auto sound = tbl.get("material2d"); sound && sound->is_table()) {
//         auto soundHandle = SerializeMethodRegistrar::Instance().Deserialize(
//             mirrow::drefl::typeinfo<SoundHandle>(), *sound);
//
//         SoundPlayer player;
//         player.ChangeSound(*mirrow::drefl::try_cast_const<SoundHandle>(soundHandle));
//
//         return mirrow::drefl::any_make_copy(std::move(player));
//     }
//
//     return {};
// }

void RegistSerializeMethods() {
    SerializeMethodRegistrar::Instance().Regist(
        mirrow::drefl::typeinfo<SoundPlayer>(), SerializeSoundPlayer,
        DeserializeSoundPlayer);
    SerializeMethodRegistrar::Instance().Regist(
        mirrow::drefl::typeinfo<AnimationPlayer>(), SerializeAnimationPlayer,
        DeserializeAnimationPlayer);

    SerializeMethodRegistrar::Instance().Regist(
        mirrow::drefl::typeinfo<TextureHandle>(), SerializeHandle<Texture>,
        DeserializeHandle<Texture>);
    SerializeMethodRegistrar::Instance().Regist(
        mirrow::drefl::typeinfo<AnimationHandle>(), SerializeHandle<Animation>,
        DeserializeHandle<Animation>);
    SerializeMethodRegistrar::Instance().Regist(
        mirrow::drefl::typeinfo<SoundHandle>(), SerializeHandle<Sound>,
        DeserializeHandle<Sound>);
    SerializeMethodRegistrar::Instance().Regist(
        mirrow::drefl::typeinfo<FontHandle>(), SerializeHandle<Font>,
        DeserializeHandle<Font>);
    SerializeMethodRegistrar::Instance().Regist(
        mirrow::drefl::typeinfo<TilesheetHandle>(), SerializeHandle<Tilesheet>,
        DeserializeHandle<Tilesheet>);
    SerializeMethodRegistrar::Instance().Regist(
        mirrow::drefl::typeinfo<GLTFHandle>(), SerializeHandle<GLTFModel>,
        DeserializeHandle<GLTFModel>);
    SerializeMethodRegistrar::Instance().Regist(
        mirrow::drefl::typeinfo<Material2DHandle>(),
        SerializeHandle<Material2D>, DeserializeHandle<Material2D>);
    SerializeMethodRegistrar::Instance().Regist(
        mirrow::drefl::typeinfo<TimerHandle>(), SerializeHandle<Timer>,
        DeserializeHandle<Timer>);
}

}  // namespace nickel