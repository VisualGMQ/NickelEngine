#pragma once

#include "core/cgmath.hpp"
#include "input/device.hpp"
#include "pch.hpp"

namespace nickel {

enum class State {
    Unknown,
    Pressed,
    Pressing,
    Released,
    Releasing,
};

struct InputActionState final {
    InputActionState(State state) : state_(state) {}

    bool IsPressed() const { return state_ == State::Pressed; }

    bool IsPressing() const { return state_ == State::Pressing; }

    bool IsReleased() const { return state_ == State::Released; }

    bool IsReleasing() const { return state_ == State::Releasing; }

private:
    State state_ = State::Unknown;
};

/**
 * @brief an input wrapper for hidding different between keyboard and touch
 * devices
 */
class BasicInput {
public:
    virtual InputActionState GetActionState(
        const std::string& action) const = 0;
    virtual cgmath::Vec2 Axis() const = 0;

    virtual ~BasicInput() = default;
};

/**
 * @brief input for keyboard devices
 */
class KeyboardInput : public BasicInput {
public:
    KeyboardInput(Keyboard& keyboard,
                  std::unordered_map<std::string, Key>&& actions)
        : keyboard_(keyboard), actions_(std::move(actions)) {}

    KeyboardInput(Keyboard& keyboard,
                  const std::unordered_map<std::string, Key>& actions)
        : keyboard_(keyboard), actions_(actions) {}

    InputActionState GetActionState(const std::string& action) const override;
    cgmath::Vec2 Axis() const override;

private:
    std::unordered_map<std::string, Key> actions_;
    Keyboard& keyboard_;
};

/**
 * @brief  input for touchable devices(like Phone)
 */
class TouchInput : public BasicInput {
    InputActionState GetActionState(const std::string& action) const override;
    cgmath::Vec2 Axis() const override;
};

/**
 * @brief a wrapper for RawInput, used for bind in Lua
 */
class Input final {
public:
    Input() = default;

    Input(std::unique_ptr<BasicInput>&& input) : input_(std::move(input)) {}

    InputActionState GetActionState(const std::string& action) const {
        Assert(input_, "input is nullptr");
        return input_->GetActionState(action);
    }

    cgmath::Vec2 Axis() const {
        Assert(input_, "input is nullptr");
        return input_->Axis();
    }

private:
    std::unique_ptr<BasicInput> input_;
};

void InputSystemInit(
    gecs::commands cmds,
    gecs::event_dispatcher<MouseButtonEvent> mouseBtnDispatcher,
    gecs::event_dispatcher<MouseMotionEvent> mouseMotionDispatcher,
    gecs::event_dispatcher<KeyboardEvent> keyboardDispatcher);

}  // namespace nickel