#pragma once

#include "core/manager.hpp"
#include "pch.hpp"

namespace nickel {

using TimeType = uint64_t;

/**
 * @brief a resource that make you get time elapse between two frame
 */
class Time final {
public:
    static void SetFPS(uint32_t fps) {
        fps_ = fps;
        fpsDuration_ = static_cast<uint64_t>(1000.0 / fps);
    }

    static uint64_t GetFPS() { return fps_; }

    Time();
    Time(const Time&) = delete;
    Time(Time&&) = default;
    Time& operator=(const Time&) = delete;
    Time& operator=(Time&&) = default;

    /**
     * @brief get elapse time between two frame (in millisecond)
     * @return elapsed milliseconds
     * */
    TimeType Elapse() const {
        auto elapse = std::chrono::duration_cast<std::chrono::milliseconds>(elapse_).count();
        return elapse > 0 ? elapse : 1;
    }

    static void Update(gecs::resource<gecs::mut<Time>> timer);

private:
    using clock_t = std::chrono::steady_clock;
    using duration_t = typename clock_t::duration;
    using time_point_t = typename clock_t::time_point;

    static uint64_t fps_;
    static uint64_t fpsDuration_;

    time_point_t curTime_;
    duration_t elapse_;
};

/**
 * @addtogroup resource-manager
 * @{
 */

class Timer;
using TimerHandle = Handle<Timer>;

using TickFunc = std::function<void()>;

/**
 * @brief a timer that tirge function when reach time
 */
class Timer final {
public:
    Timer(TimerHandle handle, TimeType time, TickFunc func, int loop = 0)
        : handle_(handle), dstTime_(time), func_(func), loop_(loop) {}

    Timer(const Timer&) = delete;
    Timer& operator=(const Timer&) = delete;

    void OnTick(TickFunc func) { func_ = func; }

    void Update(const Time& t) {
        if (!isTicking_) return;

        curTime_ += t.Elapse();
        if (curTime_ > dstTime_) {
            curTime_ = 0;
            if (func_) {
                func_();
            }
            if (loop_ > 0) {
                loop_--;
            }
            if (loop_ == 0) {
                Stop();
            }
        }
    }

    void Start() { isTicking_ = true; }

    void Pause() {
        isTicking_ = false;
        curTime_ = 0;
    }

    void Stop() { isTicking_ = false; }

private:
    Handle<Timer> handle_;
    TickFunc func_ = nullptr;
    int loop_ = 0;
    bool isTicking_ = false;
    TimeType curTime_{};
    TimeType dstTime_;
};

class TimerManager : public ResourceManager<Timer> {
public:
    TimerHandle Create(TimeType time, TickFunc func);
};

}  // namespace nickel

/**
 * @}
 */