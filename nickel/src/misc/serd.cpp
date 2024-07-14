#include "misc/serd.hpp"
#include "anim/anim.hpp"
#include "audio/audio.hpp"
#include "graphics/font.hpp"
#include "graphics/gltf.hpp"
#include "graphics/sprite.hpp"
#include "graphics/texture.hpp"
#include "common/asset_manager.hpp"
#include "graphics/tilesheet.hpp"

namespace nickel {

void SerializeSoundPlayer(toml::node& node, const mirrow::drefl::any& payload) {
    Assert(node.is_table(), "node type must be table");
    Assert(payload.type_info() == mirrow::drefl::typeinfo<SoundPlayer>(),
           "payload type must be SoundPlayer");

    auto tbl = node.as_table();

    auto player = mirrow::drefl::try_cast_const<SoundPlayer>(payload);
    if (player->Handle()) {
        auto handleTbl = mirrow::serd::drefl::serialize_class(
            mirrow::drefl::any_make_copy(player->Handle()));
        tbl->emplace("sound", handleTbl);
    }
}

void DeserializeSoundPlayer(const toml::node& node, mirrow::drefl::any& payload) {
    Assert(node.is_table(), "node type must be table");
    Assert(payload.type_info() == mirrow::drefl::typeinfo<SoundPlayer>(),
           "payload type must be SoundPlayer");


    auto tbl = node.as_table();

    if (auto sound = tbl->get("sound"); sound && sound->is_table()) {
        auto soundHandle = mirrow::drefl::typeinfo<SoundHandle>()->default_construct();
        mirrow::serd::drefl::deserialize(soundHandle, *sound);

        auto player = mirrow::drefl::try_cast<SoundPlayer>(payload);
        player->ChangeSound(
            *mirrow::drefl::try_cast_const<SoundHandle>(soundHandle));
    }
}

void SerializeAnimationPlayer(toml::node& node, const mirrow::drefl::any& payload) {
    Assert(node.is_table(), "node type must be table");
    Assert(payload.type_info() == mirrow::drefl::typeinfo<SoundPlayer>(),
           "payload type must be AnimationPlayer");

    auto tbl = node.as_table();

    auto player = mirrow::drefl::try_cast_const<AnimationPlayer>(payload);
    if (player->Anim()) {
        auto handleTbl = mirrow::serd::drefl::serialize_class(
            mirrow::drefl::any_make_copy(player->Anim()));
        tbl->emplace("animation", handleTbl);
    }
}

void DeserializeAnimationPlayer(const toml::node& node, mirrow::drefl::any& payload) {
    Assert(node.is_table(), "node type must be table");
    Assert(payload.type_info() == mirrow::drefl::typeinfo<SoundPlayer>(),
           "payload type must be AnimationPlayer");

    auto tbl = node.as_table();

    if (auto anim = tbl->get("animation"); anim && anim->is_table()) {
        auto animHandle = mirrow::drefl::typeinfo<AnimationHandle>()->default_construct();
        mirrow::serd::drefl::deserialize(animHandle, *anim);

        mirrow::drefl::try_cast<AnimationPlayer>(payload)->ChangeAnim(
            *mirrow::drefl::try_cast_const<AnimationHandle>(animHandle));
    }
}

template <typename T>
void SerializeHandle(toml::node& node, const mirrow::drefl::any& payload) {
    using HandleT = Handle<T>;
    Assert(payload.type_info() == mirrow::drefl::typeinfo<HandleT>(),
           "payload type must be Handle<>");
    Assert(node.is_table(), "Handle<> must serialize to table");

    auto handle = *mirrow::drefl::try_cast_const<HandleT>(payload);

    auto tbl = node.as_table();

    if (handle) {
        auto& elem = *handle.GetDataConst();

        tbl->emplace("path", elem.GetRelativePath().string());
    }
}

template <typename T>
void DeserializeHandle(const toml::node& node, mirrow::drefl::any& payload) {
    using HandleT = Handle<T>;
    Assert(node.is_table(), "node type must be table");
    using HandleT = Handle<T>;
    Assert(payload.type_info() == mirrow::drefl::typeinfo<HandleT>(),
           "payload type must be Handle<>");

    auto& tbl = *node.as_table();

    if (auto node = tbl.get("path"); node && node->is_string()) {
        auto handle =
            AssetManager::Instance().Find<T>(node->as_string()->get());
        payload.steal_assign(mirrow::drefl::any_make_copy(handle));
    }
}

template <typename T>
void RegistSerdMethod(
    mirrow::serd::drefl::serialize_method_storage::serialize_fn ser,
    mirrow::serd::drefl::serialize_method_storage::deserialize_fn deser) {
    auto typeinfo = mirrow::drefl::typeinfo<T>();
    auto& registrar = mirrow::serd::drefl::serialize_method_storage::instance();
    registrar.regist_serialize(typeinfo, ser);
    registrar.regist_deserialize(typeinfo, deser);
}

void RegistSerdMethod(
    const mirrow::drefl::type* typeinfo,
    mirrow::serd::drefl::serialize_method_storage::serialize_fn ser,
    mirrow::serd::drefl::serialize_method_storage::deserialize_fn deser) {
    auto& registrar = mirrow::serd::drefl::serialize_method_storage::instance();
    registrar.regist_serialize(typeinfo, ser);
    registrar.regist_deserialize(typeinfo, deser);
}

void RegistSerializeMethods() {
    RegistSerdMethod(mirrow::drefl::typeinfo<SoundPlayer>(),
                     SerializeSoundPlayer, DeserializeSoundPlayer);
    RegistSerdMethod(mirrow::drefl::typeinfo<AnimationPlayer>(),
                     SerializeAnimationPlayer, DeserializeAnimationPlayer);

    RegistSerdMethod<TextureHandle>(SerializeHandle<Texture>,
                                    DeserializeHandle<Texture>);
    RegistSerdMethod<SoundHandle>(SerializeHandle<Sound>,
                                  DeserializeHandle<Sound>);
    RegistSerdMethod<AnimationHandle>(SerializeHandle<Animation>,
                                      DeserializeHandle<Animation>);
    RegistSerdMethod<FontHandle>(SerializeHandle<Font>,
                                 DeserializeHandle<Font>);
    RegistSerdMethod<TilesheetHandle>(SerializeHandle<Tilesheet>,
                                      DeserializeHandle<Tilesheet>);
    RegistSerdMethod<GLTFHandle>(SerializeHandle<GLTFModel>,
                                 DeserializeHandle<GLTFModel>);
    RegistSerdMethod<Material2DHandle>(SerializeHandle<Material2D>,
                                       DeserializeHandle<Material2D>);
    RegistSerdMethod<TimerHandle>(SerializeHandle<Timer>,
                                  DeserializeHandle<Timer>);
    RegistSerdMethod<TimerHandle>(SerializeHandle<Timer>,
                                  DeserializeHandle<Timer>);
}

}  // namespace nickel