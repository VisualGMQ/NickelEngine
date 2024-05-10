#include "script/luabind.hpp"
#include "nickel.hpp"

#include "lua.h"
#include "luacode.h"
#include "lualib.h"

#include "LuaBridge/Array.h"
#include "LuaBridge/LuaBridge.h"
#include "LuaBridge/Vector.h"

namespace luabridge {

template <>
struct Stack<nickel::Flip>
    : luabridge::Enum<nickel::Flip, nickel::Flip::None, nickel::Flip::Vertical,
                      nickel::Flip::Horizontal, nickel::Flip::Both> {};

template <>
struct Stack<gecs::entity>
    : luabridge::Enum<gecs::entity, gecs::null_entity> {};

template <>
struct Stack<nickel::FileType>
    : luabridge::Enum<nickel::FileType, nickel::FileType::Animation,
                      nickel::FileType::Audio, nickel::FileType::Font,
                      nickel::FileType::Image, nickel::FileType::Meta,
                      nickel::FileType::Script, nickel::FileType::Tilesheet,
                      nickel::FileType::Timer, nickel::FileType::Unknown> {};

template <>
struct Stack<nickel::Key>
    : luabridge::Enum<
          nickel::Key, nickel::Key::A, nickel::Key::B, nickel::Key::C,
          nickel::Key::D, nickel::Key::E, nickel::Key::F, nickel::Key::G,
          nickel::Key::H, nickel::Key::I, nickel::Key::J, nickel::Key::K,
          nickel::Key::L, nickel::Key::M, nickel::Key::N, nickel::Key::O,
          nickel::Key::P, nickel::Key::Q, nickel::Key::R, nickel::Key::S,
          nickel::Key::T, nickel::Key::U, nickel::Key::V, nickel::Key::W,
          nickel::Key::X, nickel::Key::Y, nickel::Key::Z, nickel::Key::Num1,
          nickel::Key::Num2, nickel::Key::Num3, nickel::Key::Num4,
          nickel::Key::Num5, nickel::Key::Num6, nickel::Key::Num7,
          nickel::Key::Num8, nickel::Key::Num9, nickel::Key::Num0,
          nickel::Key::Return, nickel::Key::Escape, nickel::Key::Backspace,
          nickel::Key::Tab, nickel::Key::Space, nickel::Key::Minus,
          nickel::Key::Equals, nickel::Key::Leftbracket,
          nickel::Key::Rightbracket, nickel::Key::Backslash,
          nickel::Key::Nonushash, nickel::Key::Semicolon,
          nickel::Key::Apostrophe, nickel::Key::Grave, nickel::Key::Comma,
          nickel::Key::Period, nickel::Key::Slash, nickel::Key::Capslock,
          nickel::Key::F1, nickel::Key::F2, nickel::Key::F3, nickel::Key::F4,
          nickel::Key::F5, nickel::Key::F6, nickel::Key::F7, nickel::Key::F8,
          nickel::Key::F9, nickel::Key::F10, nickel::Key::F11, nickel::Key::F12,
          nickel::Key::Printscreen, nickel::Key::Scrolllock, nickel::Key::Pause,
          nickel::Key::Insert, nickel::Key::Home, nickel::Key::Pageup,
          nickel::Key::Delete, nickel::Key::End, nickel::Key::Pagedown,
          nickel::Key::Right, nickel::Key::Left, nickel::Key::Down,
          nickel::Key::Up, nickel::Key::Numlockclear, nickel::Key::Kp_divide,
          nickel::Key::Kp_multiply, nickel::Key::Kp_minus, nickel::Key::Kp_plus,
          nickel::Key::Kp_enter, nickel::Key::Kp_1, nickel::Key::Kp_2,
          nickel::Key::Kp_3, nickel::Key::Kp_4, nickel::Key::Kp_5,
          nickel::Key::Kp_6, nickel::Key::Kp_7, nickel::Key::Kp_8,
          nickel::Key::Kp_9, nickel::Key::Kp_0, nickel::Key::Kp_period,
          nickel::Key::Nonusbackslash, nickel::Key::Application,
          nickel::Key::Power, nickel::Key::Kp_equals, nickel::Key::F13,
          nickel::Key::F14, nickel::Key::F15, nickel::Key::F16,
          nickel::Key::F17, nickel::Key::F18, nickel::Key::F19,
          nickel::Key::F20, nickel::Key::F21, nickel::Key::F22,
          nickel::Key::F23, nickel::Key::F24, nickel::Key::Execute,
          nickel::Key::Help, nickel::Key::Menu, nickel::Key::Select,
          nickel::Key::Stop, nickel::Key::Again, nickel::Key::Undo,
          nickel::Key::Cut, nickel::Key::Copy, nickel::Key::Paste,
          nickel::Key::Find, nickel::Key::Mute, nickel::Key::Volumeup,
          nickel::Key::Volumedown, nickel::Key::Kp_comma,
          nickel::Key::Kp_equalsas400, nickel::Key::International1,
          nickel::Key::International2, nickel::Key::International3,
          nickel::Key::International4, nickel::Key::International5,
          nickel::Key::International6, nickel::Key::International7,
          nickel::Key::International8, nickel::Key::International9,
          nickel::Key::Lang1, nickel::Key::Lang2, nickel::Key::Lang3,
          nickel::Key::Lang4, nickel::Key::Lang5, nickel::Key::Lang6,
          nickel::Key::Lang7, nickel::Key::Lang8, nickel::Key::Lang9,
          nickel::Key::Alterase, nickel::Key::Sysreq, nickel::Key::Cancel,
          nickel::Key::Clear, nickel::Key::Prior, nickel::Key::Return2,
          nickel::Key::Separator, nickel::Key::Out, nickel::Key::Oper,
          nickel::Key::Clearagain, nickel::Key::Crsel, nickel::Key::Exsel,
          nickel::Key::Kp_00, nickel::Key::Kp_000,
          nickel::Key::Thousandsseparator, nickel::Key::Decimalseparator,
          nickel::Key::Currencyunit, nickel::Key::Currencysubunit,
          nickel::Key::Kp_leftparen, nickel::Key::Kp_rightparen,
          nickel::Key::Kp_leftbrace, nickel::Key::Kp_rightbrace,
          nickel::Key::Kp_tab, nickel::Key::Kp_backspace, nickel::Key::Kp_a,
          nickel::Key::Kp_b, nickel::Key::Kp_c, nickel::Key::Kp_d,
          nickel::Key::Kp_e, nickel::Key::Kp_f, nickel::Key::Kp_xor,
          nickel::Key::Kp_power, nickel::Key::Kp_percent, nickel::Key::Kp_less,
          nickel::Key::Kp_greater, nickel::Key::Kp_ampersand,
          nickel::Key::Kp_dblampersand, nickel::Key::Kp_verticalbar,
          nickel::Key::Kp_dblverticalbar, nickel::Key::Kp_colon,
          nickel::Key::Kp_hash, nickel::Key::Kp_space, nickel::Key::Kp_at,
          nickel::Key::Kp_exclam, nickel::Key::Kp_memstore,
          nickel::Key::Kp_memrecall, nickel::Key::Kp_memclear,
          nickel::Key::Kp_memadd, nickel::Key::Kp_memsubtract,
          nickel::Key::Kp_memmultiply, nickel::Key::Kp_memdivide,
          nickel::Key::Kp_plusminus, nickel::Key::Kp_clear,
          nickel::Key::Kp_clearentry, nickel::Key::Kp_binary,
          nickel::Key::Kp_octal, nickel::Key::Kp_decimal,
          nickel::Key::Kp_hexadecimal, nickel::Key::Lctrl, nickel::Key::Lshift,
          nickel::Key::Lalt, nickel::Key::Lgui, nickel::Key::Rctrl,
          nickel::Key::Rshift, nickel::Key::Ralt, nickel::Key::Rgui,
          nickel::Key::Mode, nickel::Key::Audionext, nickel::Key::Audioprev,
          nickel::Key::Audiostop, nickel::Key::Audioplay,
          nickel::Key::Audiomute, nickel::Key::Mediaselect, nickel::Key::Www,
          nickel::Key::Mail, nickel::Key::Calculator, nickel::Key::Computer,
          nickel::Key::Ac_search, nickel::Key::Ac_home, nickel::Key::Ac_back,
          nickel::Key::Ac_forward, nickel::Key::Ac_stop,
          nickel::Key::Ac_refresh, nickel::Key::Ac_bookmarks,
          nickel::Key::Brightnessdown, nickel::Key::Brightnessup,
          nickel::Key::Displayswitch, nickel::Key::Kbdillumtoggle,
          nickel::Key::Eject, nickel::Key::Sleep, nickel::Key::App1,
          nickel::Key::App2, nickel::Key::Audiorewind,
          nickel::Key::Audiofastforward> {};

}  // namespace luabridge

