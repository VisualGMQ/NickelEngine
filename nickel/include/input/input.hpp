#pragma once

#include "pch.hpp"
#include "window/event.hpp"
#include "core/cgmath.hpp"

namespace nickel {

template <typename T>
struct Button {
    T btn;
    bool isPress = false;
    bool lastState = false;

    bool IsPressed() const { return !lastState && isPress; }
    bool IsReleased() const { return lastState && !isPress; }
    bool IsPressing() const { return lastState && isPress; }
    bool IsReleasing() const { return !lastState && !isPress; }
};

using KeyButton = Button<Key>;

class Keyboard {
public:
    Keyboard();
    const KeyButton& Key(Key key) const { return buttons_[static_cast<size_t>(key)]; }

private:
    KeyButton buttons_[static_cast<size_t>(Key::KEY_LAST)];
};

using MouseButton = Button<MouseButtonType>;

class Mouse {
public:
    cgmath::Vec2 Position() const { return position_; }
    cgmath::Vec2 Offset() const { return offset_; }
    const MouseButton& LeftBtn() const { return buttons_[0]; }
    const MouseButton& MiddleBtn() const { return buttons_[1]; }
    const MouseButton& RightBtn() const { return buttons_[2]; }

private:
    cgmath::Vec2 position_;
    cgmath::Vec2 offset_;
    MouseButton buttons_[3] = {MouseButton{MouseButtonType::Left},
                                MouseButton{MouseButtonType::Right},
                                MouseButton{MouseButtonType::Middle}};
};

}