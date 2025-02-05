#pragma once
#include "nickel/common/math/math.hpp"

union SDL_Event;

namespace nickel::input {

class FingerInput {
public:
    class TouchPointImpl;

    class TouchPoint {
    public:
        explicit TouchPoint(const TouchPointImpl&);
        bool IsPressed() const;
        bool IsPressing() const;
        bool IsReleased() const;
        bool IsReleasing() const;
        const Vec2& GetPosition() const;
        const Vec2& GetOffset() const;

    private:
        const TouchPointImpl& m_impl;
    };

    FingerInput();
    ~FingerInput();
    TouchPoint GetFinger(uint32_t idx) const;
    uint32_t FingerMaxCount() const;
    
    void HandleEvent(const SDL_Event&);
    void Update();
    
private:
    class Impl;
    std::unique_ptr<Impl> m_impl;
};

}  // namespace tl