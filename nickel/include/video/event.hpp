#pragma once

#include "common/cgmath.hpp"
#include "common/ecs.hpp"
#include "video/window.hpp"

union SDL_Event;

namespace nickel {

// copied from SDL_Scancode, don't change them!
enum class Key {
    UNKNOWN = 0,

    A = 4,
    B = 5,
    C = 6,
    D = 7,
    E = 8,
    F = 9,
    G = 10,
    H = 11,
    I = 12,
    J = 13,
    K = 14,
    L = 15,
    M = 16,
    N = 17,
    O = 18,
    P = 19,
    Q = 20,
    R = 21,
    S = 22,
    T = 23,
    U = 24,
    V = 25,
    W = 26,
    X = 27,
    Y = 28,
    Z = 29,

    Num1 = 30,
    Num2 = 31,
    Num3 = 32,
    Num4 = 33,
    Num5 = 34,
    Num6 = 35,
    Num7 = 36,
    Num8 = 37,
    Num9 = 38,
    Num0 = 39,

    Return = 40,
    Escape = 41,
    Backspace = 32,
    Tab = 43,
    Space = 44,

    Minus = 45,
    Equals = 46,
    Leftbracket = 47,
    Rightbracket = 48,
    Backslash = 49,
    Nonushash = 50,
    Semicolon = 51,
    Apostrophe = 52,
    Grave = 53,
    Comma = 54,
    Period = 55,
    Slash = 56,
    Capslock = 57,

    F1 = 58,
    F2 = 59,
    F3 = 60,
    F4 = 61,
    F5 = 62,
    F6 = 63,
    F7 = 64,
    F8 = 65,
    F9 = 66,
    F10 = 67,
    F11 = 68,
    F12 = 69,

    Printscreen = 70,
    Scrolllock = 71,
    Pause = 72,
    Insert = 73,
    Home = 74,
    Pageup = 75,
    Delete = 76,
    End = 77,
    Pagedown = 78,
    Right = 79,
    Left = 80,
    Down = 81,
    Up = 82,
    Numlockclear = 83,
    Kp_divide = 84,
    Kp_multiply = 85,
    Kp_minus = 86,
    Kp_plus = 87,
    Kp_enter = 88,
    Kp_1 = 89,
    Kp_2 = 90,
    Kp_3 = 91,
    Kp_4 = 92,
    Kp_5 = 93,
    Kp_6 = 94,
    Kp_7 = 95,
    Kp_8 = 96,
    Kp_9 = 97,
    Kp_0 = 98,
    Kp_period = 99,
    Nonusbackslash = 100,
    Application = 101,
    Power = 102,
    Kp_equals = 103,
    F13 = 104,
    F14 = 105,
    F15 = 106,
    F16 = 107,
    F17 = 108,
    F18 = 109,
    F19 = 110,
    F20 = 111,
    F21 = 112,
    F22 = 113,
    F23 = 114,
    F24 = 115,
    Execute = 116,
    Help = 117,
    Menu = 118,
    Select = 119,
    Stop = 120,
    Again = 121, /**< redo */
    Undo = 122,
    Cut = 123,
    Copy = 124,
    Paste = 125,
    Find = 126,
    Mute = 127,
    Volumeup = 128,
    Volumedown = 129,
    Kp_comma = 133,
    Kp_equalsas400 = 134,
    International1 = 135,
    International2 = 136,
    International3 = 137,
    International4 = 138,
    International5 = 139,
    International6 = 140,
    International7 = 141,
    International8 = 142,
    International9 = 143,
    Lang1 = 144,
    Lang2 = 145,
    Lang3 = 146,
    Lang4 = 147,
    Lang5 = 148,
    Lang6 = 149,
    Lang7 = 150,
    Lang8 = 151,
    Lang9 = 152,
    Alterase = 153,
    Sysreq = 154,
    Cancel = 155,
    Clear = 156,
    Prior = 157,
    Return2 = 158,
    Separator = 159,
    Out = 160,
    Oper = 161,
    Clearagain = 162,
    Crsel = 163,
    Exsel = 164,
    Kp_00 = 176,
    Kp_000 = 177,
    Thousandsseparator = 178,
    Decimalseparator = 179,
    Currencyunit = 180,
    Currencysubunit = 181,
    Kp_leftparen = 182,
    Kp_rightparen = 183,
    Kp_leftbrace = 184,
    Kp_rightbrace = 185,
    Kp_tab = 186,
    Kp_backspace = 187,
    Kp_a = 188,
    Kp_b = 189,
    Kp_c = 190,
    Kp_d = 191,
    Kp_e = 192,
    Kp_f = 193,
    Kp_xor = 194,
    Kp_power = 195,
    Kp_percent = 196,
    Kp_less = 197,
    Kp_greater = 198,
    Kp_ampersand = 199,
    Kp_dblampersand = 200,
    Kp_verticalbar = 201,
    Kp_dblverticalbar = 202,
    Kp_colon = 203,
    Kp_hash = 204,
    Kp_space = 205,
    Kp_at = 206,
    Kp_exclam = 207,
    Kp_memstore = 208,
    Kp_memrecall = 209,
    Kp_memclear = 210,
    Kp_memadd = 211,
    Kp_memsubtract = 212,
    Kp_memmultiply = 213,
    Kp_memdivide = 214,
    Kp_plusminus = 215,
    Kp_clear = 216,
    Kp_clearentry = 217,
    Kp_binary = 218,
    Kp_octal = 219,
    Kp_decimal = 220,
    Kp_hexadecimal = 221,

