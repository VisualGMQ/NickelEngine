#include "window/event.hpp"

namespace nickel {

typename gecs::world::registry_type* EventPoller::reg_ = nullptr;

const std::unordered_map<std::string_view, Key> gKeynameMap = {
    {                 nameof::nameof_enum(Key::A),                  Key::A},
    {                 nameof::nameof_enum(Key::B),                  Key::B},
    {                 nameof::nameof_enum(Key::C),                  Key::C},
    {                 nameof::nameof_enum(Key::D),                  Key::D},
    {                 nameof::nameof_enum(Key::E),                  Key::E},
    {                 nameof::nameof_enum(Key::F),                  Key::F},
    {                 nameof::nameof_enum(Key::G),                  Key::G},
    {                 nameof::nameof_enum(Key::H),                  Key::H},
    {                 nameof::nameof_enum(Key::I),                  Key::I},
    {                 nameof::nameof_enum(Key::J),                  Key::J},
    {                 nameof::nameof_enum(Key::K),                  Key::K},
    {                 nameof::nameof_enum(Key::L),                  Key::L},
    {                 nameof::nameof_enum(Key::M),                  Key::M},
    {                 nameof::nameof_enum(Key::N),                  Key::N},
    {                 nameof::nameof_enum(Key::O),                  Key::O},
    {                 nameof::nameof_enum(Key::P),                  Key::P},
    {                 nameof::nameof_enum(Key::Q),                  Key::Q},
    {                 nameof::nameof_enum(Key::R),                  Key::R},
    {                 nameof::nameof_enum(Key::S),                  Key::S},
    {                 nameof::nameof_enum(Key::T),                  Key::T},
    {                 nameof::nameof_enum(Key::U),                  Key::U},
    {                 nameof::nameof_enum(Key::V),                  Key::V},
    {                 nameof::nameof_enum(Key::W),                  Key::W},
    {                 nameof::nameof_enum(Key::X),                  Key::X},
    {                 nameof::nameof_enum(Key::Y),                  Key::Y},
    {                 nameof::nameof_enum(Key::Z),                  Key::Z},
    {              nameof::nameof_enum(Key::Num1),               Key::Num1},
    {              nameof::nameof_enum(Key::Num2),               Key::Num2},
    {              nameof::nameof_enum(Key::Num3),               Key::Num3},
    {              nameof::nameof_enum(Key::Num4),               Key::Num4},
    {              nameof::nameof_enum(Key::Num5),               Key::Num5},
    {              nameof::nameof_enum(Key::Num6),               Key::Num6},
    {              nameof::nameof_enum(Key::Num7),               Key::Num7},
    {              nameof::nameof_enum(Key::Num8),               Key::Num8},
    {              nameof::nameof_enum(Key::Num9),               Key::Num9},
    {              nameof::nameof_enum(Key::Num0),               Key::Num0},
    {            nameof::nameof_enum(Key::Return),             Key::Return},
    {            nameof::nameof_enum(Key::Escape),             Key::Escape},
    {         nameof::nameof_enum(Key::Backspace),          Key::Backspace},
    {               nameof::nameof_enum(Key::Tab),                Key::Tab},
    {             nameof::nameof_enum(Key::Space),              Key::Space},
    {             nameof::nameof_enum(Key::Minus),              Key::Minus},
    {            nameof::nameof_enum(Key::Equals),             Key::Equals},
    {       nameof::nameof_enum(Key::Leftbracket),        Key::Leftbracket},
    {      nameof::nameof_enum(Key::Rightbracket),       Key::Rightbracket},
    {         nameof::nameof_enum(Key::Backslash),          Key::Backslash},
    {        nameof::nameof_enum(Key::Apostrophe),         Key::Apostrophe},
    {             nameof::nameof_enum(Key::Grave),              Key::Grave},
    {            nameof::nameof_enum(Key::Period),             Key::Period},
    {             nameof::nameof_enum(Key::Slash),              Key::Slash},
    {          nameof::nameof_enum(Key::Capslock),           Key::Capslock},
    {                nameof::nameof_enum(Key::F1),                 Key::F1},
    {                nameof::nameof_enum(Key::F2),                 Key::F2},
    {                nameof::nameof_enum(Key::F3),                 Key::F3},
    {                nameof::nameof_enum(Key::F4),                 Key::F4},
    {                nameof::nameof_enum(Key::F5),                 Key::F5},
    {                nameof::nameof_enum(Key::F6),                 Key::F6},
    {                nameof::nameof_enum(Key::F7),                 Key::F7},
    {                nameof::nameof_enum(Key::F8),                 Key::F8},
    {                nameof::nameof_enum(Key::F9),                 Key::F9},
    {               nameof::nameof_enum(Key::F10),                Key::F10},
    {               nameof::nameof_enum(Key::F11),                Key::F11},
    {               nameof::nameof_enum(Key::F12),                Key::F12},
    {       nameof::nameof_enum(Key::Printscreen),        Key::Printscreen},
    {        nameof::nameof_enum(Key::Scrolllock),         Key::Scrolllock},
    {             nameof::nameof_enum(Key::Pause),              Key::Pause},
    {            nameof::nameof_enum(Key::Insert),             Key::Insert},
    {              nameof::nameof_enum(Key::Home),               Key::Home},
    {            nameof::nameof_enum(Key::Pageup),             Key::Pageup},
    {            nameof::nameof_enum(Key::Delete),             Key::Delete},
    {               nameof::nameof_enum(Key::End),                Key::End},
    {          nameof::nameof_enum(Key::Pagedown),           Key::Pagedown},
    {             nameof::nameof_enum(Key::Right),              Key::Right},
    {              nameof::nameof_enum(Key::Left),               Key::Left},
    {              nameof::nameof_enum(Key::Down),               Key::Down},
    {                nameof::nameof_enum(Key::Up),                 Key::Up},
    {      nameof::nameof_enum(Key::Numlockclear),       Key::Numlockclear},
    {       nameof::nameof_enum(Key::Kp_multiply),        Key::Kp_multiply},
    {          nameof::nameof_enum(Key::Kp_minus),           Key::Kp_minus},
    {           nameof::nameof_enum(Key::Kp_plus),            Key::Kp_plus},
    {          nameof::nameof_enum(Key::Kp_enter),           Key::Kp_enter},
    {              nameof::nameof_enum(Key::Kp_1),               Key::Kp_1},
    {              nameof::nameof_enum(Key::Kp_2),               Key::Kp_2},
    {              nameof::nameof_enum(Key::Kp_3),               Key::Kp_3},
    {              nameof::nameof_enum(Key::Kp_4),               Key::Kp_4},
    {              nameof::nameof_enum(Key::Kp_5),               Key::Kp_5},
    {              nameof::nameof_enum(Key::Kp_6),               Key::Kp_6},
    {              nameof::nameof_enum(Key::Kp_7),               Key::Kp_7},
    {              nameof::nameof_enum(Key::Kp_8),               Key::Kp_8},
    {              nameof::nameof_enum(Key::Kp_9),               Key::Kp_9},
    {              nameof::nameof_enum(Key::Kp_0),               Key::Kp_0},
    {         nameof::nameof_enum(Key::Kp_period),          Key::Kp_period},
    {    nameof::nameof_enum(Key::Nonusbackslash),     Key::Nonusbackslash},
    {       nameof::nameof_enum(Key::Application),        Key::Application},
    {             nameof::nameof_enum(Key::Power),              Key::Power},
    {         nameof::nameof_enum(Key::Kp_equals),          Key::Kp_equals},
    {               nameof::nameof_enum(Key::F13),                Key::F13},
    {               nameof::nameof_enum(Key::F14),                Key::F14},
    {               nameof::nameof_enum(Key::F15),                Key::F15},
    {               nameof::nameof_enum(Key::F16),                Key::F16},
    {               nameof::nameof_enum(Key::F17),                Key::F17},
    {               nameof::nameof_enum(Key::F18),                Key::F18},
    {               nameof::nameof_enum(Key::F19),                Key::F19},
    {               nameof::nameof_enum(Key::F20),                Key::F20},
    {               nameof::nameof_enum(Key::F21),                Key::F21},
    {               nameof::nameof_enum(Key::F22),                Key::F22},
    {               nameof::nameof_enum(Key::F23),                Key::F23},
    {               nameof::nameof_enum(Key::F24),                Key::F24},
    {           nameof::nameof_enum(Key::Execute),            Key::Execute},
    {              nameof::nameof_enum(Key::Help),               Key::Help},
    {              nameof::nameof_enum(Key::Menu),               Key::Menu},
    {            nameof::nameof_enum(Key::Select),             Key::Select},
    {              nameof::nameof_enum(Key::Stop),               Key::Stop},
    {             nameof::nameof_enum(Key::Again),              Key::Again},
    {              nameof::nameof_enum(Key::Undo),               Key::Undo},
    {               nameof::nameof_enum(Key::Cut),                Key::Cut},
    {              nameof::nameof_enum(Key::Copy),               Key::Copy},
    {             nameof::nameof_enum(Key::Paste),              Key::Paste},
    {              nameof::nameof_enum(Key::Find),               Key::Find},
    {              nameof::nameof_enum(Key::Mute),               Key::Mute},
    {          nameof::nameof_enum(Key::Volumeup),           Key::Volumeup},
    {        nameof::nameof_enum(Key::Volumedown),         Key::Volumedown},
    {          nameof::nameof_enum(Key::Kp_comma),           Key::Kp_comma},
    {    nameof::nameof_enum(Key::Kp_equalsas400),     Key::Kp_equalsas400},
    {    nameof::nameof_enum(Key::International1),     Key::International1},
    {    nameof::nameof_enum(Key::International2),     Key::International2},
    {    nameof::nameof_enum(Key::International3),     Key::International3},
    {    nameof::nameof_enum(Key::International4),     Key::International4},
    {    nameof::nameof_enum(Key::International5),     Key::International5},
    {    nameof::nameof_enum(Key::International6),     Key::International6},
    {    nameof::nameof_enum(Key::International7),     Key::International7},
    {    nameof::nameof_enum(Key::International8),     Key::International8},
    {    nameof::nameof_enum(Key::International9),     Key::International9},
    {             nameof::nameof_enum(Key::Lang1),              Key::Lang1},
    {             nameof::nameof_enum(Key::Lang2),              Key::Lang2},
    {             nameof::nameof_enum(Key::Lang3),              Key::Lang3},
    {             nameof::nameof_enum(Key::Lang4),              Key::Lang4},
    {             nameof::nameof_enum(Key::Lang5),              Key::Lang5},
    {             nameof::nameof_enum(Key::Lang6),              Key::Lang6},
    {             nameof::nameof_enum(Key::Lang7),              Key::Lang7},
    {             nameof::nameof_enum(Key::Lang8),              Key::Lang8},
    {             nameof::nameof_enum(Key::Lang9),              Key::Lang9},
    {          nameof::nameof_enum(Key::Alterase),           Key::Alterase},
    {            nameof::nameof_enum(Key::Sysreq),             Key::Sysreq},
    {            nameof::nameof_enum(Key::Cancel),             Key::Cancel},
    {             nameof::nameof_enum(Key::Clear),              Key::Clear},
    {             nameof::nameof_enum(Key::Prior),              Key::Prior},
    {           nameof::nameof_enum(Key::Return2),            Key::Return2},
    {         nameof::nameof_enum(Key::Separator),          Key::Separator},
    {               nameof::nameof_enum(Key::Out),                Key::Out},
    {              nameof::nameof_enum(Key::Oper),               Key::Oper},
    {        nameof::nameof_enum(Key::Clearagain),         Key::Clearagain},
    {             nameof::nameof_enum(Key::Crsel),              Key::Crsel},
    {             nameof::nameof_enum(Key::Exsel),              Key::Exsel},
    {             nameof::nameof_enum(Key::Kp_00),              Key::Kp_00},
    {            nameof::nameof_enum(Key::Kp_000),             Key::Kp_000},
    {nameof::nameof_enum(Key::Thousandsseparator), Key::Thousandsseparator},
    {  nameof::nameof_enum(Key::Decimalseparator),   Key::Decimalseparator},
    {      nameof::nameof_enum(Key::Currencyunit),       Key::Currencyunit},
    {   nameof::nameof_enum(Key::Currencysubunit),    Key::Currencysubunit},
    {      nameof::nameof_enum(Key::Kp_leftparen),       Key::Kp_leftparen},
    {     nameof::nameof_enum(Key::Kp_rightparen),      Key::Kp_rightparen},
    {      nameof::nameof_enum(Key::Kp_leftbrace),       Key::Kp_leftbrace},
    {     nameof::nameof_enum(Key::Kp_rightbrace),      Key::Kp_rightbrace},
    {            nameof::nameof_enum(Key::Kp_tab),             Key::Kp_tab},
    {      nameof::nameof_enum(Key::Kp_backspace),       Key::Kp_backspace},
    {              nameof::nameof_enum(Key::Kp_a),               Key::Kp_a},
    {              nameof::nameof_enum(Key::Kp_b),               Key::Kp_b},
    {              nameof::nameof_enum(Key::Kp_c),               Key::Kp_c},
    {              nameof::nameof_enum(Key::Kp_d),               Key::Kp_d},
    {              nameof::nameof_enum(Key::Kp_e),               Key::Kp_e},
    {              nameof::nameof_enum(Key::Kp_f),               Key::Kp_f},
    {            nameof::nameof_enum(Key::Kp_xor),             Key::Kp_xor},
    {          nameof::nameof_enum(Key::Kp_power),           Key::Kp_power},
    {        nameof::nameof_enum(Key::Kp_percent),         Key::Kp_percent},
    {           nameof::nameof_enum(Key::Kp_less),            Key::Kp_less},
    {        nameof::nameof_enum(Key::Kp_greater),         Key::Kp_greater},
    {      nameof::nameof_enum(Key::Kp_ampersand),       Key::Kp_ampersand},
    {   nameof::nameof_enum(Key::Kp_dblampersand),    Key::Kp_dblampersand},
    {    nameof::nameof_enum(Key::Kp_verticalbar),     Key::Kp_verticalbar},
    { nameof::nameof_enum(Key::Kp_dblverticalbar),  Key::Kp_dblverticalbar},
    {          nameof::nameof_enum(Key::Kp_colon),           Key::Kp_colon},
    {           nameof::nameof_enum(Key::Kp_hash),            Key::Kp_hash},
    {          nameof::nameof_enum(Key::Kp_space),           Key::Kp_space},
    {             nameof::nameof_enum(Key::Kp_at),              Key::Kp_at},
    {         nameof::nameof_enum(Key::Kp_exclam),          Key::Kp_exclam},
    {       nameof::nameof_enum(Key::Kp_memstore),        Key::Kp_memstore},
    {      nameof::nameof_enum(Key::Kp_memrecall),       Key::Kp_memrecall},
    {       nameof::nameof_enum(Key::Kp_memclear),        Key::Kp_memclear},
    {         nameof::nameof_enum(Key::Kp_memadd),          Key::Kp_memadd},
    {    nameof::nameof_enum(Key::Kp_memsubtract),     Key::Kp_memsubtract},
    {    nameof::nameof_enum(Key::Kp_memmultiply),     Key::Kp_memmultiply},
    {      nameof::nameof_enum(Key::Kp_memdivide),       Key::Kp_memdivide},
    {      nameof::nameof_enum(Key::Kp_plusminus),       Key::Kp_plusminus},
    {          nameof::nameof_enum(Key::Kp_clear),           Key::Kp_clear},
    {     nameof::nameof_enum(Key::Kp_clearentry),      Key::Kp_clearentry},
    {         nameof::nameof_enum(Key::Kp_binary),          Key::Kp_binary},
    {          nameof::nameof_enum(Key::Kp_octal),           Key::Kp_octal},
    {        nameof::nameof_enum(Key::Kp_decimal),         Key::Kp_decimal},
    {    nameof::nameof_enum(Key::Kp_hexadecimal),     Key::Kp_hexadecimal},
    {             nameof::nameof_enum(Key::Lctrl),              Key::Lctrl},
    {            nameof::nameof_enum(Key::Lshift),             Key::Lshift},
    {              nameof::nameof_enum(Key::Lalt),               Key::Lalt},
    {              nameof::nameof_enum(Key::Lgui),               Key::Lgui},
    {             nameof::nameof_enum(Key::Rctrl),              Key::Rctrl},
    {            nameof::nameof_enum(Key::Rshift),             Key::Rshift},
    {              nameof::nameof_enum(Key::Ralt),               Key::Ralt},
    {              nameof::nameof_enum(Key::Rgui),               Key::Rgui},
    {              nameof::nameof_enum(Key::Mode),               Key::Mode},
    {         nameof::nameof_enum(Key::Audionext),          Key::Audionext},
    {         nameof::nameof_enum(Key::Audioprev),          Key::Audioprev},
    {         nameof::nameof_enum(Key::Audiostop),          Key::Audiostop},
    {         nameof::nameof_enum(Key::Audioplay),          Key::Audioplay},
    {         nameof::nameof_enum(Key::Audiomute),          Key::Audiomute},
    {       nameof::nameof_enum(Key::Mediaselect),        Key::Mediaselect},
    {               nameof::nameof_enum(Key::Www),                Key::Www},
    {              nameof::nameof_enum(Key::Mail),               Key::Mail},
    {        nameof::nameof_enum(Key::Calculator),         Key::Calculator},
    {          nameof::nameof_enum(Key::Computer),           Key::Computer},
    {         nameof::nameof_enum(Key::Ac_search),          Key::Ac_search},
    {           nameof::nameof_enum(Key::Ac_home),            Key::Ac_home},
    {           nameof::nameof_enum(Key::Ac_back),            Key::Ac_back},
    {        nameof::nameof_enum(Key::Ac_forward),         Key::Ac_forward},
    {           nameof::nameof_enum(Key::Ac_stop),            Key::Ac_stop},
    {        nameof::nameof_enum(Key::Ac_refresh),         Key::Ac_refresh},
    {      nameof::nameof_enum(Key::Ac_bookmarks),       Key::Ac_bookmarks},
    {    nameof::nameof_enum(Key::Brightnessdown),     Key::Brightnessdown},
    {      nameof::nameof_enum(Key::Brightnessup),       Key::Brightnessup},
    {     nameof::nameof_enum(Key::Displayswitch),      Key::Displayswitch},
    {    nameof::nameof_enum(Key::Kbdillumtoggle),     Key::Kbdillumtoggle},
    {      nameof::nameof_enum(Key::Kbdillumdown),       Key::Kbdillumdown},
    {        nameof::nameof_enum(Key::Kbdillumup),         Key::Kbdillumup},
    {             nameof::nameof_enum(Key::Eject),              Key::Eject},
    {             nameof::nameof_enum(Key::Sleep),              Key::Sleep},
    {              nameof::nameof_enum(Key::App1),               Key::App1},
    {              nameof::nameof_enum(Key::App2),               Key::App2},
    {       nameof::nameof_enum(Key::Audiorewind),        Key::Audiorewind},
    {  nameof::nameof_enum(Key::Audiofastforward),   Key::Audiofastforward},
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
        for (auto h : handlers_) {
            h(event);
        }

        if (event.type == SDL_QUIT) {
            reg_->event_dispatcher<QuitEvent>().enqueue();
        }

        if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
            KeyboardEvent e;
            e.key = static_cast<Key>(event.key.keysym.scancode);
            e.action = event.key.repeat
                           ? Action::Repeat
                           : (event.type == SDL_KEYDOWN ? Action::Press
                                                        : Action::Release);
            e.mod = static_cast<Modifier>(event.key.keysym.mod);

            reg_->event_dispatcher<KeyboardEvent>().enqueue(e);
        }

