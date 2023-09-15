#include "input/device.hpp"

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

void Keyboard::keyboardEventHandle(const KeyboardEvent& event,
                              gecs::resource<gecs::mut<Keyboard>> keyboard) {
    auto& key = keyboard->buttons_[static_cast<uint32_t>(event.key)];
    key.lastState = key.isPress;
    key.isPress = event.action != Action::Release ? true : false;
}

void Mouse::mouseMotionEventHandle(const MouseMotionEvent& event,
                              gecs::resource<gecs::mut<Mouse>> mouse) {
    mouse->offset_ = event.position - mouse->offset_;
    mouse->offset_ = event.position;
}

void Mouse::mouseBtnEventHandle(const MouseButtonEvent& event,
                           gecs::resource<gecs::mut<Mouse>> mouse) {
    auto& btn = mouse->buttons_[static_cast<uint32_t>(event.btn)];

    btn.lastState = btn.isPress;
    btn.isPress = event.action != Action::Release ? true : false;
}

void Keyboard::Update(gecs::resource<gecs::mut<Keyboard>> keyboard) {
    for (auto& key : keyboard->buttons_)  {
        key.lastState = key.isPress;
    }
}

void Mouse::Update(gecs::resource<gecs::mut<Mouse>> mouse) {
    for (auto& btn : mouse->buttons_) {
        btn.lastState = btn.isPress;
    }
}

}  // namespace nickel