#pragma once

#include "core/cgmath.hpp"
#include "pch.hpp"
#include "window/window.hpp"

namespace nickel {

// copied from SDL_Scancode, don't change them!
enum class Key {
    UNKNOWN = SDL_SCANCODE_UNKNOWN,

    A = SDL_SCANCODE_A,
    B = SDL_SCANCODE_B,
    C = SDL_SCANCODE_C,
    D = SDL_SCANCODE_D,
    E = SDL_SCANCODE_E,
    F = SDL_SCANCODE_F,
    G = SDL_SCANCODE_G,
    H = SDL_SCANCODE_H,
    I = SDL_SCANCODE_I,
    J = SDL_SCANCODE_J,
    K = SDL_SCANCODE_K,
    L = SDL_SCANCODE_L,
    M = SDL_SCANCODE_M,
    N = SDL_SCANCODE_N,
    O = SDL_SCANCODE_O,
    P = SDL_SCANCODE_P,
    Q = SDL_SCANCODE_Q,
    R = SDL_SCANCODE_R,
    S = SDL_SCANCODE_S,
    T = SDL_SCANCODE_T,
    U = SDL_SCANCODE_U,
    V = SDL_SCANCODE_V,
    W = SDL_SCANCODE_W,
    X = SDL_SCANCODE_X,
    Y = SDL_SCANCODE_Y,
    Z = SDL_SCANCODE_Z,

    Num1 = SDL_SCANCODE_1,
    Num2 = SDL_SCANCODE_2,
    Num3 = SDL_SCANCODE_3,
    Num4 = SDL_SCANCODE_4,
    Num5 = SDL_SCANCODE_5,
    Num6 = SDL_SCANCODE_6,
    Num7 = SDL_SCANCODE_7,
    Num8 = SDL_SCANCODE_8,
    Num9 = SDL_SCANCODE_9,
    Num0 = SDL_SCANCODE_0,

    Return = SDL_SCANCODE_RETURN,
    Escape = SDL_SCANCODE_ESCAPE,
    Backspace = SDL_SCANCODE_BACKSPACE,
    Tab = SDL_SCANCODE_TAB,
    Space = SDL_SCANCODE_SPACE,

    Minus = SDL_SCANCODE_MINUS,
    Equals = SDL_SCANCODE_EQUALS,
    Leftbracket = SDL_SCANCODE_LEFTBRACKET,
    Rightbracket = SDL_SCANCODE_RIGHTBRACKET,
    Backslash =
        SDL_SCANCODE_BACKSLASH, /**< Located at the lower left of the return
                                 *   key on ISO keyboards and at the right end
                                 *   of the QWERTY row on ANSI keyboards.
                                 *   Produces REVERSE SOLIDUS (backslash) and
                                 *   VERTICAL LINE in a US layout, REVERSE
                                 *   SOLIDUS and VERTICAL LINE in a UK Mac
                                 *   layout, NUMBER SIGN and TILDE in a UK
                                 *   Windows layout, DOLLAR SIGN and POUND SIGN
                                 *   in a Swiss German layout, NUMBER SIGN and
                                 *   APOSTROPHE in a German layout, GRAVE
                                 *   ACCENT and POUND SIGN in a French Mac
                                 *   layout, and ASTERISK and MICRO SIGN in a
                                 *   French Windows layout.
                                 */
    Nonushash =
        SDL_SCANCODE_NONUSHASH, /**< ISO USB keyboards actually use this code
                                 *   instead of 49 for the same key, but all
                                 *   OSes I've seen treat the two codes
                                 *   identically. So, as an implementor, unless
                                 *   your keyboard generates both of those
                                 *   codes and your OS treats them differently,
                                 *   you should generate SDL_SCANCODE_BACKSLASH
                                 *   instead of this code. As a user, you
                                 *   should not rely on this code because SDL
                                 *   will never generate it with most (all?)
                                 *   keyboards.
                                 */
    Semicolon = SDL_SCANCODE_SEMICOLON,
    Apostrophe = SDL_SCANCODE_APOSTROPHE,
    Grave =
        SDL_SCANCODE_GRAVE, /**< Located in the top left corner (on both ANSI
                             *   and ISO keyboards). Produces GRAVE ACCENT and
                             *   TILDE in a US Windows layout and in US and UK
                             *   Mac layouts on ANSI keyboards, GRAVE ACCENT
                             *   and NOT SIGN in a UK Windows layout, SECTION
                             *   SIGN and PLUS-MINUS SIGN in US and UK Mac
                             *   layouts on ISO keyboards, SECTION SIGN and
                             *   DEGREE SIGN in a Swiss German layout (Mac:
                             *   only on ISO keyboards), CIRCUMFLEX ACCENT and
                             *   DEGREE SIGN in a German layout (Mac: only on
                             *   ISO keyboards), SUPERSCRIPT TWO and TILDE in a
                             *   French Windows layout, COMMERCIAL AT and
                             *   NUMBER SIGN in a French Mac layout on ISO
                             *   keyboards, and LESS-THAN SIGN and GREATER-THAN
                             *   SIGN in a Swiss German, German, or French Mac
                             *   layout on ANSI keyboards.
                             */
    Comma = SDL_SCANCODE_COMMA,
    Period = SDL_SCANCODE_PERIOD,
    Slash = SDL_SCANCODE_SLASH,