        if (event.type == SDL_MOUSEMOTION) {
            MouseMotionEvent e;
            e.position.x = event.motion.x;
            e.position.y = event.motion.y;
            reg_->event_dispatcher<MouseMotionEvent>().enqueue(e);
        }

        if (event.type == SDL_MOUSEBUTTONDOWN ||
            event.type == SDL_MOUSEBUTTONUP) {
            MouseButtonEvent e;
            e.action = event.type == SDL_MOUSEBUTTONDOWN ? Action::Press
                                                         : Action::Release;
            e.btn = static_cast<MouseButtonType>(event.button.button);
            e.clicks = event.button.clicks;

            reg_->event_dispatcher<MouseButtonEvent>().enqueue(e);
        }

        if (event.type == SDL_WINDOWEVENT) {
            if (event.window.event == SDL_WINDOWEVENT_RESIZED ||
                event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                WindowResizeEvent e;
                e.size.x = event.window.data1;
                e.size.y = event.window.data2;

                reg_->event_dispatcher<WindowResizeEvent>().enqueue(e);
            }
        }

        if (event.type == SDL_DROPBEGIN) {
            reg_->event_dispatcher<DropBeginEvent>().enqueue(DropBeginEvent{});
        }

        if (event.type == SDL_DROPCOMPLETE) {
            reg_->event_dispatcher<DropEndEvent>().enqueue(DropEndEvent{});
        }

        if (event.type == SDL_DROPTEXT) {
            DropTextEvent e;
            e.text = event.drop.file;
            SDL_free(event.drop.file);
            reg_->event_dispatcher<DropTextEvent>().enqueue(e);
        }

        if (event.type == SDL_DROPFILE) {
            DropFileEvent e;
            e.path = event.drop.file;
            SDL_free(event.drop.file);
            reg_->event_dispatcher<DropFileEvent>().enqueue(e);
        }
    }
}

void EventPollerInit(gecs::commands cmds) {
    cmds.emplace_resource<EventPoller>(EventPoller{});
    EventPoller::AssociatePollerAndECS(*gWorld->cur_registry());
}

}  // namespace nickel