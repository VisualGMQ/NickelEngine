#include "input/input.hpp"
#include "core/profile.hpp"

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

void InputSystemInit(
    gecs::commands cmds,
    gecs::event_dispatcher<MouseButtonEvent> mouseBtnDispatcher,
    gecs::event_dispatcher<MouseMotionEvent> mouseMotionDispatcher,
    gecs::event_dispatcher<KeyboardEvent> keyboardDispatcher) {
    PROFILE_BEGIN();

    auto& keyboard = cmds.emplace_resource<Keyboard>();
    cmds.emplace_resource<Mouse>();
    ConnectInput2Event(mouseBtnDispatcher, mouseMotionDispatcher,
                       keyboardDispatcher);

    std::unordered_map<std::string, Key> actions = {
        {    "up", Key::W},
        {  "down", Key::S},
        {  "left", Key::A},
        { "right", Key::D},
        {"attack", Key::J},
        {  "jump", Key::K}
    };
    // read actions from config file
    // IMPROVE: use serd to auto-parse(now serd don't support serialize
    // unordered_map)
    auto parseResult = toml::parse_file("./nickel-config.toml");
    if (!parseResult) {
        LOGW(log_tag::Config, "parse ",
             "nickel-config.toml"
             " failed, use default actions.\nError: ",
             parseResult.error());
    } else {
        const auto& tbl = parseResult.table();
        if (!tbl["input-action"].is_table()) {
            LOGW(log_tag::Config,
                 "\"input-action\" table not exists, use default actions");
        } else {
            for (auto& [key, value] : tbl) {
                if (!value.is_string()) {
                    LOGW(log_tag::Config, "value of input action ", key,
                         " is not string!");
                } else {
                    actions[std::string(key.str())] =
                        GetKeyFromName(value.as_string()->get());
                }
            }
        }
    }

    cmds.emplace_resource<Input>(
        std::make_unique<KeyboardInput>(keyboard, actions));
}

}  // namespace nickel