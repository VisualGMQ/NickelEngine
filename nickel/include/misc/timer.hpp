#pragma once

#include "core/asset.hpp"
#include "core/manager.hpp"
#include "pch.hpp"


namespace nickel {

using TimeType = uint32_t;

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
        auto elapse =
            std::chrono::duration_cast<std::chrono::milliseconds>(elapse_)
                .count();
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

using TimerID = int;

/**
 * @brief [event] will trigger when timer ticked
 */
struct TimerEvent {
    TimerID id;
};

/**
 * @brief a timer that tirge function/timer-event when reach time
 */
class Timer final : public Asset {
public:
    friend class TimerManager;

    bool shouldSendEvent;

    Timer(TimerID id, TimeType time, int loop = 0)
        : id_(id), dstTime_(time), loop_(loop), shouldSendEvent(true) {}

    Timer(const std::filesystem::path& root, const std::filesystem::path& path);

    Timer(const Timer&) = delete;
    Timer& operator=(const Timer&) = delete;

    explicit operator bool() const { return id_.has_value(); }

    auto ID() const { return id_; }

    void Update(const Time& t) {
        if (!isTicking_) return;

        curTime_ += t.Elapse();
        if (curTime_ > dstTime_) {
            curTime_ = 0;
            if (loop_ > 0) {
                loop_--;
            }
            if (shouldSendEvent && id_) {
                gWorld->cur_registry()->event_dispatcher<TimerEvent>().enqueue(
                    TimerEvent{id_.value()});
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

    void Save(const std::filesystem::path& path);

    toml::table Save2Toml() const override {
        toml::table tbl;
        tbl.emplace("path", RelativePath().string());
        return tbl;
    }

private:
    std::optional<TimerID> id_;
    int loop_ = 0;
    bool isTicking_ = false;
    TimeType curTime_{};
    TimeType dstTime_;
};

template <>
std::unique_ptr<Timer> LoadAssetFromToml(const toml::table& tbl,
                                         const std::filesystem::path& root);

class TimerManager : public Manager<Timer> {
public:
    TimerHandle Create(const std::filesystem::path& path, TimerID, TimeType, int loop = 0);
    TimerHandle Load(const std::filesystem::path& path);
};

}  // namespace nickel

/**
 * @}
 */