    Lctrl = 224,
    Lshift = 225,
    Lalt = 226,
    Lgui = 227,
    Rctrl = 228,
    Rshift = 229,
    Ralt = 230,
    Rgui = 231,

    Mode = 257,

    Audionext = 258,
    Audioprev = 259,
    Audiostop = 260,
    Audioplay = 261,
    Audiomute = 262,
    Mediaselect = 263,
    Www = 264,
    Mail = 265,
    Calculator = 266,
    Computer = 267,
    Ac_search = 268,
    Ac_home = 269,
    Ac_back = 270,
    Ac_forward = 271,
    Ac_stop = 272,
    Ac_refresh = 273,
    Ac_bookmarks = 274,

    Brightnessdown = 275,
    Brightnessup = 276,
    Displayswitch = 277,
    Kbdillumtoggle = 278,
    Eject = 281,
    Sleep = 282,

    App1 = 283,
    App2 = 284,

    Audiorewind = 285,
    Audiofastforward = 286,

    KEY_LAST = 512,
};

Key GetKeyFromName(std::string_view name);

enum class MouseButtonType {
    Left = 1,
    Middle = 2,
    Right = 3,
};

enum class Action {
    Press = 1,
    Repeat,
    Release,
};

enum class Modifier {
    None = 0x0,                  
    LShift = 0x0001,
    RShift = 0x0002,
    LCtrl = 0x0040,
    RCtrl = 0x0080,
    LAlt = 0x0100,
    RAlt = 0x0200,
    LGui = 0x0400,
    RGui = 0x0800,
    Num = 0x1000,
    Caps = 0x2000,
    Mode = 0x4000,
    Scroll = 0x8000,
    GuiBoth = LGui | RGui,
    ShiftBoth = LShift | RShift,
    AltBoth = LAlt | RAlt,      
};                              
                                
struct QuitEvent final {};

struct KeyboardEvent final {
    Key key;
    Action action;
    Modifier mod;
};

struct MouseMotionEvent final {
    cgmath::Vec2 position;
    cgmath::Vec2 offset;
};

struct MouseButtonEvent final {
    MouseButtonType btn;
    Action action;
    uint8_t clicks;  // 1 for singl-click, 2 for double-click
};

struct ScrollEvent final {
    cgmath::Vec2 offset;
};

struct WindowResizeEvent final {
    cgmath::Vec2 size;
};

struct DropBeginEvent final {};

struct DropEndEvent final {};

struct DropTextEvent final {
    std::string text;
};

struct DropFileEvent final {
    std::filesystem::path path;
};

class EventPoller final {
public:
    using event_handler = void (*)(const SDL_Event& event);

    void Poll() const;

    template <event_handler h>
    void InjectHandler() {
        handlers_.emplace_back(h);
    }

private:
    std::vector<event_handler> handlers_;
};

}  // namespace nickel