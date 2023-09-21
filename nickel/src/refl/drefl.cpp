#include "refl/drefl.hpp"
#include "core/cgmath.hpp"
#include "misc/transform.hpp"

namespace nickel {

void reflectVec2() {
    mirrow::drefl::factory<cgmath::Vec2>("Vec2")
        .var<&cgmath::Vec2::x>("x")
        .var<&cgmath::Vec2::y>("y");
}

void reflectVec3() {
    mirrow::drefl::factory<cgmath::Vec3>("Vec3")
        .var<&cgmath::Vec3::x>("x")
        .var<&cgmath::Vec3::y>("y")
        .var<&cgmath::Vec3::z>("z");
}

void reflectVec4() {
    mirrow::drefl::factory<cgmath::Vec4>("Vec4")
        .var<&cgmath::Vec4::x>("x")
        .var<&cgmath::Vec4::y>("y")
        .var<&cgmath::Vec4::w>("w")
        .var<&cgmath::Vec4::z>("z");
}

void reflectTramsform() {
    mirrow::drefl::factory<Transform>("Transform")
        .var<&Transform::translation>("translation")
        .var<&Transform::rotation>("rotation")
        .var<&Transform::scale>("scale");
}

void InitDynamicReflect() {
    reflectVec2();
    reflectVec3();
    reflectVec4();
    reflectTramsform();
}

}