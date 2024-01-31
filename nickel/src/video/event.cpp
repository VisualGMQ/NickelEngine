#include "video/event.hpp"
#include "common/profile.hpp"

#include "SDL.h"

namespace nickel {

const std::unordered_map<std::string_view, Key> gKeynameMap = {
    {"A",                  Key::A},
    {"B",                  Key::B},
    {"C",                  Key::C},
    {"D",                  Key::D},
    {"E",                  Key::E},
    {"F",                  Key::F},
    {"G",                  Key::G},
    {"H",                  Key::H},
    {"I",                  Key::I},
    {"J",                  Key::J},
    {"K",                  Key::K},
    {"L",                  Key::L},
    {"M",                  Key::M},
    {"N",                  Key::N},
    {"O",                  Key::O},
    {"P",                  Key::P},
    {"Q",                  Key::Q},
    {"R",                  Key::R},
    {"S",                  Key::S},
    {"T",                  Key::T},
    {"U",                  Key::U},
    {"V",                  Key::V},
    {"W",                  Key::W},
    {"X",                  Key::X},
    {"Y",                  Key::Y},
    {"Z",                  Key::Z},
    {"Num1",               Key::Num1},
    {"Num2",               Key::Num2},
    {"Num3",               Key::Num3},
    {"Num4",               Key::Num4},
    {"Num5",               Key::Num5},
    {"Num6",               Key::Num6},
    {"Num7",               Key::Num7},
    {"Num8",               Key::Num8},
    {"Num9",               Key::Num9},
    {"Num0",               Key::Num0},
    {"Return",             Key::Return},
    {"Escape",             Key::Escape},
    {"Backspace",          Key::Backspace},
    {"Tab",                Key::Tab},
    {"Space",              Key::Space},
    {"Minus",              Key::Minus},
    {"Equals",             Key::Equals},
    {"Leftbracket",        Key::Leftbracket},
    {"Rightbracket",       Key::Rightbracket},
    {"Backslash",          Key::Backslash},
    {"Apostrophe",         Key::Apostrophe},
    {"Grave",              Key::Grave},
    {"Period",             Key::Period},
    {"Slash",              Key::Slash},
    {"Capslock",           Key::Capslock},
    {"F1",                 Key::F1},
    {"F2",                 Key::F2},
    {"F3",                 Key::F3},
    {"F4",                 Key::F4},
    {"F5",                 Key::F5},
    {"F6",                 Key::F6},
    {"F7",                 Key::F7},
    {"F8",                 Key::F8},
    {"F9",                 Key::F9},
    {"F10",                Key::F10},
    {"F11",                Key::F11},
    {"F12",                Key::F12},
    {"Printscreen",        Key::Printscreen},
    {"Scrolllock",         Key::Scrolllock},
    {"Pause",              Key::Pause},
    {"Insert",             Key::Insert},
    {"Home",               Key::Home},
    {"Pageup",             Key::Pageup},
    {"Delete",             Key::Delete},
    {"End",                Key::End},
    {"Pagedown",           Key::Pagedown},
    {"Right",              Key::Right},
    {"Left",               Key::Left},
    {"Down",               Key::Down},
    {"Up",                 Key::Up},
    {"Numlockclear",       Key::Numlockclear},
    {"Kp_multiply",        Key::Kp_multiply},
    {"Kp_minus",           Key::Kp_minus},
    {"Kp_plus",            Key::Kp_plus},
    {"Kp_enter",           Key::Kp_enter},
    {"Kp_1",               Key::Kp_1},
    {"Kp_2",               Key::Kp_2},
    {"Kp_3",               Key::Kp_3},
    {"Kp_4",               Key::Kp_4},
    {"Kp_5",               Key::Kp_5},
    {"Kp_6",               Key::Kp_6},
    {"Kp_7",               Key::Kp_7},
    {"Kp_8",               Key::Kp_8},
    {"Kp_9",               Key::Kp_9},
    {"Kp_0",               Key::Kp_0},
    {"Kp_period",          Key::Kp_period},
    {"Nonusbackslash",     Key::Nonusbackslash},
    {"Application",        Key::Application},
    {"Power",              Key::Power},
    {"Kp_equals",          Key::Kp_equals},
    {"F13",                Key::F13},
    {"F14",                Key::F14},
    {"F15",                Key::F15},
    {"F16",                Key::F16},
    {"F17",                Key::F17},
    {"F18",                Key::F18},
    {"F19",                Key::F19},
    {"F20",                Key::F20},
    {"F21",                Key::F21},
    {"F22",                Key::F22},
    {"F23",                Key::F23},
    {"F24",                Key::F24},
    {"Execute",            Key::Execute},
    {"Help",               Key::Help},
    {"Menu",               Key::Menu},
    {"Select",             Key::Select},
    {"Stop",               Key::Stop},
    {"Again",              Key::Again},
    {"Undo",               Key::Undo},
    {"Cut",                Key::Cut},
    {"Copy",               Key::Copy},
    {"Paste",              Key::Paste},
    {"Find",               Key::Find},
    {"Mute",               Key::Mute},
    {"Volumeup",           Key::Volumeup},
    {"Volumedown",         Key::Volumedown},
    {"Kp_comma",           Key::Kp_comma},
    {"Kp_equalsas400",     Key::Kp_equalsas400},
    {"International1",     Key::International1},
    {"International2",     Key::International2},
    {"International3",     Key::International3},
    {"International4",     Key::International4},
    {"International5",     Key::International5},
    {"International6",     Key::International6},
    {"International7",     Key::International7},
    {"International8",     Key::International8},
    {"International9",     Key::International9},
    {"Lang1",              Key::Lang1},
    {"Lang2",              Key::Lang2},
    {"Lang3",              Key::Lang3},
    {"Lang4",              Key::Lang4},
    {"Lang5",              Key::Lang5},
    {"Lang6",              Key::Lang6},
    {"Lang7",              Key::Lang7},
    {"Lang8",              Key::Lang8},
    {"Lang9",              Key::Lang9},
    {"Alterase",           Key::Alterase},
    {"Sysreq",             Key::Sysreq},
    {"Cancel",             Key::Cancel},
    {"Clear",              Key::Clear},
    {"Prior",              Key::Prior},
    {"Return2",            Key::Return2},
    {"Separator",          Key::Separator},
    {"Out",                Key::Out},
    {"Oper",               Key::Oper},
    {"Clearagain",         Key::Clearagain},
    {"Crsel",              Key::Crsel},
    {"Exsel",              Key::Exsel},
    {"Kp_00",              Key::Kp_00},
    {"Kp_000",             Key::Kp_000},
    {"Thousandsseparator", Key::Thousandsseparator},
    {"Decimalseparator",   Key::Decimalseparator},
    {"Currencyunit",       Key::Currencyunit},
    {"Currencysubunit",    Key::Currencysubunit},
    {"Kp_leftparen",       Key::Kp_leftparen},
    {"Kp_rightparen",      Key::Kp_rightparen},
    {"Kp_leftbrace",       Key::Kp_leftbrace},
    {"Kp_rightbrace",      Key::Kp_rightbrace},
    {"Kp_tab",             Key::Kp_tab},
    {"Kp_backspace",       Key::Kp_backspace},
    {"Kp_a",               Key::Kp_a},
    {"Kp_b",               Key::Kp_b},
    {"Kp_c",               Key::Kp_c},
    {"Kp_d",               Key::Kp_d},
    {"Kp_e",               Key::Kp_e},
    {"Kp_f",               Key::Kp_f},
    {"Kp_xor",             Key::Kp_xor},
    {"Kp_power",           Key::Kp_power},
    {"Kp_percent",         Key::Kp_percent},
    {"Kp_less",            Key::Kp_less},
    {"Kp_greater",         Key::Kp_greater},
    {"Kp_ampersand",       Key::Kp_ampersand},
    {"Kp_dblampersand",    Key::Kp_dblampersand},
    {"Kp_verticalbar",     Key::Kp_verticalbar},
    {"Kp_dblverticalbar",  Key::Kp_dblverticalbar},
    {"Kp_colon",           Key::Kp_colon},
    {"Kp_hash",            Key::Kp_hash},
    {"Kp_space",           Key::Kp_space},
    {"Kp_at",              Key::Kp_at},
    {"Kp_exclam",          Key::Kp_exclam},
    {"Kp_memstore",        Key::Kp_memstore},
    {"Kp_memrecall",       Key::Kp_memrecall},
    {"Kp_memclear",        Key::Kp_memclear},
    {"Kp_memadd",          Key::Kp_memadd},
    {"Kp_memsubtract",     Key::Kp_memsubtract},
    {"Kp_memmultiply",     Key::Kp_memmultiply},
    {"Kp_memdivide",       Key::Kp_memdivide},
    {"Kp_plusminus",       Key::Kp_plusminus},
    {"Kp_clear",           Key::Kp_clear},
    {"Kp_clearentry",      Key::Kp_clearentry},
    {"Kp_binary",          Key::Kp_binary},
    {"Kp_octal",           Key::Kp_octal},
    {"Kp_decimal",         Key::Kp_decimal},
    {"Kp_hexadecimal",     Key::Kp_hexadecimal},
    {"Lctrl",              Key::Lctrl},
    {"Lshift",             Key::Lshift},
    {"Lalt",               Key::Lalt},
    {"Lgui",               Key::Lgui},
    {"Rctrl",              Key::Rctrl},
    {"Rshift",             Key::Rshift},
    {"Ralt",               Key::Ralt},
    {"Rgui",               Key::Rgui},
    {"Mode",               Key::Mode},
    {"Audionext",          Key::Audionext},
    {"Audioprev",          Key::Audioprev},
    {"Audiostop",          Key::Audiostop},
    {"Audioplay",          Key::Audioplay},
    {"Audiomute",          Key::Audiomute},
    {"Mediaselect",        Key::Mediaselect},
    {"Www",                Key::Www},
    {"Mail",               Key::Mail},
    {"Calculator",         Key::Calculator},
    {"Computer",           Key::Computer},
    {"Ac_search",          Key::Ac_search},
    {"Ac_home",            Key::Ac_home},
    {"Ac_back",            Key::Ac_back},
    {"Ac_forward",         Key::Ac_forward},
    {"Ac_stop",            Key::Ac_stop},
    {"Ac_refresh",         Key::Ac_refresh},
    {"Ac_bookmarks",       Key::Ac_bookmarks},
    {"Brightnessdown",     Key::Brightnessdown},
    {"Brightnessup",       Key::Brightnessup},
    {"Displayswitch",      Key::Displayswitch},
    {"Kbdillumtoggle",     Key::Kbdillumtoggle},
    {"Eject",              Key::Eject},
    {"Sleep",              Key::Sleep},
    {"App1",               Key::App1},
    {"App2",               Key::App2},
    {"Audiorewind",        Key::Audiorewind},
    {"Audiofastforward",   Key::Audiofastforward},
};

Key GetKeyFromName(std::string_view name) {
    if (auto it = gKeynameMap.find(name); it != gKeynameMap.end()) {
        return it->second;
    }
    return Key::UNKNOWN;
}

void EventPoller::Poll() const {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        auto reg = ECS::Instance().World().cur_registry();

        for (auto h : handlers_) {
            h(event);
        }

        if (event.type == SDL_QUIT) {
            reg->event_dispatcher<QuitEvent>().enqueue();
        }

        if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
            KeyboardEvent e;
            e.key = static_cast<Key>(event.key.keysym.scancode);
            e.action = event.key.repeat
                           ? Action::Repeat
                           : (event.type == SDL_KEYDOWN ? Action::Press
                                                        : Action::Release);
            e.mod = static_cast<Modifier>(event.key.keysym.mod);

            reg->event_dispatcher<KeyboardEvent>().enqueue(e);
        }