    Capslock = SDL_SCANCODE_CAPSLOCK,

    F1 = SDL_SCANCODE_F1,
    F2 = SDL_SCANCODE_F2,
    F3 = SDL_SCANCODE_F3,
    F4 = SDL_SCANCODE_F4,
    F5 = SDL_SCANCODE_F5,
    F6 = SDL_SCANCODE_F6,
    F7 = SDL_SCANCODE_F7,
    F8 = SDL_SCANCODE_F8,
    F9 = SDL_SCANCODE_F9,
    F10 = SDL_SCANCODE_F10,
    F11 = SDL_SCANCODE_F11,
    F12 = SDL_SCANCODE_F12,

    Printscreen = SDL_SCANCODE_PRINTSCREEN,
    Scrolllock = SDL_SCANCODE_SCROLLLOCK,
    Pause = SDL_SCANCODE_PAUSE,
    Insert = SDL_SCANCODE_INSERT, /**< insert on PC, help on some Mac keyboards
                                   (but does send code 73, not 117) */
    Home = SDL_SCANCODE_HOME,
    Pageup = SDL_SCANCODE_PAGEUP,
    Delete = SDL_SCANCODE_DELETE,
    End = SDL_SCANCODE_END,
    Pagedown = SDL_SCANCODE_PAGEDOWN,
    Right = SDL_SCANCODE_RIGHT,
    Left = SDL_SCANCODE_LEFT,
    Down = SDL_SCANCODE_DOWN,
    Up = SDL_SCANCODE_UP,

    Numlockclear =
        SDL_SCANCODE_NUMLOCKCLEAR, /**< num lock on PC, clear on Mac keyboards
                                    */
    Kp_divide = SDL_SCANCODE_KP_DIVIDE,
    Kp_multiply = SDL_SCANCODE_KP_MULTIPLY,
    Kp_minus = SDL_SCANCODE_KP_MINUS,
    Kp_plus = SDL_SCANCODE_KP_PLUS,
    Kp_enter = SDL_SCANCODE_KP_ENTER,
    Kp_1 = SDL_SCANCODE_KP_1,
    Kp_2 = SDL_SCANCODE_KP_2,
    Kp_3 = SDL_SCANCODE_KP_3,
    Kp_4 = SDL_SCANCODE_KP_4,
    Kp_5 = SDL_SCANCODE_KP_5,
    Kp_6 = SDL_SCANCODE_KP_6,
    Kp_7 = SDL_SCANCODE_KP_7,
    Kp_8 = SDL_SCANCODE_KP_8,
    Kp_9 = SDL_SCANCODE_KP_9,
    Kp_0 = SDL_SCANCODE_KP_0,
    Kp_period = SDL_SCANCODE_KP_PERIOD,

