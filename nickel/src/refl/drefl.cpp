#include "refl/drefl.hpp"
#include "mirrow/drefl/cast_any.hpp"
#include "mirrow/drefl/factory.hpp"
#include "common/transform.hpp"
#include "misc/prefab.hpp"
#include "graphics/sprite.hpp"
#include "anim/anim.hpp"
#include "graphics/tilesheet.hpp"
#include "graphics/tilesheet.hpp"
#include "misc/name.hpp"
#include "audio/audio.hpp"
#include "ui/ui.hpp"

namespace nickel {

void reflectVec2() {
    mirrow::drefl::registrar<cgmath::Vec2>::instance()
        .regist("Vec2")
        .property("x", &cgmath::Vec2::x)
        .property("y", &cgmath::Vec2::y);
}

void reflectVec3() {
    mirrow::drefl::registrar<cgmath::Vec3>::instance()
        .regist("Vec3")
        .property("x", &cgmath::Vec3::x)
        .property("y", &cgmath::Vec3::y)
        .property("z", &cgmath::Vec3::z);
}

void reflectVec4() {
    mirrow::drefl::registrar<cgmath::Vec4>::instance()
        .regist("Vec4")
        .property("x", &cgmath::Vec4::x)
        .property("y", &cgmath::Vec4::y)
        .property("w", &cgmath::Vec4::w)
        .property("z", &cgmath::Vec4::z);
}

void reflectRect() {
    mirrow::drefl::registrar<cgmath::Rect>::instance()
        .regist("Rect")
        .property("position", &cgmath::Rect::position)
        .property("size", &cgmath::Rect::size);
}

void reflectTramsform() {
    mirrow::drefl::registrar<Transform>::instance()
        .regist("Transform")
        .property("translation", &Transform::translation)
        .property("rotation", &Transform::rotation)
        .property("scale", &Transform::scale);

    PrefabEmplaceMethods::Instance().RegistEmplaceFn<Transform>();
}

void reflectGlobalTramsform() {
    mirrow::drefl::registrar<GlobalTransform>::instance().regist(
        "GlobalTransform");

    PrefabEmplaceMethods::Instance().RegistEmplaceFn<GlobalTransform>();
}

void reflectSprite() {
    mirrow::drefl::registrar<Sprite>::instance()
        .regist("Sprite")
        .property("anchor", &Sprite::anchor, {AttrRange01})
        .property("color", &Sprite::color, {AttrColor})
        .property("region", &Sprite::region)
        .property("customSize", &Sprite::customSize)
        .property("flip", &Sprite::flip)
        .property("visiable", &Sprite::visiable)
        .property("texture", &Sprite::texture)
        .property("z-index", &Sprite::zIndex);

    mirrow::drefl::registrar<Flip>::instance()
        .regist("Flip")
        .add("None", Flip::None)
        .add("Horizontal", Flip::Horizontal)
        .add("Vertical", Flip::Vertical)
        .add("Both", Flip::Both);

    mirrow::drefl::registrar<TextureHandle>::instance().regist("TextureHandle");

    PrefabEmplaceMethods::Instance().RegistEmplaceFn<Sprite>();
    PrefabEmplaceMethods::Instance().RegistEmplaceFn<Flip>();
}

void reflectAnimation() {
    mirrow::drefl::registrar<AnimationPlayer>::instance().regist(
        "AnimationPlayer");

    PrefabEmplaceMethods::Instance().RegistEmplaceFn<AnimationPlayer>();
}

void reflectUI() {
    mirrow::drefl::registrar<ui::Style>::instance()
        .regist("UIStyle")
        .property("center", &ui::Style::center)
        .property("size", &ui::Style::size)
        .property("padding", &ui::Style::padding)
        .property("margin", &ui::Style::margin)
        .property("background-color", &ui::Style::backgroundColor, {AttrColor})
        .property("border-color", &ui::Style::borderColor, {AttrColor})
        .property("border-size", &ui::Style::borderSize);

    mirrow::drefl::registrar<ui::Button>::instance()
        .regist("Button")
        .property("color", &ui::Button::color, {AttrColor})
        .property("hover-color", &ui::Button::hoverColor, {AttrColor})
        .property("press-color", &ui::Button::pressColor, {AttrColor});

    mirrow::drefl::registrar<ui::Label>::instance()
        .regist("Label")
        .property("color", &ui::Label::color, {AttrColor})
        .property("hoverColor", &ui::Label::hoverColor, {AttrColor})
        .property("pressColor", &ui::Label::pressColor, {AttrColor});

    PrefabEmplaceMethods::Instance().RegistEmplaceFn<ui::Style>();
    PrefabEmplaceMethods::Instance().RegistEmplaceFn<ui::Button>();
    PrefabEmplaceMethods::Instance().RegistEmplaceFn<ui::Label>();
}

void reflectRenderRelate() {
    mirrow::drefl::registrar<gogl::Sampler>::instance()
        .regist("Sampler")
        .property("filter", &gogl::Sampler::filter)
        .property("mipmap", &gogl::Sampler::mipmap)
        .property("wrapper", &gogl::Sampler::wrapper);

    mirrow::drefl::registrar<gogl::Sampler::Wrapper>::instance()
        .regist("Wrapper")
        .property("borderColor", &gogl::Sampler::Wrapper::borderColor)
        .property("r", &gogl::Sampler::Wrapper::r)
        .property("s", &gogl::Sampler::Wrapper::s)
        .property("t", &gogl::Sampler::Wrapper::t);

    mirrow::drefl::registrar<gogl::Sampler::Filter>::instance()
        .regist("Filter")
        .property("min", &gogl::Sampler::Filter::min)
        .property("mag", &gogl::Sampler::Filter::mag);

    mirrow::drefl::registrar<gogl::TextureFilterType>::instance()
        .regist("TextureFilterType")
        .add("Linear", gogl::TextureFilterType::Linear)
        .add("Nearest", gogl::TextureFilterType::Nearest);
        // TODO: add Other filter type enum

    mirrow::drefl::registrar<gogl::TextureWrapperType>::instance()
        .regist("TextureWrapperType")
        .add("Repeat", gogl::TextureWrapperType::Repeat)
        .add("MirrowRepeat", gogl::TextureWrapperType::MirroredRepeat)
        .add("ClampToBorder", gogl::TextureWrapperType::ClampToBorder)
        .add("ClampToEdge", gogl::TextureWrapperType::ClampToEdge);

    PrefabEmplaceMethods::Instance().RegistEmplaceFn<gogl::Sampler>();
    PrefabEmplaceMethods::Instance().RegistEmplaceFn<gogl::Sampler::Wrapper>();
    PrefabEmplaceMethods::Instance().RegistEmplaceFn<gogl::TextureFilterType>();
    PrefabEmplaceMethods::Instance().RegistEmplaceFn<gogl::TextureWrapperType>();
}

void reflectWindow() {
    mirrow::drefl::registrar<WindowBuilder::Data>::instance()
        .regist("WindowData")
        .property("title", &WindowBuilder::Data::title)
        .property("size", &WindowBuilder::Data::size);

    PrefabEmplaceMethods::Instance().RegistEmplaceFn<WindowBuilder::Data>();
}

void reflectTilesheet() {
    mirrow::drefl::registrar<Margin>::instance()
        .regist("Margin")
        .property("left", &Margin::left)
        .property("right", &Margin::right)
        .property("top", &Margin::top)
        .property("bottom", &Margin::bottom);

    mirrow::drefl::registrar<Spacing>::instance()
        .regist("Spacing")
        .property("x", &Spacing::x)
        .property("y", &Spacing::y);

    mirrow::drefl::registrar<Tilesheet>::instance()
        .regist("Tilesheet");

    PrefabEmplaceMethods::Instance().RegistEmplaceFn<Margin>();
    PrefabEmplaceMethods::Instance().RegistEmplaceFn<Spacing>();
    PrefabEmplaceMethods::Instance().RegistEmplaceFn<Tilesheet>();
}

void reflectMisc() {
    mirrow::drefl::registrar<Name>::instance().regist("Name").property(
        "name", &Name::name);

    PrefabEmplaceMethods::Instance().RegistEmplaceFn<Name>();

    mirrow::drefl::registrar<gecs::entity>::instance().regist("entity");
}

void serializeTextureHandle(toml::node& node, const mirrow::drefl::any& elem) {
    Assert(node.is_table(), "serialize texture-handle to table");
    auto mgr = ECS::Instance().World().res<TextureManager>();
    auto handle =  mirrow::drefl::try_cast_const<TextureHandle>(elem);
    if (mgr->Has(*handle)) {
        auto& texture = mgr->Get(*handle);
        node.as_table()->emplace("path", texture.RelativePath().string());
    }
}

void deserializeTextureHandle(const toml::node& node, mirrow::drefl::any& elem) {
    Assert(node.is_table(), "serialize texture-handle to table");
    auto& tbl = *node.as_table();

    if (auto path = tbl.get("path"); path && path->is_string())  {
        auto filename = path->as_string()->get();
        auto& handle = *mirrow::drefl::try_cast<TextureHandle>(elem);

        auto mgr = ECS::Instance().World().res_mut<TextureManager>();
        if (mgr->Has(filename)) {
            handle = mgr->GetHandle(filename);
        } else {
            handle = mgr->Load(filename, gogl::Sampler::CreateLinearRepeat());
        }
    }
}

void registTextureHandleSerd() {
    auto& serd = mirrow::serd::drefl::serialize_method_storage::instance();
    serd.regist_serialize(mirrow::drefl::typeinfo<TextureHandle>(), serializeTextureHandle);
    serd.regist_deserialize(mirrow::drefl::typeinfo<TextureHandle>(), deserializeTextureHandle);

    PrefabEmplaceMethods::Instance().RegistEmplaceFn<TextureHandle>();
}

void serializeSoundPlayer(toml::node& node, const mirrow::drefl::any& elem) {
    Assert(node.is_table(), "serialize sound-handle to table");
    auto mgr = ECS::Instance().World().res<AudioManager>();
    auto& player = *mirrow::drefl::try_cast_const<SoundPlayer>(elem);
    if (mgr->Has(player.Handle())) {
        auto& sound = mgr->Get(player.Handle());
        toml::table tbl;
        tbl.emplace("path", sound.RelativePath().string());
        node.as_table()->emplace("SoundPlayer", tbl);
    }
}

void deserializeSoundPlayer(const toml::node& node, mirrow::drefl::any& elem) {
    Assert(node.is_table(), "serialize sound player to table");
    auto& tbl = *node.as_table();

    if (auto path = tbl.get("path"); path && path->is_string())  {
        auto filename = tbl.get("path")->as_string()->get();
        auto& player = *mirrow::drefl::try_cast<SoundPlayer>(elem);

        auto mgr = ECS::Instance().World().res_mut<AudioManager>();
        if (mgr->Has(filename)) {
            player.ChangeSound(mgr->GetHandle(filename), mgr.get());
        }
    }
}

void registSoundPlayerSerd() {
    auto& serd = mirrow::serd::drefl::serialize_method_storage::instance();
    serd.regist_serialize(mirrow::drefl::typeinfo<SoundPlayer>(), serializeSoundPlayer);
    serd.regist_deserialize(mirrow::drefl::typeinfo<SoundPlayer>(), deserializeSoundPlayer);

    PrefabEmplaceMethods::Instance().RegistEmplaceFn<SoundPlayer>();
}

void serializeAnimationPlayer(toml::node& node, const mirrow::drefl::any& elem) {
    Assert(node.is_table(), "serialize animation-handle to table");
    auto& player = *mirrow::drefl::try_cast_const<AnimationPlayer>(elem);
    auto mgr = ECS::Instance().World().res<AnimationManager>();
    if (mgr->Has(player.Anim())) {
        auto& anim = mgr->Get(player.Anim());
        toml::table tbl;
        tbl.emplace("path", anim.RelativePath().string());
        node.as_table()->emplace("AnimationPlayer", tbl);
    }
}

void deserializeAnimationPlayer(const toml::node& node, mirrow::drefl::any& elem) {
    Assert(node.is_table(), "serialize sound player to table");
    auto& tbl = *node.as_table();

    auto mgr = ECS::Instance().World().res<AnimationManager>();
    if (auto path = tbl.get("path"); path && path->is_string())  {
        auto filename = tbl.get("path")->as_string()->get();
        auto& player = *mirrow::drefl::try_cast<AnimationPlayer>(elem);

        if (mgr->Has(filename)) {
            player.ChangeAnim(mgr->GetHandle(filename));
        }
    }
}


void registAnimationPlayerSerd() {
    auto& serd = mirrow::serd::drefl::serialize_method_storage::instance();
    serd.regist_serialize(mirrow::drefl::typeinfo<AnimationPlayer>(), serializeAnimationPlayer);
    serd.regist_deserialize(mirrow::drefl::typeinfo<AnimationPlayer>(), deserializeAnimationPlayer);

    PrefabEmplaceMethods::Instance().RegistEmplaceFn<AnimationPlayer>();
}

void serializeGlobalTransform(toml::node& node, const mirrow::drefl::any& elem) {
    Assert(elem.type_info() == mirrow::drefl::typeinfo<GlobalTransform>(),
           "elem type incorrect");
    Assert(node.is_table(), "serialize GlobalTransform require table");

    node.as_table()->emplace(elem.type_info()->name(), toml::table{});
}

void deserializeGlobalTransform(const toml::node& node, mirrow::drefl::any& elem) {
    Assert(elem.type_info() == mirrow::drefl::typeinfo<GlobalTransform>(),
           "elem type incorrect");
    Assert(node.is_table(), "serialize GlobalTransform require table");

    mirrow::drefl::try_cast<GlobalTransform>(elem)->mat = cgmath::Mat44::Identity();
}

void registGlobalTransformSerd() {
    auto& serd = mirrow::serd::drefl::serialize_method_storage::instance();
    serd.regist_serialize(mirrow::drefl::typeinfo<GlobalTransform>(), serializeGlobalTransform);
    serd.regist_deserialize(mirrow::drefl::typeinfo<GlobalTransform>(), deserializeGlobalTransform);
}

void reflectHierarchy() {
    mirrow::drefl::registrar<Parent>::instance().regist("Parent", {EditorNodisplay}).property(
        "entity", &Parent::entity);
    mirrow::drefl::registrar<Child>::instance().regist("Child", {EditorNodisplay}).property(
        "entities", &Child::entities);
}

void reflectAudio() {
    mirrow::drefl::registrar<SoundPlayer>::instance().regist("SoundPlayer");
}

void InitDynamicReflect() {
    reflectVec2();
    reflectVec3();
    reflectVec4();
    reflectRect();
    reflectTramsform();
    reflectGlobalTramsform();
    reflectAnimation();
    reflectSprite();
    reflectUI();
    reflectRenderRelate();
    reflectWindow();
    reflectTilesheet();
    reflectHierarchy();
    reflectMisc();
    reflectAudio();

    registTextureHandleSerd();
    registSoundPlayerSerd();
    registAnimationPlayerSerd();
    registGlobalTransformSerd();
}

}  // namespace nickel