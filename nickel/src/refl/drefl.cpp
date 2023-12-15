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

    mirrow::drefl::registrar<ui::Label>::instance()
        .regist("Label")
        .property("color", &ui::Label::color, {AttrColor})
        .property("hoverColor", &ui::Label::hoverColor, {AttrColor})
        .property("pressColor", &ui::Label::pressColor, {AttrColor});
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
}

}  // namespace nickel