    Nonusbackslash =
        SDL_SCANCODE_NONUSBACKSLASH, /**< This is the additional key that ISO
                                      *   keyboards have over ANSI ones,
                                      *   located between left shift and Y.
                                      *   Produces GRAVE ACCENT and TILDE in a
                                      *   US or UK Mac layout, REVERSE SOLIDUS
                                      *   (backslash) and VERTICAL LINE in a
                                      *   US or UK Windows layout, and
                                      *   LESS-THAN SIGN and GREATER-THAN SIGN
                                      *   in a Swiss German, German, or French
                                      *   layout. */
    Application =
        SDL_SCANCODE_APPLICATION, /**< windows contextual menu, compose */
    Power = SDL_SCANCODE_POWER,   /**< The USB document says this is a status
                                   * flag,   not a physical key - but some Mac
                                   * keyboards   do have a power key. */
    Kp_equals = SDL_SCANCODE_KP_EQUALS,
    F13 = SDL_SCANCODE_F13,
    F14 = SDL_SCANCODE_F14,
    F15 = SDL_SCANCODE_F15,
    F16 = SDL_SCANCODE_F16,
    F17 = SDL_SCANCODE_F17,
    F18 = SDL_SCANCODE_F18,
    F19 = SDL_SCANCODE_F19,
    F20 = SDL_SCANCODE_F20,
    F21 = SDL_SCANCODE_F21,
    F22 = SDL_SCANCODE_F22,
    F23 = SDL_SCANCODE_F23,
    F24 = SDL_SCANCODE_F24,
    Execute = SDL_SCANCODE_EXECUTE,
    Help = SDL_SCANCODE_HELP,
    Menu = SDL_SCANCODE_MENU,
    Select = SDL_SCANCODE_SELECT,
    Stop = SDL_SCANCODE_STOP,
    Again = SDL_SCANCODE_AGAIN, /**< redo */
    Undo = SDL_SCANCODE_UNDO,
    Cut = SDL_SCANCODE_CUT,
    Copy = SDL_SCANCODE_COPY,
    Paste = SDL_SCANCODE_PASTE,
    Find = SDL_SCANCODE_FIND,
    Mute = SDL_SCANCODE_MUTE,
    Volumeup = SDL_SCANCODE_VOLUMEUP,
    Volumedown = SDL_SCANCODE_VOLUMEDOWN,
    Kp_comma = SDL_SCANCODE_KP_COMMA,
    Kp_equalsas400 = SDL_SCANCODE_KP_EQUALSAS400,

    International1 = SDL_SCANCODE_INTERNATIONAL1, /**< used on Asian keyboards,
                                            see footnotes in USB doc */
    International2 = SDL_SCANCODE_INTERNATIONAL2,
    International3 = SDL_SCANCODE_INTERNATIONAL3, /**< Yen */
    International4 = SDL_SCANCODE_INTERNATIONAL4,
    International5 = SDL_SCANCODE_INTERNATIONAL5,
    International6 = SDL_SCANCODE_INTERNATIONAL6,
    International7 = SDL_SCANCODE_INTERNATIONAL7,
    International8 = SDL_SCANCODE_INTERNATIONAL8,
    International9 = SDL_SCANCODE_INTERNATIONAL9,
    Lang1 = SDL_SCANCODE_LANG1, /**< Hangul/English toggle */
    Lang2 = SDL_SCANCODE_LANG2, /**< Hanja conversion */
    Lang3 = SDL_SCANCODE_LANG3, /**< Katakana */
    Lang4 = SDL_SCANCODE_LANG4, /**< Hiragana */
    Lang5 = SDL_SCANCODE_LANG5, /**< Zenkaku/Hankaku */
    Lang6 = SDL_SCANCODE_LANG6, /**< reserved */
    Lang7 = SDL_SCANCODE_LANG7, /**< reserved */
    Lang8 = SDL_SCANCODE_LANG8, /**< reserved */
    Lang9 = SDL_SCANCODE_LANG9, /**< reserved */

    Alterase = SDL_SCANCODE_ALTERASE, /**< Erase-Eaze */
    Sysreq = SDL_SCANCODE_SYSREQ,
    Cancel = SDL_SCANCODE_CANCEL,
    Clear = SDL_SCANCODE_CLEAR,
    Prior = SDL_SCANCODE_PRIOR,
    Return2 = SDL_SCANCODE_RETURN2,
    Separator = SDL_SCANCODE_SEPARATOR,
    Out = SDL_SCANCODE_OUT,
    Oper = SDL_SCANCODE_OPER,
    Clearagain = SDL_SCANCODE_CLEARAGAIN,
    Crsel = SDL_SCANCODE_CRSEL,
    Exsel = SDL_SCANCODE_EXSEL,

