#pragma once

#include "window/event.hpp"

namespace nickel {

typename gecs::world::registry_type* EventPoller::reg_ = nullptr;

const std::unordered_map<std::string_view, Key> gKeynameMap = {
    {        "space",         Key::SPACE},
    {   "apostrophe",    Key::APOSTROPHE},
    {        "comma",         Key::COMMA},
    {        "minus",         Key::MINUS},
    {       "period",        Key::PERIOD},
    {        "slash",         Key::SLASH},
    {         "num0",          Key::NUM0},
    {         "num1",          Key::NUM1},
    {         "num2",          Key::NUM2},
    {         "num3",          Key::NUM3},
    {         "num4",          Key::NUM4},
    {         "num5",          Key::NUM5},
    {         "num6",          Key::NUM6},
    {         "num7",          Key::NUM7},
    {         "num8",          Key::NUM8},
    {         "num9",          Key::NUM9},
    {    "semicolon",     Key::SEMICOLON},
    {        "equal",         Key::EQUAL},
    {            "a",             Key::A},
    {            "b",             Key::B},
    {            "c",             Key::C},
    {            "d",             Key::D},
    {            "e",             Key::E},
    {            "f",             Key::F},
    {            "g",             Key::G},
    {            "h",             Key::H},
    {            "i",             Key::I},
    {            "j",             Key::J},
    {            "k",             Key::K},
    {            "l",             Key::L},
    {            "m",             Key::M},
    {            "n",             Key::N},
    {            "o",             Key::O},
    {            "p",             Key::P},
    {            "q",             Key::Q},
    {            "r",             Key::R},
    {            "s",             Key::S},
    {            "t",             Key::T},
    {            "u",             Key::U},
    {            "v",             Key::V},
    {            "w",             Key::W},
    {            "x",             Key::X},
    {            "y",             Key::Y},
    {            "z",             Key::Z},
    { "left_bracket",  Key::LEFT_BRACKET},
    {    "backslash",     Key::BACKSLASH},
    {"right_bracket", Key::RIGHT_BRACKET},
    { "grave_accent",  Key::GRAVE_ACCENT},
    {      "world_1",       Key::WORLD_1},
    {      "world_2",       Key::WORLD_2},
    {       "escape",        Key::ESCAPE},
    {        "enter",         Key::ENTER},
    {          "tab",           Key::TAB},
    {    "backspace",     Key::BACKSPACE},
    {       "insert",        Key::INSERT},
    {       "delete",        Key::DELETE},
    {        "right",         Key::RIGHT},
    {         "left",          Key::LEFT},
    {         "down",          Key::DOWN},
    {           "up",            Key::UP},
    {      "page_up",       Key::PAGE_UP},
    {    "page_down",     Key::PAGE_DOWN},
    {         "home",          Key::HOME},
    {          "end",           Key::END},
    {    "caps_lock",     Key::CAPS_LOCK},
    {  "scroll_lock",   Key::SCROLL_LOCK},
    {     "num_lock",      Key::NUM_LOCK},
    { "print_screen",  Key::PRINT_SCREEN},
    {        "pause",         Key::PAUSE},
    {           "f1",            Key::F1},
    {           "f2",            Key::F2},
    {           "f3",            Key::F3},
    {           "f4",            Key::F4},
    {           "f5",            Key::F5},
    {           "f6",            Key::F6},
    {           "f7",            Key::F7},
    {           "f8",            Key::F8},
    {           "f9",            Key::F9},
    {          "f10",           Key::F10},
    {          "f11",           Key::F11},
    {          "f12",           Key::F12},
    {          "f13",           Key::F13},
    {          "f14",           Key::F14},
    {          "f15",           Key::F15},
    {          "f16",           Key::F16},
    {          "f17",           Key::F17},
    {          "f18",           Key::F18},
    {          "f19",           Key::F19},
    {          "f20",           Key::F20},
    {          "f21",           Key::F21},
    {          "f22",           Key::F22},
    {          "f23",           Key::F23},
    {          "f24",           Key::F24},
    {          "f25",           Key::F25},
    {         "kp_0",          Key::KP_0},
    {         "kp_1",          Key::KP_1},
    {         "kp_2",          Key::KP_2},
    {         "kp_3",          Key::KP_3},
    {         "kp_4",          Key::KP_4},
    {         "kp_5",          Key::KP_5},
    {         "kp_6",          Key::KP_6},
    {         "kp_7",          Key::KP_7},
    {         "kp_8",          Key::KP_8},
    {         "kp_9",          Key::KP_9},
    {   "kp_decimal",    Key::KP_DECIMAL},
    {    "kp_divide",     Key::KP_DIVIDE},
    {  "kp_multiply",   Key::KP_MULTIPLY},
    {  "kp_subtract",   Key::KP_SUBTRACT},
    {       "kp_add",        Key::KP_ADD},
    {     "kp_enter",      Key::KP_ENTER},
    {     "kp_equal",      Key::KP_EQUAL},
    {   "left_shift",    Key::LEFT_SHIFT},
    { "left_control",  Key::LEFT_CONTROL},
    {     "left_alt",      Key::LEFT_ALT},
    {   "left_super",    Key::LEFT_SUPER},
    {  "right_shift",   Key::RIGHT_SHIFT},
    {"right_control", Key::RIGHT_CONTROL},
    {    "right_alt",     Key::RIGHT_ALT},
    {  "right_super",   Key::RIGHT_SUPER},
    {         "menu",          Key::MENU},
};

Key GetKeyFromName(std::string_view name) {
    if (auto it = gKeynameMap.find(name); it != gKeynameMap.end()) {
        return it->second;
    }
    return Key::UNKNOWN;
}

void EventPoller::Poll() const {
    glfwPollEvents();
}

void EventPoller::ConnectPoller2Events(Window& window) {
    glfwSetKeyCallback((GLFWwindow*)window.Raw(), EventPoller::keyCallback);
    glfwSetCursorPosCallback((GLFWwindow*)window.Raw(),
                             EventPoller::mouseMotionCallback);
    glfwSetMouseButtonCallback((GLFWwindow*)window.Raw(),
                               EventPoller::mouseBtnCallback);
}

void EventPoller::keyCallback(GLFWwindow* window, int key, int scancode,
                              int action, int mods) {
    KeyboardEvent event;

    event.action = static_cast<Action>(action);
    event.key = static_cast<Key>(key);
    event.mod = static_cast<Modifier>(mods);

    EventPoller::getRegistry().event_dispatcher<KeyboardEvent>().enqueue(event);
}

void EventPoller::mouseBtnCallback(GLFWwindow* window, int button, int action,
                                   int mods) {
    MouseButtonEvent event;

    event.action = static_cast<Action>(action);
    event.btn = static_cast<MouseButtonType>(button);
    event.mod = static_cast<Modifier>(mods);

    EventPoller::getRegistry().event_dispatcher<MouseButtonEvent>().enqueue(
        event);
}

void EventPoller::mouseMotionCallback(GLFWwindow* window, double xpos,
                                      double ypos) {
    MouseMotionEvent event;
    event.position.x = static_cast<float>(xpos);
    event.position.y = static_cast<float>(ypos);

    EventPoller::getRegistry().event_dispatcher<MouseMotionEvent>().enqueue(
        event);
}

}  // namespace nickel