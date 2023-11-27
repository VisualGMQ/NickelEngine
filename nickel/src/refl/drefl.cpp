#include "refl/drefl.hpp"
#include "mirrow/drefl/factory.hpp"
#include "nickel.hpp"

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
}

void reflectGlobalTramsform() {
    mirrow::drefl::registrar<GlobalTransform>::instance().regist(
        "GlobalTransform");
}

void reflectSprite() {
    mirrow::drefl::registrar<Sprite>::instance()
        .regist("Sprite")
        .property("anchor", &Sprite::anchor)
        .property("color", &Sprite::color)
        .property("region", &Sprite::region)
        .property("customSize", &Sprite::customSize);

    mirrow::drefl::registrar<SpriteBundle>::instance()
        .regist("SpriteBundle")
        .property("flip", &SpriteBundle::flip)
        .property("image", &SpriteBundle::image)
        .property("sprite", &SpriteBundle::sprite);

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

void InitDynamicReflect() {
    reflectVec2();
    reflectVec3();
    reflectVec4();
    reflectRect();
    reflectTramsform();
    reflectGlobalTramsform();
    reflectAnimation();
    reflectSprite();
}

}  // namespace nickel