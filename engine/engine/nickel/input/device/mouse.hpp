#pragma once
#include "nickel/common/math/math.hpp"

union SDL_Event;

namespace nickel::input {
class Mouse {
public:
    class ButtonImpl;

    class Button {
    public:
        // NOTE: don't change! copied from SDL 3.20.0
        enum class Type {
            Left = 1,
            Middle = 2,
            Right = 3,
            X1 = 4,
            X2 = 5,
        };

        Button(const ButtonImpl&);
        Type GetType() const;
        bool IsPressed() const;
        bool IsPressing() const;
        bool IsReleased() const;
        bool IsReleasing() const;

    private:
        const ButtonImpl& m_impl;
    };

    Mouse();
    ~Mouse();
    const Button& GetButton(Button::Type) const;
    const Vec2& GetPosition() const;
    const Vec2& GetOffset() const;
    void HandleEvent(const SDL_Event&);
    void Update();

private:
    class Impl;
    std::unique_ptr<Impl> m_impl;
};
} // namespace tl