        if (event.type == SDL_MOUSEMOTION) {
            MouseMotionEvent e;
            e.position.x = event.motion.x;
            e.position.y = event.motion.y;
            e.offset.x = event.motion.xrel;
            e.offset.y = event.motion.yrel;
            reg->event_dispatcher<MouseMotionEvent>().enqueue(e);
        }

        if (event.type == SDL_MOUSEBUTTONDOWN ||
            event.type == SDL_MOUSEBUTTONUP) {
            MouseButtonEvent e;
            e.action = event.type == SDL_MOUSEBUTTONDOWN ? Action::Press
                                                         : Action::Release;
            e.btn = static_cast<MouseButtonType>(event.button.button);
            e.clicks = event.button.clicks;

            reg->event_dispatcher<MouseButtonEvent>().enqueue(e);
        }

        if (event.type == SDL_WINDOWEVENT) {
            if (event.window.event == SDL_WINDOWEVENT_RESIZED ||
                event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                WindowResizeEvent e;
                e.size.x = event.window.data1;
                e.size.y = event.window.data2;

                reg->event_dispatcher<WindowResizeEvent>().enqueue(e);
            }
        }

        if (event.type == SDL_DROPBEGIN) {
            reg->event_dispatcher<DropBeginEvent>().enqueue(DropBeginEvent{});
        }

        if (event.type == SDL_DROPCOMPLETE) {
            reg->event_dispatcher<DropEndEvent>().enqueue(DropEndEvent{});
        }

        if (event.type == SDL_DROPTEXT) {
            DropTextEvent e;
            e.text = event.drop.file;
            SDL_free(event.drop.file);
            reg->event_dispatcher<DropTextEvent>().enqueue(e);
        }

        if (event.type == SDL_DROPFILE) {
            DropFileEvent e;
            e.path = event.drop.file;
            SDL_free(event.drop.file);
            reg->event_dispatcher<DropFileEvent>().enqueue(e);
        }
    }
}

}  // namespace nickel