    Kp_00 = SDL_SCANCODE_KP_00,
    Kp_000 = SDL_SCANCODE_KP_000,
    Thousandsseparator = SDL_SCANCODE_THOUSANDSSEPARATOR,
    Decimalseparator = SDL_SCANCODE_DECIMALSEPARATOR,
    Currencyunit = SDL_SCANCODE_CURRENCYUNIT,
    Currencysubunit = SDL_SCANCODE_CURRENCYSUBUNIT,
    Kp_leftparen = SDL_SCANCODE_KP_LEFTPAREN,
    Kp_rightparen = SDL_SCANCODE_KP_RIGHTPAREN,
    Kp_leftbrace = SDL_SCANCODE_KP_LEFTBRACE,
    Kp_rightbrace = SDL_SCANCODE_KP_RIGHTBRACE,
    Kp_tab = SDL_SCANCODE_KP_TAB,
    Kp_backspace = SDL_SCANCODE_KP_BACKSPACE,
    Kp_a = SDL_SCANCODE_KP_A,
    Kp_b = SDL_SCANCODE_KP_B,
    Kp_c = SDL_SCANCODE_KP_C,
    Kp_d = SDL_SCANCODE_KP_D,
    Kp_e = SDL_SCANCODE_KP_E,
    Kp_f = SDL_SCANCODE_KP_F,
    Kp_xor = SDL_SCANCODE_KP_XOR,
    Kp_power = SDL_SCANCODE_KP_POWER,
    Kp_percent = SDL_SCANCODE_KP_PERCENT,
    Kp_less = SDL_SCANCODE_KP_LESS,
    Kp_greater = SDL_SCANCODE_KP_GREATER,
    Kp_ampersand = SDL_SCANCODE_KP_AMPERSAND,
    Kp_dblampersand = SDL_SCANCODE_KP_DBLAMPERSAND,
    Kp_verticalbar = SDL_SCANCODE_KP_VERTICALBAR,
    Kp_dblverticalbar = SDL_SCANCODE_KP_DBLVERTICALBAR,
    Kp_colon = SDL_SCANCODE_KP_COLON,
    Kp_hash = SDL_SCANCODE_KP_HASH,
    Kp_space = SDL_SCANCODE_KP_SPACE,
    Kp_at = SDL_SCANCODE_KP_AT,
    Kp_exclam = SDL_SCANCODE_KP_EXCLAM,
    Kp_memstore = SDL_SCANCODE_KP_MEMSTORE,
    Kp_memrecall = SDL_SCANCODE_KP_MEMRECALL,
    Kp_memclear = SDL_SCANCODE_KP_MEMCLEAR,
    Kp_memadd = SDL_SCANCODE_KP_MEMADD,
    Kp_memsubtract = SDL_SCANCODE_KP_MEMSUBTRACT,
    Kp_memmultiply = SDL_SCANCODE_KP_MEMMULTIPLY,
    Kp_memdivide = SDL_SCANCODE_KP_MEMDIVIDE,
    Kp_plusminus = SDL_SCANCODE_KP_PLUSMINUS,
    Kp_clear = SDL_SCANCODE_KP_CLEAR,
    Kp_clearentry = SDL_SCANCODE_KP_CLEARENTRY,
    Kp_binary = SDL_SCANCODE_KP_BINARY,
    Kp_octal = SDL_SCANCODE_KP_OCTAL,
    Kp_decimal = SDL_SCANCODE_KP_DECIMAL,
    Kp_hexadecimal = SDL_SCANCODE_KP_HEXADECIMAL,

    Lctrl = SDL_SCANCODE_LCTRL,
    Lshift = SDL_SCANCODE_LSHIFT,
    Lalt = SDL_SCANCODE_LALT, /**< alt, option */
    Lgui = SDL_SCANCODE_LGUI, /**< windows, command (apple), meta */
    Rctrl = SDL_SCANCODE_RCTRL,
    Rshift = SDL_SCANCODE_RSHIFT,
    Ralt = SDL_SCANCODE_RALT, /**< alt gr, option */
    Rgui = SDL_SCANCODE_RGUI, /**< windows, command (apple), meta */