namespace nickel {

// clang-format off

void bindCommon(luabridge::Namespace& scope) {
    scope = scope.beginNamespace("FileType")
        .addProperty("Audio", +[](){ return FileType::Audio; })
        .addProperty("Animation", +[](){ return FileType::Animation; })
        .addProperty("Font", +[](){ return FileType::Font; })
        .addProperty("Image", +[](){ return FileType::Image; })
        .addProperty("Meta", +[](){ return FileType::Meta; })
        .addProperty("Script", +[](){ return FileType::Script; })
        .addProperty("Tilesheet", +[](){ return FileType::Tilesheet; })
        .addProperty("Timer", +[](){ return FileType::Timer; })
    .endNamespace();
}

void bindMath(luabridge::Namespace& scope) {
    // scope = scope.beginNamespace("cgmath")
    //     .beginClass<cgmath::Vec2>("Vec2")
    //         .addConstructor<void(float, float), void(void)>()
    //         .addProperty("x", &cgmath::Vec2::x)
    //         .addProperty("y", &cgmath::Vec2::y)
    //         .addProperty("w", &cgmath::Vec2::w)
    //         .addProperty("h", &cgmath::Vec2::h)
    //         .addFunction("Dot", &cgmath::Vec2::Dot) 
    //         .addFunction("__add", &cgmath::operator+<float, 2>)
    //         .addFunction("__sub", &cgmath::operator-<float, 2>)
    //         .addFunction("__mul", &cgmath::operator*<float, 2>)
    //         .addFunction("__div", &cgmath::operator/<float, 2>)
    //         .addFunction("__eq", &cgmath::operator==<float, 2>)
    //         .addFunction("__tostring", [](const cgmath::Vec2& v) {
    //             return "vec2[ " + std::to_string(v.x) + ", " + std::to_string(v.y) + "]";
    //         })
    //         .addFunction("Set", &cgmath::Vec2::Set)
    //     .endClass()
    //     .beginClass<cgmath::Vec3>("Vec3")
    //         .addConstructor<void(float, float, float), void(void)>()
    //         .addProperty("x", &cgmath::Vec3::x)
    //         .addProperty("y", &cgmath::Vec3::y)
    //         .addProperty("z", &cgmath::Vec3::z)
    //         .addProperty("s", &cgmath::Vec3::s)
    //         .addProperty("r", &cgmath::Vec3::r)
    //         .addProperty("t", &cgmath::Vec3::t)
    //         .addFunction("Dot", &cgmath::Vec3::Dot) 
    //         .addFunction("__add", &cgmath::operator+<float, 3>)
    //         .addFunction("__sub", &cgmath::operator-<float, 3>)
    //         .addFunction("__mul", &cgmath::operator*<float, 3>)
    //         .addFunction("__div", &cgmath::operator/<float, 3>)
    //         .addFunction("__eq", &cgmath::operator==<float, 3>)
    //         .addFunction("__tostring", [](const cgmath::Vec3& v) {
    //             return "vec3[ " + std::to_string(v.x) + ", " + std::to_string(v.y) + ", " + std::to_string(v.z) + "]";
    //         })
    //         .addFunction("Set", &cgmath::Vec3::Set)
    //     .endClass()
    //     .beginClass<cgmath::Vec4>("Vec4")
    //         .addConstructor<void(float, float, float, float), void(void)>()
    //         .addProperty("x", &cgmath::Vec4::x)
    //         .addProperty("y", &cgmath::Vec4::y)
    //         .addProperty("z", &cgmath::Vec4::z)
    //         .addProperty("w", &cgmath::Vec4::w)
    //         .addProperty("r", &cgmath::Vec4::r)
    //         .addProperty("g", &cgmath::Vec4::g)
    //         .addProperty("b", &cgmath::Vec4::b)
    //         .addProperty("a", &cgmath::Vec4::a)
    //         .addFunction("Dot", &cgmath::Vec4::Dot) 
    //         .addFunction("__add", &cgmath::operator+<float, 4>)
    //         .addFunction("__sub", &cgmath::operator-<float, 4>)
    //         .addFunction("__mul", &cgmath::operator*<float, 4>)
    //         .addFunction("__div", &cgmath::operator/<float, 4>)
    //         .addFunction("__eq", &cgmath::operator==<float, 4>)
    //         .addFunction("__tostring", [](const cgmath::Vec4& v) {
    //             return "vec4[ " + std::to_string(v.x) + ", " + std::to_string(v.y) + ", " + std::to_string(v.z) + ", " + std::to_string(v.w) + "]";
    //         })
    //         .addFunction("Set", &cgmath::Vec4::Set)
    //     .endClass()
    //     .addFunction("Dot", &cgmath::Dot<float, 2>, &cgmath::Dot<float, 3>, &cgmath::Dot<float, 4>)
    //     .addFunction("Cross",
    //         luabridge::overload<const cgmath::Vec2&, const cgmath::Vec2&>(&cgmath::Cross<float>),
    //         luabridge::overload<const cgmath::Vec3&, const cgmath::Vec3&>(&cgmath::Cross<float>))
    //     .addFunction("TripleCross", &cgmath::TripleCross<float>)
    //     .addFunction("Length", &cgmath::Length<float, 2>, &cgmath::Length<float, 3>, &cgmath::Length<float, 4>)
    //     .addFunction("LengthSqrd", &cgmath::LengthSqrd<float, 2>, &cgmath::LengthSqrd<float, 3>, &cgmath::LengthSqrd<float, 4>)
    //     .addFunction("Project", &cgmath::Project<float, 2>, &cgmath::Project<float, 3>, &cgmath::Project<float, 4>)
    //     .addFunction("Normalize", &cgmath::Normalize<float, 2>, &cgmath::Normalize<float, 3>, &cgmath::Normalize<float, 4>)
    //     .addFunction("Rad2Deg", &cgmath::Rad2Deg<float>, &cgmath::Rad2Deg<double>)
    //     .addFunction("Deg2Rad", &cgmath::Deg2Rad<float>, &cgmath::Deg2Rad<double>)
    //     .beginClass<cgmath::Mat22>("Mat22")
    //         .addConstructor<void(void)>()
    //         .addFunction("Set", &cgmath::Mat22::Set)
    //         .addFunction("W", &cgmath::Mat22::W)
    //         .addFunction("H", &cgmath::Mat22::H)
    //         .addFunction("Get",
    //             luabridge::nonConstOverload<int, int>(&cgmath::Mat22::Get),
    //             luabridge::constOverload<int, int>(&cgmath::Mat22::Get))
    //         .addStaticFunction("Identity", &cgmath::Mat22::Identity)
    //         .addStaticFunction("Ones", &cgmath::Mat22::Ones)
    //         .addStaticFunction("Zeros", &cgmath::Mat22::Zeros)
    //         .addStaticFunction("FromRow",
    //             [](float m00, float m10,
    //                float m01, float m11) {
    //                 return cgmath::Mat22::FromRow(
    //                     {m00, m10, 
    //                     m01, m11, });
    //         })
    //         .addFunction("__mul", &cgmath::MulEach<float, 2, 2>)
    //         .addFunction("__div", &cgmath::DivEach<float, 2, 2>)
    //         .addFunction("__add", &cgmath::operator+<float, 2, 2>)
    //         .addFunction("__sub", &cgmath::operator-<float, 2, 2>)
    //         .addFunction("Transpose", &cgmath::Transpose<float, 2, 2>)
    //     .endClass()
    //     .beginClass<cgmath::Mat33>("Mat33")
    //         .addConstructor<void(void)>()
    //         .addFunction("Set", &cgmath::Mat33::Set)
    //         .addFunction("W", &cgmath::Mat33::W)
    //         .addFunction("H", &cgmath::Mat33::H)
    //         .addFunction("Get",
    //             luabridge::nonConstOverload<int, int>(&cgmath::Mat33::Get),
    //             luabridge::constOverload<int, int>(&cgmath::Mat33::Get))
    //         .addStaticFunction("Identity", &cgmath::Mat33::Identity)
    //         .addStaticFunction("Ones", &cgmath::Mat33::Ones)
    //         .addStaticFunction("Zeros", &cgmath::Mat33::Zeros)
    //         .addStaticFunction("FromRow",
    //             [](float m00, float m10, float m20,
    //                float m01, float m11, float m21,
    //                float m02, float m12, float m22) {
    //                 return cgmath::Mat33::FromRow(
    //                     {m00, m10, m20,
    //                     m01, m11, m21,
    //                     m02, m12, m22});
    //         })
    //         .addFunction("__mul", &cgmath::MulEach<float, 3, 3>)
    //         .addFunction("__div", &cgmath::DivEach<float, 3, 3>)
    //         .addFunction("__add", &cgmath::operator+<float, 3, 3>)
    //         .addFunction("__sub", &cgmath::operator-<float, 3, 3>)
    //         .addFunction("Transpose", &cgmath::Transpose<float, 3, 3>)
    //     .endClass()
    //     .beginClass<cgmath::Mat44>("Mat44")
    //         .addConstructor<void(void)>()
    //         .addFunction("Set", &cgmath::Mat44::Set)
    //         .addFunction("W", &cgmath::Mat44::W)
    //         .addFunction("H", &cgmath::Mat44::H)
    //         .addFunction("Get",
    //             luabridge::nonConstOverload<int, int>(&cgmath::Mat44::Get),
    //             luabridge::constOverload<int, int>(&cgmath::Mat44::Get))
    //         .addStaticFunction("Identity", &cgmath::Mat44::Identity)
    //         .addStaticFunction("Ones", &cgmath::Mat44::Ones)
    //         .addStaticFunction("Zeros", &cgmath::Mat44::Zeros)
    //         .addStaticFunction("FromRow",
    //             [](float m00, float m10, float m20, float m30,
    //                float m01, float m11, float m21, float m31,
    //                float m02, float m12, float m22, float m32,
    //                float m03, float m13, float m23, float m33) {
    //                 return cgmath::Mat44::FromRow(
    //                     {m00, m10, m20, m30,
    //                     m01, m11, m21, m31,
    //                     m02, m12, m22, m32,
    //                     m03, m13, m23, m33});
    //         })
    //         .addFunction("__mul", &cgmath::MulEach<float, 4, 4>)
    //         .addFunction("__div", &cgmath::DivEach<float, 4, 4>)
    //         .addFunction("__add", &cgmath::operator+<float, 4, 4>)
    //         .addFunction("__sub", &cgmath::operator-<float, 4, 4>)
    //         .addFunction("Transpose", &cgmath::Transpose<float, 4, 4>)
    //     .endClass()
    //     .addFunction("Mul",
    //         luabridge::overload<const cgmath::Vec2&, float>(&cgmath::operator*<float, float, 2>),
    //         luabridge::overload<float, const cgmath::Vec2&>(&cgmath::operator*<float, float, 2>),
    //         luabridge::overload<const cgmath::Vec3&, float>(&cgmath::operator*<float, float, 3>),
    //         luabridge::overload<float, const cgmath::Vec3&>(&cgmath::operator*<float, float, 3>),
    //         luabridge::overload<const cgmath::Vec4&, float>(&cgmath::operator*<float, float, 4>),
    //         luabridge::overload<float, const cgmath::Vec4&>(&cgmath::operator*<float, float, 4>),
    //         luabridge::overload<const cgmath::Mat22&, float>(&cgmath::operator*<float, float, 2, 2>),
    //         luabridge::overload<float, const cgmath::Mat22&>(&cgmath::operator*<float, float, 2, 2>),
    //         luabridge::overload<const cgmath::Mat22&, const cgmath::Mat22&>(&cgmath::operator*<float, 2, 2, 2>),
    //         luabridge::overload<const cgmath::Mat22&, const cgmath::Vec2&>(&cgmath::operator*<float, 2, 2>),
    //         luabridge::overload<const cgmath::Mat33&, float>(&cgmath::operator*<float, float, 3, 3>),
    //         luabridge::overload<float, const cgmath::Mat33&>(&cgmath::operator*<float, float, 3, 3>),
    //         luabridge::overload<const cgmath::Mat33&, const cgmath::Mat33&>(&cgmath::operator*<float, 3, 3, 3>),
    //         luabridge::overload<const cgmath::Mat33&, const cgmath::Vec3&>(&cgmath::operator*<float, 3, 3>),
    //         luabridge::overload<const cgmath::Mat44&, float>(&cgmath::operator*<float, float, 4, 4>),
    //         luabridge::overload<float, const cgmath::Mat44&>(&cgmath::operator*<float, float, 4, 4>),
    //         luabridge::overload<const cgmath::Mat44&, const cgmath::Mat44&>(&cgmath::operator*<float, 4, 4, 4>),
    //         luabridge::overload<const cgmath::Mat44&, const cgmath::Vec4&>(&cgmath::operator*<float, 4, 4>))
    //     .addFunction("Div",
    //         luabridge::overload<const cgmath::Mat22&, float>(&cgmath::operator/<float, float, 2, 2>),
    //         luabridge::overload<const cgmath::Mat33&, float>(&cgmath::operator/<float, float, 3, 3>),
    //         luabridge::overload<const cgmath::Mat44&, float>(&cgmath::operator/<float, float, 4, 4>),
    //         luabridge::overload<const cgmath::Vec2&, float>(&cgmath::operator/<float, float, 2>),
    //         luabridge::overload<const cgmath::Vec3&, float>(&cgmath::operator/<float, float, 3>),
    //         luabridge::overload<const cgmath::Vec4&, float>(&cgmath::operator/<float, float, 4>))
    //     .addFunction("CreateTranslation", &cgmath::CreateTranslation)
    //     .addFunction("CreateScale", &cgmath::CreateScale)
    //     .addFunction("CreateXRotation", &cgmath::CreateXRotation)
    //     .addFunction("CreateYRotation", &cgmath::CreateYRotation)
    //     .addFunction("CreateZRotation", &cgmath::CreateZRotation)
    //     .addFunction("CreateXYZRotation", &cgmath::CreateXYZRotation)
    //     .addFunction("CreateRotation2D", &cgmath::CreateRotation2D)
    //     .addFunction("Sign", &cgmath::Sign<float>)
    //     .addFunction("IsOverlap", &cgmath::IsOverlap<float>)
    //     .addFunction("Wrap", &cgmath::Wrap<float>)
    //     .addFunction("GetRadianIn2PI", &cgmath::GetRadianIn2PI<float>)
    //     .addFunction("GetRadianInPISigned", &cgmath::GetRadianInPISigned<float>)
    //     .beginClass<cgmath::Rect>("Rect")
    //         .addProperty("position", &cgmath::Rect::position)
    //         .addProperty("size", &cgmath::Rect::size)
    //         .addConstructor<void(const cgmath::Vec2&, const cgmath::Vec2&), void(float, float, float, float)>()
    //         .addStaticFunction("FromCenter", cgmath::Rect::FromCenter)
    //         .addFunction("IsPtIn", &cgmath::Rect::IsPtIn)
    //         .addFunction("Intrersect", &cgmath::Rect::Intersect)
    //         .addFunction("IsIntrersect", &cgmath::Rect::IsIntersect)
    //     .endClass()
    // .endNamespace();
}

void bindTransform(luabridge::Namespace& scope) {
    scope = scope.beginClass<Transform>("Transform")
        .addConstructor<void(void)>()
        .addProperty("translation", &Transform::translation)
        .addProperty("rotation", &Transform::rotation)
        .addProperty("scale", &Transform::scale)
        .addStaticFunction("FromRotation", &Transform::FromRotation)
        .addStaticFunction("FromScale", &Transform::FromScale)
        .addStaticFunction("FromTranslation", &Transform::FromTranslation)
        .addFunction("ToMat", &Transform::ToMat)
    .endClass();
}

template <typename HandleT>
void bindHandle(luabridge::Namespace& scope, const std::string& name) {
    scope = scope.beginClass<HandleT>(name.c_str())
        .addFunction("__tostring", [=](HandleT handle) -> std::string {
            if (handle) {
                return name + "(" + std::to_string((HandleInnerIDType)(handle)) + ")";
            } else {
                return name + "(Null)";
            }
        })
    .endClass();
}

void bindHandles(luabridge::Namespace& scope) {
    bindHandle<TextureHandle>(scope, "TextureHandle");
    bindHandle<AnimationHandle>(scope, "AnimationHandle");
    bindHandle<FontHandle>(scope, "FontHandle");
    bindHandle<SoundHandle>(scope, "SoundHandle");
    bindHandle<ScriptHandle>(scope, "ScriptHandle");
    bindHandle<TilesheetHandle>(scope, "TilesheetHandle");
}

void bindGraphics(luabridge::Namespace& scope) {
    scope = scope
    .beginNamespace("Flip")
        .addProperty("None", +[]() { return Flip::None; })
        .addProperty("Vertical", +[]() { return Flip::Vertical; })
        .addProperty("Horizontal", +[](){ return Flip::Horizontal; })
        .addProperty("Both", +[](){ return Flip::Both; })
    .endNamespace()
    .beginClass<Sprite>("Sprite")
        .addConstructor<void(void)>()
        .addProperty("anchor", &Sprite::anchor)
        .addProperty("color", &Sprite::color)
        .addProperty("customSize", &Sprite::customSize)
        .addProperty("region", &Sprite::region)
        .addProperty("flip", &Sprite::flip)
        .addProperty("visible", &Sprite::visiable)
        .addProperty("orderInLayer", &Sprite::orderInLayer)
    .endClass()
    .beginClass<SpriteBundle>("SpriteBundle")
        .addConstructor<void(void)>()
        .addProperty("sprite", &SpriteBundle::sprite)
        .addProperty("transform", &SpriteBundle::transform)
    .endClass()
    .beginClass<Texture>("Texture")
        .addFunction("Width", &Texture::Width)
        .addFunction("Height", &Texture::Height)
        .addFunction("Size", &Texture::Size)
        .addFunction("IsValid", &Texture::operator bool)
    .endClass()
    .beginClass<TextureManager>("TextureManager")
        .addStaticFunction("GetFileType", &TextureManager::GetFileType)
        .addFunction("Load", &TextureManager::Load)
        .addFunction("Destroy",
            luabridge::overload<TextureHandle>(&TextureManager::Destroy),
            luabridge::overload<const std::filesystem::path&>(&TextureManager::Destroy))
        .addFunction("GetHandle", &TextureManager::GetHandle)
        .addFunction("Has",
            luabridge::overload<TextureHandle>(&TextureManager::Has),
            luabridge::overload<const std::filesystem::path&>(&TextureManager::Has))
        .addFunction("Get",
            luabridge::nonConstOverload<TextureHandle>(&TextureManager::Get),
            luabridge::constOverload<TextureHandle>(&TextureManager::Get),
            luabridge::overload<const std::filesystem::path&>(&TextureManager::Get))
    .endClass()
    .beginClass<Font>("Font")
        .addFunction("IsValid", &Font::operator bool)
    .endClass()
    .beginClass<FontManager>("FontManager")
        .addStaticFunction("GetFileType", &FontManager::GetFileType)
        .addFunction("Load", &FontManager::Load)
        .addFunction("Destroy",
            luabridge::overload<FontHandle>(&FontManager::Destroy),
            luabridge::overload<const std::filesystem::path&>(&FontManager::Destroy))
        .addFunction("GetHandle", &FontManager::GetHandle)
        .addFunction("Has",
            luabridge::overload<FontHandle>(&FontManager::Has),
            luabridge::overload<const std::filesystem::path&>(&FontManager::Has))
        .addFunction("Get",
            luabridge::nonConstOverload<FontHandle>(&FontManager::Get),
            luabridge::constOverload<FontHandle>(&FontManager::Get),
            luabridge::overload<const std::filesystem::path&>(&FontManager::Get))
    .endClass()
    .beginClass<Margin>("Margin")
        .addConstructor<void(void)>()
        .addProperty("left", &Margin::left)
        .addProperty("right", &Margin::right)
        .addProperty("top", &Margin::top)
        .addProperty("bottom", &Margin::bottom)
        .addStaticFunction("Zero", &Margin::Zero)
    .endClass()
    .beginClass<Spacing>("Spacing")
        .addConstructor<void(void)>()
        .addProperty("x", &Spacing::x)
        .addProperty("y", &Spacing::y)
    .endClass()
    .beginClass<Tile>("Tile")
        .addProperty("region", &Tile::region)
        .addProperty("handle", &Tile::handle)
    .endClass()
    .beginClass<Tilesheet>("Tilesheet")
        .addFunction("IsValid", &Tilesheet::operator bool)
        .addFunction("Row", &Tilesheet::Row)
        .addFunction("Col", &Tilesheet::Col)
        .addFunction("Get",
            luabridge::overload<uint32_t, uint32_t>(&Tilesheet::Get),
            luabridge::overload<uint32_t>(&Tilesheet::Get))
        .addFunction("Handle", &Tilesheet::Handle)
        .addFunction("TileSize", &Tilesheet::TileSize)
    .endClass();
}

void bindInput(luabridge::Namespace& scope) {
    scope = 
    scope
    .beginNamespace("Key")
        .addProperty("A", +[](){ return nickel::Key::A; })
        .addProperty("B", +[](){ return nickel::Key::B; })
        .addProperty("C", +[](){ return nickel::Key::C; })
        .addProperty("D", +[](){ return nickel::Key::D; })
        .addProperty("E", +[](){ return nickel::Key::E; })
        .addProperty("F", +[](){ return nickel::Key::F; })
        .addProperty("G", +[](){ return nickel::Key::G; })
        .addProperty("H", +[](){ return nickel::Key::H; })
        .addProperty("I", +[](){ return nickel::Key::I; })
        .addProperty("J", +[](){ return nickel::Key::J; })
        .addProperty("K", +[](){ return nickel::Key::K; })
        .addProperty("L", +[](){ return nickel::Key::L; })
        .addProperty("M", +[](){ return nickel::Key::M; })
        .addProperty("N", +[](){ return nickel::Key::N; })
        .addProperty("O", +[](){ return nickel::Key::O; })
        .addProperty("P", +[](){ return nickel::Key::P; })
        .addProperty("Q", +[](){ return nickel::Key::Q; })
        .addProperty("R", +[](){ return nickel::Key::R; })
        .addProperty("S", +[](){ return nickel::Key::S; })
        .addProperty("T", +[](){ return nickel::Key::T; })
        .addProperty("U", +[](){ return nickel::Key::U; })
        .addProperty("V", +[](){ return nickel::Key::V; })
        .addProperty("W", +[](){ return nickel::Key::W; })
        .addProperty("X", +[](){ return nickel::Key::X; })
        .addProperty("Y", +[](){ return nickel::Key::Y; })
        .addProperty("Z", +[](){ return nickel::Key::Z; })
        .addProperty("Num1", +[](){ return nickel::Key::Num1; })
        .addProperty("Num2", +[](){ return nickel::Key::Num2; })
        .addProperty("Num3", +[](){ return nickel::Key::Num3; })
        .addProperty("Num4", +[](){ return nickel::Key::Num4; })
        .addProperty("Num5", +[](){ return nickel::Key::Num5; })
        .addProperty("Num6", +[](){ return nickel::Key::Num6; })
        .addProperty("Num7", +[](){ return nickel::Key::Num7; })
        .addProperty("Num8", +[](){ return nickel::Key::Num8; })
        .addProperty("Num9", +[](){ return nickel::Key::Num9; })
        .addProperty("Num0", +[](){ return nickel::Key::Num0; })
        .addProperty("Return", +[](){ return nickel::Key::Return; })
        .addProperty("Escape", +[](){ return nickel::Key::Escape; })
        .addProperty("Backspace", +[](){ return nickel::Key::Backspace; })
        .addProperty("Tab", +[](){ return nickel::Key::Tab; })
        .addProperty("Space", +[](){ return nickel::Key::Space; })
        .addProperty("Minus", +[](){ return nickel::Key::Minus; })
        .addProperty("Equals", +[](){ return nickel::Key::Equals; })
        .addProperty("Leftbracket", +[](){ return nickel::Key::Leftbracket; })
        .addProperty("Rightbracket", +[](){ return nickel::Key::Rightbracket; })
        .addProperty("Backslash", +[](){ return nickel::Key::Backslash; })
        .addProperty("Nonushash", +[](){ return nickel::Key::Nonushash; })
        .addProperty("Semicolon", +[](){ return nickel::Key::Semicolon; })
        .addProperty("Apostrophe", +[](){ return nickel::Key::Apostrophe; })
        .addProperty("Grave", +[](){ return nickel::Key::Grave; })
        .addProperty("Comma", +[](){ return nickel::Key::Comma; })
        .addProperty("Period", +[](){ return nickel::Key::Period; })
        .addProperty("Slash", +[](){ return nickel::Key::Slash; })
        .addProperty("Capslock", +[](){ return nickel::Key::Capslock; })
        .addProperty("F1", +[](){ return nickel::Key::F1; })
        .addProperty("F2", +[](){ return nickel::Key::F2; })
        .addProperty("F3", +[](){ return nickel::Key::F3; })
        .addProperty("F4", +[](){ return nickel::Key::F4; })
        .addProperty("F5", +[](){ return nickel::Key::F5; })
        .addProperty("F6", +[](){ return nickel::Key::F6; })
        .addProperty("F7", +[](){ return nickel::Key::F7; })
        .addProperty("F8", +[](){ return nickel::Key::F8; })
        .addProperty("F9", +[](){ return nickel::Key::F9; })
        .addProperty("F10", +[](){ return nickel::Key::F10; })
        .addProperty("F11", +[](){ return nickel::Key::F11; })
        .addProperty("F12", +[](){ return nickel::Key::F12; })
        .addProperty("Printscreen", +[](){ return nickel::Key::Printscreen; })
        .addProperty("Scrolllock", +[](){ return nickel::Key::Scrolllock; })
        .addProperty("Pause", +[](){ return nickel::Key::Pause; })
        .addProperty("Insert", +[](){ return nickel::Key::Insert; })
        .addProperty("Home", +[](){ return nickel::Key::Home; })
        .addProperty("Pageup", +[](){ return nickel::Key::Pageup; })
        .addProperty("Delete", +[](){ return nickel::Key::Delete; })
        .addProperty("End", +[](){ return nickel::Key::End; })
        .addProperty("Pagedown", +[](){ return nickel::Key::Pagedown; })
        .addProperty("Right", +[](){ return nickel::Key::Right; })
        .addProperty("Left", +[](){ return nickel::Key::Left; })
        .addProperty("Down", +[](){ return nickel::Key::Down; })
        .addProperty("Up", +[](){ return nickel::Key::Up; })
        .addProperty("Numlockclear", +[](){ return nickel::Key::Numlockclear; })
        .addProperty("Kp_divide", +[](){ return nickel::Key::Kp_divide; })
        .addProperty("Kp_multiply", +[](){ return nickel::Key::Kp_multiply; })
        .addProperty("Kp_minus", +[](){ return nickel::Key::Kp_minus; })
        .addProperty("Kp_plus", +[](){ return nickel::Key::Kp_plus; })
        .addProperty("Kp_enter", +[](){ return nickel::Key::Kp_enter; })
        .addProperty("Kp_1", +[](){ return nickel::Key::Kp_1; })
        .addProperty("Kp_2", +[](){ return nickel::Key::Kp_2; })
        .addProperty("Kp_3", +[](){ return nickel::Key::Kp_3; })
        .addProperty("Kp_4", +[](){ return nickel::Key::Kp_4; })
        .addProperty("Kp_5", +[](){ return nickel::Key::Kp_5; })
        .addProperty("Kp_6", +[](){ return nickel::Key::Kp_6; })
        .addProperty("Kp_7", +[](){ return nickel::Key::Kp_7; })
        .addProperty("Kp_8", +[](){ return nickel::Key::Kp_8; })
        .addProperty("Kp_9", +[](){ return nickel::Key::Kp_9; })
        .addProperty("Kp_0", +[](){ return nickel::Key::Kp_0; })
        .addProperty("Kp_period", +[](){ return nickel::Key::Kp_period; })
        .addProperty("Nonusbackslash", +[](){ return nickel::Key::Nonusbackslash; })
        .addProperty("Application", +[](){ return nickel::Key::Application; })
        .addProperty("Power", +[](){ return nickel::Key::Power; })
        .addProperty("Kp_equals", +[](){ return nickel::Key::Kp_equals; })
        .addProperty("F13", +[](){ return nickel::Key::F13; })
        .addProperty("F14", +[](){ return nickel::Key::F14; })
        .addProperty("F15", +[](){ return nickel::Key::F15; })
        .addProperty("F16", +[](){ return nickel::Key::F16; })
        .addProperty("F17", +[](){ return nickel::Key::F17; })
        .addProperty("F18", +[](){ return nickel::Key::F18; })
        .addProperty("F19", +[](){ return nickel::Key::F19; })
        .addProperty("F20", +[](){ return nickel::Key::F20; })
        .addProperty("F21", +[](){ return nickel::Key::F21; })
        .addProperty("F22", +[](){ return nickel::Key::F22; })
        .addProperty("F23", +[](){ return nickel::Key::F23; })
        .addProperty("F24", +[](){ return nickel::Key::F24; })
        .addProperty("Execute", +[](){ return nickel::Key::Execute; })
        .addProperty("Help", +[](){ return nickel::Key::Help; })
        .addProperty("Menu", +[](){ return nickel::Key::Menu; })
        .addProperty("Select", +[](){ return nickel::Key::Select; })
        .addProperty("Stop", +[](){ return nickel::Key::Stop; })
        .addProperty("Again", +[](){ return nickel::Key::Again; })
        .addProperty("Undo", +[](){ return nickel::Key::Undo; })
        .addProperty("Cut", +[](){ return nickel::Key::Cut; })
        .addProperty("Copy", +[](){ return nickel::Key::Copy; })
        .addProperty("Paste", +[](){ return nickel::Key::Paste; })
        .addProperty("Find", +[](){ return nickel::Key::Find; })
        .addProperty("Mute", +[](){ return nickel::Key::Mute; })
        .addProperty("Volumeup", +[](){ return nickel::Key::Volumeup; })
        .addProperty("Volumedown", +[](){ return nickel::Key::Volumedown; })
        .addProperty("Kp_comma", +[](){ return nickel::Key::Kp_comma; })
        .addProperty("Kp_equalsas400", +[](){ return nickel::Key::Kp_equalsas400; })
        .addProperty("International1", +[](){ return nickel::Key::International1; })
        .addProperty("International2", +[](){ return nickel::Key::International2; })
        .addProperty("International3", +[](){ return nickel::Key::International3; })
        .addProperty("International4", +[](){ return nickel::Key::International4; })
        .addProperty("International5", +[](){ return nickel::Key::International5; })
        .addProperty("International6", +[](){ return nickel::Key::International6; })
        .addProperty("International7", +[](){ return nickel::Key::International7; })
        .addProperty("International8", +[](){ return nickel::Key::International8; })
        .addProperty("International9", +[](){ return nickel::Key::International9; })
        .addProperty("Lang1", +[](){ return nickel::Key::Lang1; })
        .addProperty("Lang2", +[](){ return nickel::Key::Lang2; })
        .addProperty("Lang3", +[](){ return nickel::Key::Lang3; })
        .addProperty("Lang4", +[](){ return nickel::Key::Lang4; })
        .addProperty("Lang5", +[](){ return nickel::Key::Lang5; })
        .addProperty("Lang6", +[](){ return nickel::Key::Lang6; })
        .addProperty("Lang7", +[](){ return nickel::Key::Lang7; })
        .addProperty("Lang8", +[](){ return nickel::Key::Lang8; })
        .addProperty("Lang9", +[](){ return nickel::Key::Lang9; })
        .addProperty("Alterase", +[](){ return nickel::Key::Alterase; })
        .addProperty("Sysreq", +[](){ return nickel::Key::Sysreq; })
        .addProperty("Cancel", +[](){ return nickel::Key::Cancel; })
        .addProperty("Clear", +[](){ return nickel::Key::Clear; })
        .addProperty("Prior", +[](){ return nickel::Key::Prior; })
        .addProperty("Return2", +[](){ return nickel::Key::Return2; })
        .addProperty("Separator", +[](){ return nickel::Key::Separator; })
        .addProperty("Out", +[](){ return nickel::Key::Out; })
        .addProperty("Oper", +[](){ return nickel::Key::Oper; })
        .addProperty("Clearagain", +[](){ return nickel::Key::Clearagain; })
        .addProperty("Crsel", +[](){ return nickel::Key::Crsel; })
        .addProperty("Exsel", +[](){ return nickel::Key::Exsel; })
        .addProperty("Kp_00", +[](){ return nickel::Key::Kp_00; })
        .addProperty("Kp_000", +[](){ return nickel::Key::Kp_000; })
        .addProperty("Thousandsseparator", +[](){ return nickel::Key::Thousandsseparator; })
        .addProperty("Decimalseparator", +[](){ return nickel::Key::Decimalseparator; })
        .addProperty("Currencyunit", +[](){ return nickel::Key::Currencyunit; })
        .addProperty("Currencysubunit", +[](){ return nickel::Key::Currencysubunit; })
        .addProperty("Kp_leftparen", +[](){ return nickel::Key::Kp_leftparen; })
        .addProperty("Kp_rightparen", +[](){ return nickel::Key::Kp_rightparen; })
        .addProperty("Kp_leftbrace", +[](){ return nickel::Key::Kp_leftbrace; })
        .addProperty("Kp_rightbrace", +[](){ return nickel::Key::Kp_rightbrace; })
        .addProperty("Kp_tab", +[](){ return nickel::Key::Kp_tab; })
        .addProperty("Kp_backspace", +[](){ return nickel::Key::Kp_backspace; })
        .addProperty("Kp_a", +[](){ return nickel::Key::Kp_a; })
        .addProperty("Kp_b", +[](){ return nickel::Key::Kp_b; })
        .addProperty("Kp_c", +[](){ return nickel::Key::Kp_c; })
        .addProperty("Kp_d", +[](){ return nickel::Key::Kp_d; })
        .addProperty("Kp_e", +[](){ return nickel::Key::Kp_e; })
        .addProperty("Kp_f", +[](){ return nickel::Key::Kp_f; })
        .addProperty("Kp_xor", +[](){ return nickel::Key::Kp_xor; })
        .addProperty("Kp_power", +[](){ return nickel::Key::Kp_power; })
        .addProperty("Kp_percent", +[](){ return nickel::Key::Kp_percent; })
        .addProperty("Kp_less", +[](){ return nickel::Key::Kp_less; })
        .addProperty("Kp_greater", +[](){ return nickel::Key::Kp_greater; })
        .addProperty("Kp_ampersand", +[](){ return nickel::Key::Kp_ampersand; })
        .addProperty("Kp_dblampersand", +[](){ return nickel::Key::Kp_dblampersand; })
        .addProperty("Kp_verticalbar", +[](){ return nickel::Key::Kp_verticalbar; })
        .addProperty("Kp_dblverticalbar", +[](){ return nickel::Key::Kp_dblverticalbar; })
        .addProperty("Kp_colon", +[](){ return nickel::Key::Kp_colon; })
        .addProperty("Kp_hash", +[](){ return nickel::Key::Kp_hash; })
        .addProperty("Kp_space", +[](){ return nickel::Key::Kp_space; })
        .addProperty("Kp_at", +[](){ return nickel::Key::Kp_at; })
        .addProperty("Kp_exclam", +[](){ return nickel::Key::Kp_exclam; })
        .addProperty("Kp_memstore", +[](){ return nickel::Key::Kp_memstore; })
        .addProperty("Kp_memrecall", +[](){ return nickel::Key::Kp_memrecall; })
        .addProperty("Kp_memclear", +[](){ return nickel::Key::Kp_memclear; })
        .addProperty("Kp_memadd", +[](){ return nickel::Key::Kp_memadd; })
        .addProperty("Kp_memsubtract", +[](){ return nickel::Key::Kp_memsubtract; })
        .addProperty("Kp_memmultiply", +[](){ return nickel::Key::Kp_memmultiply; })
        .addProperty("Kp_memdivide", +[](){ return nickel::Key::Kp_memdivide; })
        .addProperty("Kp_plusminus", +[](){ return nickel::Key::Kp_plusminus; })
        .addProperty("Kp_clear", +[](){ return nickel::Key::Kp_clear; })
        .addProperty("Kp_clearentry", +[](){ return nickel::Key::Kp_clearentry; })
        .addProperty("Kp_binary", +[](){ return nickel::Key::Kp_binary; })
        .addProperty("Kp_octal", +[](){ return nickel::Key::Kp_octal; })
        .addProperty("Kp_decimal", +[](){ return nickel::Key::Kp_decimal; })
        .addProperty("Kp_hexadecimal", +[](){ return nickel::Key::Kp_hexadecimal; })
        .addProperty("Lctrl", +[](){ return nickel::Key::Lctrl; })
        .addProperty("Lshift", +[](){ return nickel::Key::Lshift; })
        .addProperty("Lalt", +[](){ return nickel::Key::Lalt; })
        .addProperty("Lgui", +[](){ return nickel::Key::Lgui; })
        .addProperty("Rctrl", +[](){ return nickel::Key::Rctrl; })
        .addProperty("Rshift", +[](){ return nickel::Key::Rshift; })
        .addProperty("Ralt", +[](){ return nickel::Key::Ralt; })
        .addProperty("Rgui", +[](){ return nickel::Key::Rgui; })
        .addProperty("Mode", +[](){ return nickel::Key::Mode; })
        .addProperty("Audionext", +[](){ return nickel::Key::Audionext; })
        .addProperty("Audioprev", +[](){ return nickel::Key::Audioprev; })
        .addProperty("Audiostop", +[](){ return nickel::Key::Audiostop; })
        .addProperty("Audioplay", +[](){ return nickel::Key::Audioplay; })
        .addProperty("Audiomute", +[](){ return nickel::Key::Audiomute; })
        .addProperty("Mediaselect", +[](){ return nickel::Key::Mediaselect; })
        .addProperty("Www", +[](){ return nickel::Key::Www; })
        .addProperty("Mail", +[](){ return nickel::Key::Mail; })
        .addProperty("Calculator", +[](){ return nickel::Key::Calculator; })
        .addProperty("Computer", +[](){ return nickel::Key::Computer; })
        .addProperty("Ac_search", +[](){ return nickel::Key::Ac_search; })
        .addProperty("Ac_home", +[](){ return nickel::Key::Ac_home; })
        .addProperty("Ac_back", +[](){ return nickel::Key::Ac_back; })
        .addProperty("Ac_forward", +[](){ return nickel::Key::Ac_forward; })
        .addProperty("Ac_stop", +[](){ return nickel::Key::Ac_stop; })
        .addProperty("Ac_refresh", +[](){ return nickel::Key::Ac_refresh; })
        .addProperty("Ac_bookmarks", +[](){ return nickel::Key::Ac_bookmarks; })
        .addProperty("Brightnessdown", +[](){ return nickel::Key::Brightnessdown; })
        .addProperty("Brightnessup", +[](){ return nickel::Key::Brightnessup; })
        .addProperty("Displayswitch", +[](){ return nickel::Key::Displayswitch; })
        .addProperty("Kbdillumtoggle", +[](){ return nickel::Key::Kbdillumtoggle; })
        .addProperty("Eject", +[](){ return nickel::Key::Eject; })
        .addProperty("Sleep", +[](){ return nickel::Key::Sleep; })
        .addProperty("App1", +[](){ return nickel::Key::App1; })
        .addProperty("App2", +[](){ return nickel::Key::App2; })
        .addProperty("Audiorewind", +[](){ return nickel::Key::Audiorewind; })
        .addProperty("Audiofastforward", +[](){ return nickel::Key::Audiofastforward; })
    .endNamespace()
    .beginClass<Mouse>("Mouse")
        .addFunction("LeftBtn", &Mouse::LeftBtn)
        .addFunction("RightBtn", &Mouse::RightBtn)
        .addFunction("MiddleBtn", &Mouse::MiddleBtn)
        .addFunction("Offset", &Mouse::Offset)
        .addFunction("Position", &Mouse::Position)
    .endClass()
    .beginClass<MouseButton>("MouseButton")
        .addFunction("IsPress", &MouseButton::IsPress)
        .addFunction("IsPressing", &MouseButton::IsPressing)
        .addFunction("IsReleasing", &MouseButton::IsReleasing)
        .addFunction("IsRelease", &MouseButton::IsRelease)
        .addFunction("IsReleased", &MouseButton::IsReleased)
    .endClass()
    .beginClass<Keyboard>("Keyboard")
        .addFunction("Key", &Keyboard::Key)
    .endClass()
    .beginClass<KeyButton>("KeyButton")
        .addFunction("IsPress", &KeyButton::IsPress)
        .addFunction("IsPressing", &KeyButton::IsPressing)
        .addFunction("IsReleasing", &KeyButton::IsReleasing)
        .addFunction("IsRelease", &KeyButton::IsRelease)
        .addFunction("IsReleased", &KeyButton::IsReleased)
    .endClass();
}

template <typename T>
bool hasComponent(std::underlying_type_t<gecs::entity> ent) {
    auto reg = ECS::Instance().World().cur_registry();
    if (reg) {
        return  reg->has<T>(static_cast<gecs::entity>(ent));
    }
    return false;
}

template <typename T>
T* getComponent(std::underlying_type_t<gecs::entity> ent) {
    auto reg = ECS::Instance().World().cur_registry();
    if (reg) {
        return  &reg->get_mut<T>(static_cast<gecs::entity>(ent));
    }
    return nullptr;
}

void bindECS(luabridge::Namespace& scope) {
    scope = 
    scope.beginNamespace("ecs")
        .addFunction("HasSprite", hasComponent<Sprite>)
        .addFunction("GetSprite", getComponent<Sprite>)
        .addFunction("HasTransform", hasComponent<Transform>)
        .addFunction("GetTransform", getComponent<Transform>)
        .beginNamespace("res")
            .addProperty("Keyboard", +[]()->const Keyboard& { return ECS::Instance().World().res<nickel::Keyboard>().get(); })
        .endNamespace()
    .endNamespace();
}

// clang-format on

void BindLua(lua_State* L) {
    auto key = ECS::Instance().World().res<nickel::Keyboard>();

    auto scope = luabridge::getGlobalNamespace(L).beginNamespace("nickel");
    bindMath(scope);
    bindCommon(scope);
    bindHandles(scope);
    bindTransform(scope);
    bindGraphics(scope);
    bindInput(scope);
    bindECS(scope);
    scope.endNamespace();
}

}  // namespace nickel