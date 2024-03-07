#pragma once

#include "common/cgmath.hpp"
#include "video/event.hpp"
#include "common/ecs.hpp"

namespace nickel {

void ConnectInput2Event(gecs::event_dispatcher<MouseButtonEvent>,
                        gecs::event_dispatcher<MouseMotionEvent>,
                        gecs::event_dispatcher<MouseWheelEvent>,
                        gecs::event_dispatcher<KeyboardEvent>);

void HandleInputEvents(gecs::event_dispatcher<MouseButtonEvent>,
                       gecs::event_dispatcher<MouseMotionEvent>,
                       gecs::event_dispatcher<MouseWheelEvent>,
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

    bool IsPress() const { return IsPressed() || IsPressing(); }
    bool IsRelease() const { return IsReleased() || IsReleasing(); }
};

using KeyButton = Button<Key>;

class Keyboard {
public:
    friend void ConnectInput2Event(gecs::event_dispatcher<MouseButtonEvent>,
                                   gecs::event_dispatcher<MouseMotionEvent>,
                                   gecs::event_dispatcher<MouseWheelEvent>,
                                   gecs::event_dispatcher<KeyboardEvent>);

    Keyboard();

    const KeyButton& Key(Key key) const {
        return buttons_[static_cast<size_t>(key)];
    }

    static void Update(gecs::resource<gecs::mut<Keyboard>> keyboard);

private:
    KeyButton buttons_[static_cast<size_t>(Key::KEY_LAST)];

    static void keyboardEventHandle(
        const KeyboardEvent& event,
        gecs::resource<gecs::mut<Keyboard>> keyboard);
};

using MouseButton = Button<MouseButtonType>;

class Mouse {
public:
    friend void ConnectInput2Event(gecs::event_dispatcher<MouseButtonEvent>,
                                   gecs::event_dispatcher<MouseMotionEvent>,
                                   gecs::event_dispatcher<MouseWheelEvent>,
                                   gecs::event_dispatcher<KeyboardEvent>);

    cgmath::Vec2 Position() const { return position_; }

    cgmath::Vec2 Offset() const { return offset_; }

    const MouseButton& LeftBtn() const { return buttons_[0]; }

    const MouseButton& RightBtn() const { return buttons_[1]; }

    const MouseButton& MiddleBtn() const { return buttons_[2]; }

    cgmath::Vec2 WheelOffset() const { return wheel_; }

    static void Update(gecs::resource<gecs::mut<Mouse>>);

private:
    cgmath::Vec2 position_;
    cgmath::Vec2 offset_;
    cgmath::Vec2 wheel_;
    MouseButton buttons_[3] = {MouseButton{MouseButtonType::Left},
                               MouseButton{MouseButtonType::Middle},
                               MouseButton{MouseButtonType::Right}};

    static void mouseMotionEventHandle(const MouseMotionEvent& event,
                                       gecs::resource<gecs::mut<Mouse>> mouse);
    static void mouseBtnEventHandle(const MouseButtonEvent& event,
                                    gecs::resource<gecs::mut<Mouse>> mouse);
    static void mouseWheelEventHandle(const MouseWheelEvent& event,
                                      gecs::resource<gecs::mut<Mouse>> mouse);
};

}  // namespace nickel