    Mode = SDL_SCANCODE_MODE, /**< I'm not sure if this is really not covered
                               *   by any of the above, but since there's a
                               *   special KMOD_MODE for it I'm adding it here
                               */

    Audionext = SDL_SCANCODE_AUDIONEXT,
    Audioprev = SDL_SCANCODE_AUDIOPREV,
    Audiostop = SDL_SCANCODE_AUDIOSTOP,
    Audioplay = SDL_SCANCODE_AUDIOPLAY,
    Audiomute = SDL_SCANCODE_AUDIOMUTE,
    Mediaselect = SDL_SCANCODE_MEDIASELECT,
    Www = SDL_SCANCODE_WWW,
    Mail = SDL_SCANCODE_MAIL,
    Calculator = SDL_SCANCODE_CALCULATOR,
    Computer = SDL_SCANCODE_COMPUTER,
    Ac_search = SDL_SCANCODE_AC_SEARCH,
    Ac_home = SDL_SCANCODE_AC_HOME,
    Ac_back = SDL_SCANCODE_AC_BACK,
    Ac_forward = SDL_SCANCODE_AC_FORWARD,
    Ac_stop = SDL_SCANCODE_AC_STOP,
    Ac_refresh = SDL_SCANCODE_AC_REFRESH,
    Ac_bookmarks = SDL_SCANCODE_AC_BOOKMARKS,

    Brightnessdown = SDL_SCANCODE_BRIGHTNESSDOWN,
    Brightnessup = SDL_SCANCODE_BRIGHTNESSUP,
    Displayswitch = SDL_SCANCODE_DISPLAYSWITCH, /**< display mirroring/dual
                                           display switch, video mode switch */
    Kbdillumtoggle = SDL_SCANCODE_KBDILLUMTOGGLE,
    Kbdillumdown = SDL_SCANCODE_KBDILLUMDOWN,
    Kbdillumup = SDL_SCANCODE_KBDILLUMUP,
    Eject = SDL_SCANCODE_EJECT,
    Sleep = SDL_SCANCODE_SLEEP,

    App1 = SDL_SCANCODE_APP1,
    App2 = SDL_SCANCODE_APP2,

    Audiorewind = SDL_SCANCODE_AUDIOREWIND,
    Audiofastforward = SDL_SCANCODE_AUDIOFASTFORWARD,

    KEY_LAST = SDL_NUM_SCANCODES,
};

Key GetKeyFromName(std::string_view name);

enum class MouseButtonType {
    Left = SDL_BUTTON_LEFT,
    Right = SDL_BUTTON_RIGHT,
    Middle = SDL_BUTTON_MIDDLE,
};

enum class Action {
    Press = 1,
    Repeat,
    Release,
};

enum class Modifier {
    LShift = KMOD_LSHIFT,
    RShift = KMOD_RSHIFT,
    LCtrl = KMOD_LCTRL,
    RCtrl = KMOD_RCTRL,
    LAlt = KMOD_LALT,
    RAlt = KMOD_RALT,
    LGui = KMOD_LGUI,
    RGui = KMOD_RGUI,
    Num = KMOD_NUM,
    Caps = KMOD_CAPS,
    Mode = KMOD_MODE,
    Scroll = KMOD_SCROLL,
    CtrlBoth = KMOD_CTRL,
    GuiBoth = KMOD_GUI,
    ShiftBoth = KMOD_SHIFT,
    AltBoth = KMOD_ALT,
};

struct QuitEvent final {};

struct KeyboardEvent final {
    Key key;
    Action action;
    Modifier mod;
};

struct MouseMotionEvent final {
    cgmath::Vec2 position;
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

class EventPoller final {
public:
    using event_handler = void (*)(const SDL_Event& event);

    void Poll() const;

    template <event_handler h>
    void InjectHandler() {
        handlers_.emplace_back(h);
    }

    static void AssociatePollerAndECS(
        typename gecs::world::registry_type& reg) {
        reg_ = &reg;
    }

    static typename gecs::world::registry_type* reg_;

private:
    std::vector<event_handler> handlers_;
};

}  // namespace nickel