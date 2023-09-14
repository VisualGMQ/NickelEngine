#pragma once

#include "core/cgmath.hpp"
#include "pch.hpp"
#include "window/event.hpp"

namespace nickel {

void ConnectInput2Event(gecs::event_dispatcher<MouseButtonEvent>,
                        gecs::event_dispatcher<MouseMotionEvent>,
                        gecs::event_dispatcher<KeyboardEvent>);

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
    friend void ConnectInput2Event(gecs::event_dispatcher<MouseButtonEvent>,
                                   gecs::event_dispatcher<MouseMotionEvent>,
                                   gecs::event_dispatcher<KeyboardEvent>);

    Keyboard();

    const KeyButton& Key(Key key) const {
        return buttons_[static_cast<size_t>(key)];
    }

    static void Update(gecs::resource<gecs::mut<Keyboard>> keyboard);

private:
    KeyButton buttons_[static_cast<size_t>(Key::KEY_LAST)];

    static void keyboardEventHandle(const KeyboardEvent& event,
                               gecs::resource<gecs::mut<Keyboard>> keyboard);
};

using MouseButton = Button<MouseButtonType>;

class Mouse {
public:
    friend void ConnectInput2Event(gecs::event_dispatcher<MouseButtonEvent>,
                                   gecs::event_dispatcher<MouseMotionEvent>,
                                   gecs::event_dispatcher<KeyboardEvent>);

    cgmath::Vec2 Position() const { return position_; }

    cgmath::Vec2 Offset() const { return offset_; }

    const MouseButton& LeftBtn() const { return buttons_[0]; }

    const MouseButton& MiddleBtn() const { return buttons_[1]; }

    const MouseButton& RightBtn() const { return buttons_[2]; }

    static void Update(gecs::resource<gecs::mut<Mouse>>);

private:
    cgmath::Vec2 position_;
    cgmath::Vec2 offset_;
    MouseButton buttons_[3] = {MouseButton{MouseButtonType::Left},
                               MouseButton{MouseButtonType::Right},
                               MouseButton{MouseButtonType::Middle}};

    static void mouseMotionEventHandle(const MouseMotionEvent& event,
                                  gecs::resource<gecs::mut<Mouse>> mouse);
    static void mouseBtnEventHandle(const MouseButtonEvent& event,
                               gecs::resource<gecs::mut<Mouse>> mouse);
};

}  // namespace nickel