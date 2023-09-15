#include "input/input.hpp"

namespace nickel {

InputActionState KeyboardInput::GetActionState(
    const std::string& action) const {
    if (auto it = actions_.find(action); it != actions_.end()) {
        auto key = keyboard_.Key(it->second);
        if (key.IsPressed()) {
            return InputActionState(State::Pressed);
        } else if (key.IsReleased()) {
            return InputActionState(State::Released);
        } else if (key.IsPressing()) {
            return InputActionState(State::Pressing);
        } else {
            return InputActionState(State::Releasing);
        }
    } else {
        LOGW("[Input]: can't find action ", action,
             ", did you define it in game_conf.lua?");
        return InputActionState(State::Unknown);
    }
}

cgmath::Vec2 KeyboardInput::Axis() const {
    cgmath::Vec2 axis;
    if (auto state = GetActionState("left");
        state.IsPressed() || state.IsPressing()) {
        axis.x -= 1;
    }
    if (auto state = GetActionState("right");
        state.IsPressed() || state.IsPressing()) {
        axis.x += 1;
    }
    if (auto state = GetActionState("up");
        state.IsPressed() || state.IsPressing()) {
        axis.y += 1;
    }
    if (auto state = GetActionState("down");
        state.IsPressed() || state.IsPressing()) {
        axis.y -= 1;
    }
    return axis;
}

InputActionState TouchInput::GetActionState(const std::string& action) const {
    Assert(false, "not implemented");
    return InputActionState{State::Unknown};
}

cgmath::Vec2 TouchInput::Axis() const {
    Assert(false, "not implemented");
    return cgmath::Vec2{};
}

}  // namespace nickel