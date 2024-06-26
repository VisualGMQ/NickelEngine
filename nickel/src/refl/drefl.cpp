#include "refl/drefl.hpp"
#include "graphics/gltf.hpp"
#include "anim/anim.hpp"
#include "audio/audio.hpp"
#include "common/transform.hpp"
#include "graphics/sprite.hpp"
#include "graphics/tilesheet.hpp"
#include "mirrow/drefl/cast_any.hpp"
#include "mirrow/drefl/factory.hpp"
#include "misc/name.hpp"
#include "misc/prefab.hpp"
#include "script/script.hpp"
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

void reflectMat() {
    mirrow::drefl::registrar<cgmath::Mat44>::instance()
        .regist("Mat44")
        .property("data", &cgmath::Mat44::data);

    mirrow::drefl::registrar<cgmath::Mat33>::instance()
        .regist("Mat33")
        .property("data", &cgmath::Mat33::data);

    mirrow::drefl::registrar<cgmath::Mat22>::instance()
        .regist("Mat22")
        .property("data", &cgmath::Mat22::data);
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
}

void reflectGlobalTramsform() {
    mirrow::drefl::registrar<GlobalTransform>::instance().regist(
        "GlobalTransform");
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
        .property("z-index", &Sprite::orderInLayer);

    mirrow::drefl::registrar<Flip>::instance()
        .regist("Flip")
        .add("None", Flip::None)
        .add("Horizontal", Flip::Horizontal)
        .add("Vertical", Flip::Vertical)
        .add("Both", Flip::Both);

    mirrow::drefl::registrar<TextureHandle>::instance().regist("TextureHandle");
}

void reflectAnimation() {
    mirrow::drefl::registrar<AnimationPlayer>::instance().regist(
        "AnimationPlayer");
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

    // mirrow::drefl::registrar<ui::Label>::instance()
    //     .regist("Label")
    //     .property("color", &ui::Label::color, {AttrColor})
    //     .property("hoverColor", &ui::Label::hoverColor, {AttrColor})
    //     .property("pressColor", &ui::Label::pressColor, {AttrColor});

}

void reflectWindow() {
    mirrow::drefl::registrar<WindowBuilder::Data>::instance()
        .regist("WindowData")
        .property("title", &WindowBuilder::Data::title)
        .property("size", &WindowBuilder::Data::size);
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

    mirrow::drefl::registrar<Tilesheet>::instance().regist("Tilesheet");
}

void reflectScript() {
    mirrow::drefl::registrar<LuaScript>::instance().regist("LuaScript");
    mirrow::drefl::registrar<Script>::instance()
        .regist("Script")
        .property("handle", &Script::handle);
}

void reflectMisc() {
    mirrow::drefl::registrar<Name>::instance().regist("Name").property(
        "name", &Name::name);

    mirrow::drefl::registrar<gecs::entity>::instance().regist("entity");
}

void serializeTextureHandle(toml::node& node, const mirrow::drefl::any& elem) {
    Assert(node.is_table(), "serialize texture-handle to table");
    auto mgr = ECS::Instance().World().res<TextureManager>();
    auto handle = mirrow::drefl::try_cast_const<TextureHandle>(elem);
    if (mgr->Has(*handle)) {
        auto& texture = mgr->Get(*handle);
        node.as_table()->emplace("path", texture.RelativePath().string());
    }
}

void deserializeTextureHandle(const toml::node& node,
                              mirrow::drefl::any& elem) {
    Assert(node.is_table(), "serialize texture-handle to table");
    auto& tbl = *node.as_table();

    if (auto path = tbl.get("path"); path && path->is_string()) {
        auto filename = path->as_string()->get();
        auto& handle = *mirrow::drefl::try_cast<TextureHandle>(elem);

        auto mgr = ECS::Instance().World().res_mut<TextureManager>();
        if (mgr->Has(filename)) {
            handle = mgr->GetHandle(filename);
        } else {
            // TODO: add format & sampler descriptor here
            handle = mgr->Load(filename);
        }
    }
}

void registTextureHandleSerd() {
    auto& serd = mirrow::serd::drefl::serialize_method_storage::instance();
    serd.regist_serialize(mirrow::drefl::typeinfo<TextureHandle>(),
                          serializeTextureHandle);
    serd.regist_deserialize(mirrow::drefl::typeinfo<TextureHandle>(),
                            deserializeTextureHandle);
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

    if (auto path = tbl.get("path"); path && path->is_string()) {
        auto filename = tbl.get("path")->as_string()->get();
        auto& player = *mirrow::drefl::try_cast<SoundPlayer>(elem);

        auto mgr = ECS::Instance().World().res_mut<AudioManager>();
        if (mgr->Has(filename)) {
            player.ChangeSound(mgr->GetHandle(filename));
        }
    }
}

