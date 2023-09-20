#pragma once

#include "anim/tweeny.hpp"
#include "misc/timer.hpp"
#include "misc/transform.hpp"

namespace nickel {

template <typename T>
struct KeyFrame final {
    T value;
    TimeType timePoint;

    KeyFrame Create(T value, TimeType duration) {
        return KeyFrame{value, duration};
    }
};

template <typename T>
class BasicAnimation final {
public:
    using keyframe_type = KeyFrame<T>;

    BasicAnimation(const std::vector<keyframe_type>& frames) {
        cvtFrames2Tweens(frames);
    }

    auto Direction() const { return tween_.direction_; }

    bool IsFinish() const { return tween_.IsFinish(); }

    bool IsStart() const { return tween_.IsStart(); }

    auto CurTick() const { return tween_.CurTick(); }

    auto CurValue() const { return tween_.CurValue(); }

    void Step(T step) { return tween_.Step(step); }

    bool Empty() const { return tween_.Empty(); }

    auto& CurKeyPoint() const { return tween_.CurKeyPoint(); }

private:
    BasicTween<T, TimeType> tween_;

    void cvtFrames2Tweens(const std::vector<keyframe_type>& frames) {
        T startValue = T{};
        TimeType timePoint = 0;
        for (auto& frame : frames) {
            tween_.From(startValue)
                .To(frame.value)
                .During(frame.timePoint - timePoint);
        }
    }
};

using Animation = BasicAnimation<float>;

template <typename T>
class BasicAnimationPlayer final {
public:
    using animation_type = BasicAnimation<T>;
    using container_type = std::vector<animation_type>;

    BasicAnimationPlayer(const container_type& anims) : anims_(anims) {}
    BasicAnimationPlayer(container_type&& anims) : anims_(std::move(anims)) {}

    auto& Animations() const { return anims_; }

    void Step(TimeType step) {
        for (auto& anim : anims_) {
            anim.Step(step);
        }
    }

private:
    TimeType curTime_ = 0;
    container_type anims_;
};

using AnimationPlayer = BasicAnimationPlayer<float>;

}  // namespace nickel