#pragma once
#include <memory>

union SDL_Event;

namespace nickel::input {
class Gamepad {
public:
    using ID = uint32_t;
    static constexpr auto InvalidID = std::numeric_limits<ID>::max();
    
    class Impl;
    class ButtonImpl;

    // NOTE: don't change! copied from SDL 3.20.0
    class Button {
    public:
        enum class Type {
            Invalid = -1,
            South, // Bottom face button (e.g. Xbox A button)
            East,  // Right face button (e.g. Xbox B button)
            West,  // Left face button (e.g. Xbox X button)
            North, // Top face button (e.g. Xbox Y button)
            Back,
            Guide,
            Start,
            LeftStick,
            RightStick,
            LeftShoulder,
            RightShoulder,
            DpadUp,
            DpadDown,
            DpadLeft,
            DpadRight,
            Misc1,
            // Additional button (e.g. Xbox Series X share button, PS5 microphone button, Nintendo Switch Pro capture button, Amazon Luna microphone button, Google Stadia capture button)
            RightPaddle1,
            // Upper or primary paddle, under your right hand (e.g. Xbox Elite paddle P1)
            LeftPaddle1,
            // Upper or primary paddle, under your left hand (e.g. Xbox Elite paddle P3)
            RightPaddle2,
            // Lower or secondary paddle, under your right hand (e.g. Xbox Elite paddle P2)
            LeftPaddle2,
            // Lower or secondary paddle, under your left hand (e.g. Xbox Elite paddle P4)
            Touchpad, // PS4/PS5 touchpad button
            Misc2,    // Additional button
            Misc3,    // Additional button
            Misc4,    // Additional button
            Misc5,    // Additional button
            Misc6,    // Additional button
        };

        explicit Button(const ButtonImpl&);
        Type GetType() const;
        operator bool() const;
        bool IsPressed() const;
        bool IsPressing() const;
        bool IsReleased() const;
        bool IsReleasing() const;

    private:
        const ButtonImpl& m_impl;
    };

    class AxisImpl;

    class Axis {
    public:
        friend class gamepad;

        // NOTE: don't change! copied from SDL 3.20.0
        enum class Type {
            Invalid = -1,
            LeftX,
            LeftY,
            RightX,
            RightY,
            LeftTrigger,
            RightTrigger,
        };

        explicit Axis(const AxisImpl&);
        Type GetType() const;
        double GetValue() const;

    private:
        const AxisImpl& m_impl;
    };

    Gamepad(ID id);
    Axis GetAxis(Axis::Type) const;
    Button GetButton(Button::Type) const;
    ID GetID() const;

    Impl& GetImpl();
    const Impl& GetImpl() const;

private:
    std::unique_ptr<Impl> m_impl;
};

class GamepadManager {
public:
    const std::vector<Gamepad>& GetGamepads() const;
    std::vector<Gamepad>& GetGamepads();
    Gamepad* GetByID(Gamepad::ID id);
    const Gamepad* GetByID(Gamepad::ID id) const;

    GamepadManager();
    ~GamepadManager();
    void HandleEvent(const SDL_Event&);
    void Update();

private:
    class Impl;
    std::unique_ptr<Impl> m_impl;
};
}
