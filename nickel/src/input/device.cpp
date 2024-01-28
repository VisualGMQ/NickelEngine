#include "input/device.hpp"
#include "core/profile.hpp"

namespace nickel {

Keyboard::Keyboard() {
    for (int i = 0; i < static_cast<int>(Key::KEY_LAST); i++) {
        buttons_[i].btn = static_cast<::nickel::Key>(i);
    }
}

void ConnectInput2Event(gecs::event_dispatcher<MouseButtonEvent> btn,
                        gecs::event_dispatcher<MouseMotionEvent> motion,
                        gecs::event_dispatcher<KeyboardEvent> keyboard) {
    motion.sink().add<Mouse::mouseMotionEventHandle>();
    btn.sink().add<Mouse::mouseBtnEventHandle>();
    keyboard.sink().add<Keyboard::keyboardEventHandle>();
}

void HandleInputEvents(gecs::event_dispatcher<MouseButtonEvent> btn,
                       gecs::event_dispatcher<MouseMotionEvent> motion,
                       gecs::event_dispatcher<KeyboardEvent> keyboard) {
    PROFILE_BEGIN();

    btn.update();
    motion.update();
    keyboard.update();
}

void Keyboard::keyboardEventHandle(
    const KeyboardEvent& event, gecs::resource<gecs::mut<Keyboard>> keyboard) {
    auto newKey = static_cast<uint32_t>(event.key);
    if (newKey < static_cast<uint32_t>(Key::KEY_LAST)) {
        auto& key = keyboard->buttons_[newKey];
        key.isPress = event.action != Action::Release;
    }
}

void Mouse::mouseMotionEventHandle(const MouseMotionEvent& event,
                                   gecs::resource<gecs::mut<Mouse>> mouse) {
    mouse->offset_ = event.offset;
    mouse->position_ = event.position;
}

void Mouse::mouseBtnEventHandle(const MouseButtonEvent& event,
                                gecs::resource<gecs::mut<Mouse>> mouse) {
    auto& btn = mouse->buttons_[static_cast<uint32_t>(event.btn) - 1];

    btn.isPress = event.action != Action::Release;
}

void Keyboard::Update(gecs::resource<gecs::mut<Keyboard>> keyboard) {
    PROFILE_BEGIN();

    for (auto& key : keyboard->buttons_) {
        key.lastState = key.isPress;
    }
}

void Mouse::Update(gecs::resource<gecs::mut<Mouse>> mouse) {
    PROFILE_BEGIN();

    for (auto& btn : mouse->buttons_) {
        btn.lastState = btn.isPress;
    }
    mouse->offset_.Set(0, 0);
}

}  // namespace nickel