void registSoundPlayerSerd() {
    auto& serd = mirrow::serd::drefl::serialize_method_storage::instance();
    serd.regist_serialize(mirrow::drefl::typeinfo<SoundPlayer>(),
                          serializeSoundPlayer);
    serd.regist_deserialize(mirrow::drefl::typeinfo<SoundPlayer>(),
                            deserializeSoundPlayer);
}

void serializeAnimationPlayer(toml::node& node,
                              const mirrow::drefl::any& elem) {
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

void deserializeAnimationPlayer(const toml::node& node,
                                mirrow::drefl::any& elem) {
    Assert(node.is_table(), "serialize sound player to table");
    auto& tbl = *node.as_table();

    auto mgr = ECS::Instance().World().res<AnimationManager>();
    if (auto path = tbl.get("path"); path && path->is_string()) {
        auto filename = tbl.get("path")->as_string()->get();
        auto& player = *mirrow::drefl::try_cast<AnimationPlayer>(elem);

        if (mgr->Has(filename)) {
            player.ChangeAnim(mgr->GetHandle(filename));
        }
    }
}

void registAnimationPlayerSerd() {
    auto& serd = mirrow::serd::drefl::serialize_method_storage::instance();
    serd.regist_serialize(mirrow::drefl::typeinfo<AnimationPlayer>(),
                          serializeAnimationPlayer);
    serd.regist_deserialize(mirrow::drefl::typeinfo<AnimationPlayer>(),
                            deserializeAnimationPlayer);
}

void serializeGlobalTransform(toml::node& node,
                              const mirrow::drefl::any& elem) {
    Assert(elem.type_info() == mirrow::drefl::typeinfo<GlobalTransform>(),
           "elem type incorrect");
    Assert(node.is_table(), "serialize GlobalTransform require table");

    node.as_table()->emplace(elem.type_info()->name(), toml::table{});
}

void deserializeGlobalTransform(const toml::node& node,
                                mirrow::drefl::any& elem) {
    Assert(elem.type_info() == mirrow::drefl::typeinfo<GlobalTransform>(),
           "elem type incorrect");
    Assert(node.is_table(), "serialize GlobalTransform require table");

    mirrow::drefl::try_cast<GlobalTransform>(elem)->mat =
        cgmath::Mat44::Identity();
}

void registGlobalTransformSerd() {
    auto& serd = mirrow::serd::drefl::serialize_method_storage::instance();
    serd.regist_serialize(mirrow::drefl::typeinfo<GlobalTransform>(),
                          serializeGlobalTransform);
    serd.regist_deserialize(mirrow::drefl::typeinfo<GlobalTransform>(),
                            deserializeGlobalTransform);
}

void reflectHierarchy() {
    mirrow::drefl::registrar<Parent>::instance()
        .regist("Parent", {EditorNodisplay})
        .property("entity", &Parent::entity);
    mirrow::drefl::registrar<Child>::instance()
        .regist("Child", {EditorNodisplay})
        .property("entities", &Child::entities);
}

void reflectAudio() {
    mirrow::drefl::registrar<SoundPlayer>::instance().regist("SoundPlayer");
}

void reflectRHI() {
    mirrow::drefl::registrar<rhi::SamplerAddressMode>::instance()
        .regist("SamplerAddressMode")
        .add("ClampToEdge", rhi::SamplerAddressMode::ClampToEdge)
        .add("MirrowRepeat", rhi::SamplerAddressMode::MirrorRepeat)
        .add("Repeat", rhi::SamplerAddressMode::Repeat);

    mirrow::drefl::registrar<rhi::Filter>::instance()
        .regist("Filter")
        .add("Linear", rhi::Filter::Linear)
        .add("Nearest", rhi::Filter::Nearest);
}

void reflectTexture() {
    mirrow::drefl::registrar<Texture>::instance().regist("Texture");
}

void reflectMaterial() {
    mirrow::drefl::registrar<Material2D>::instance().regist("Material2D");

    mirrow::drefl::registrar<Material2DHandle>::instance().regist(
        "Material2DHandle");

    mirrow::drefl::registrar<Material3D>::instance().regist("Material3D");

    mirrow::drefl::registrar<SpriteMaterial>::instance()
        .regist("SpriteMaterial")
        .property("material", &SpriteMaterial::material);
}

void reflectGLTFModel() {
    mirrow::drefl::registrar<GLTFModel>::instance().regist("GLTFModel");
}

void RegistReflectInfos() {
    reflectVec2();
    reflectVec3();
    reflectVec4();
    reflectRect();
    reflectMat();
    reflectTramsform();
    reflectGlobalTramsform();
    reflectAnimation();
    reflectSprite();
    reflectUI();
    reflectWindow();
    reflectTilesheet();
    reflectHierarchy();
    reflectMisc();
    reflectAudio();
    reflectScript();
    reflectRHI();
    reflectTexture();
    reflectMaterial();
    reflectGLTFModel();
}

}  // namespace nickel