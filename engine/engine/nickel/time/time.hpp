#pragma once

#include <chrono>

namespace nickel {

class Time {
public:
    Time();
    uint64_t CurrentTime() const;
    float DeltaTime() const;
    uint32_t FPS() const;
    
    void Update();

private:
    std::chrono::system_clock::time_point m_last_frame_time{};
    std::chrono::system_clock::duration m_delta_time{};
};

}  // namespace nickel