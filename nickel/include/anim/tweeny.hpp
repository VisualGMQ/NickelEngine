//! @file tweeny.hpp
//! @brief a small tween lib for game and gui interaction

#pragma once

#include "pch.hpp"

namespace nickel {

enum class TweenyDirection {
    Forward = 1,
    Backward = -1,
};

template <typename T>
using TweenFunc = std::function<T(float, T, T)>;

template <typename T>
struct Easing {
    inline static TweenFunc<T> Linear = [](float t, T a, T b) {
        return a + (b - a) * t;
    };
};

template <typename T, typename TimeType>
class BasicTween final {
public:
    using time_type = TimeType;
    using value_type = T;

    struct KeyPoint {
        T value;
        time_type time;

        static KeyPoint Create(T value, time_type time) {
            return {value, time};
        }
    };

    static BasicTween From(T from) {
        BasicTween tweeny;
        tweeny.keyPoints_.push_back({from, 0});
        return tweeny;
    }

    BasicTween& To(T value) {
        keyPoints_.push_back({value, 0});
        return *this;
    }

    BasicTween& Via(TweenFunc<T> func) {
        func_ = func;
        return *this;
    }

    BasicTween& Loop(int loop) {
        loop_ = loop;
        return *this;
    }

    BasicTween& Timing(time_type time) {
        if (!keyPoints_.empty()) {
            keyPoints_.back().time = time;
        }
        return *this;
    }

    BasicTween& During(time_type during) {
        if (keyPoints_.size() >= 2) {
            keyPoints_.back().time =
                keyPoints_[keyPoints_.size() - 2].time + during;
        }
        return *this;
    }

    auto Direction() const { return direction_; }

    BasicTween& Forward() {
        direction_ = TweenyDirection::Forward;
        return *this;
    }

    int StartTick() const {
        return keyPoints_.empty() ? 0 : keyPoints_[0].time;
    }

    int EndTick() const {
        return keyPoints_.empty() ? 0 : keyPoints_.back().time;
    }

    void SetTick(time_type t) {
        curDur_ = std::clamp<int>(t, StartTick(), EndTick());
        if (t <= StartTick()) {
            curPoint_ = 0;
            return ;
        }
        if (t >= EndTick()) {
            curPoint_ = keyPoints_.size() - 1;
            return ;
        }

        for (int i = 0; i < keyPoints_.size() - 1; i++) {
            auto start = keyPoints_[i].time;
            auto end = keyPoints_[i + 1].time;
            if (t >= start && t < end) {
                curPoint_ = i;
                return ;
            }
        }
    }

    BasicTween& Backward() {
        direction_ = TweenyDirection::Backward;
        return *this;
    }

    void Step(time_type step) {
        if (keyPoints_.size() < 2) {
            return;
        }

        curDur_ += static_cast<int>(direction_) * step;

        const KeyPoint *from = nullptr, *to = nullptr;

        if (curPoint_ + 1 >= keyPoints_.size()) {
            from = &keyPoints_[curPoint_];
            to = &keyPoints_[curPoint_];
        } else {
            from = &keyPoints_[curPoint_];
            to = &keyPoints_[curPoint_ + 1];
        }

        if (curDur_ < from->time) {
            curPoint_--;
        } else if (curDur_ > to->time) {
            curPoint_++;
        }

        if (curPoint_ < 0) {
            curPoint_ = 0;
            if (loop_ != 0) {
                curPoint_ = keyPoints_.size() - 2;
                curDur_ = keyPoints_.back().time;
                loop_ -= loop_ < 0 ? 0 : 1;
            }
        } else if (curPoint_ + 2 >= keyPoints_.size() && curDur_ >= keyPoints_.back().time) {
            curPoint_ = keyPoints_.size() - 2;
            if (loop_ != 0) {
                curPoint_ = 0;
                curDur_ = 0;
                loop_ -= loop_ < 0 ? 0 : 1;
            }
        }
    }

    T CurValue() const {
        if (keyPoints_.size() < 2) {
            if (keyPoints_.size() == 1) {
                return keyPoints_.back().value;
            } else {
                return T{};
            }
        }

        if (curPoint_ >= keyPoints_.size() - 1) {
            return keyPoints_.back().value;
        }

        if (curPoint_ < 0) {
            return keyPoints_[0].value;
        }

        const auto& from = keyPoints_[curPoint_];
        const auto& to = keyPoints_[curPoint_ + 1];
        auto t = std::clamp<TimeType>(curDur_, from.time, to.time);
        return func_(
            static_cast<float>(t - from.time) / (to.time - from.time),
            from.value, to.value);
    }

    TimeType CurTick() const { return curDur_; }

    bool IsStart() const {
        if (direction_ == TweenyDirection::Forward) {
            return curPoint_ == 0 && curDur_ == keyPoints_.front().time;
        } else {
            return curPoint_ == keyPoints_.size() - 2 &&
                   curDur_ == keyPoints_.back().time;
        }
    }

    bool IsFinish() const {
        if (direction_ == TweenyDirection::Forward) {
            return curPoint_ == keyPoints_.size() - 2 && loop_ == 0 &&
                   curDur_ == keyPoints_.back().time;
        } else {
            return curPoint_ == 0 && loop_ == 0 &&
                   curDur_ == keyPoints_.front().time;
        }
    }

    auto& KeyPoints() const {
        return keyPoints_;
    }

    auto& CurKeyPoint() const {
        return keyPoints_[curPoint_];
    }

    bool Empty() const {
        return keyPoints_.empty();
    }

private:
    TweenFunc<T> func_ = Easing<T>::Linear;
    std::vector<KeyPoint> keyPoints_;
    int loop_ = 0;
    TweenyDirection direction_ = TweenyDirection::Forward;
    time_type curDur_ = 0;
    int curPoint_ = 0;
};

using Tween = BasicTween<float, int>;

template <typename TimeType, typename... Ts>
class Tweeny {
public:
    static auto From(Ts... args) {
        Tweeny<Ts...> tw;
        tw.tweens_ =
            std::make_tuple(BasicTween<Ts, TimeType>::From(std::forward<Ts>(args))...);
        return tw;
    }

    auto& To(Ts... args) {
        to<0, Ts...>(std::forward<Ts>(args)...);
        return *this;
    }

    template <typename... Funcs>
    auto& Via(Funcs&&... args) {
        if constexpr (sizeof...(args) == 1) {
            viaOne<0, Funcs...>(std::forward<Funcs>(args)...);
        } else {
            via<0, Funcs...>(std::forward<Funcs>(args)...);
        }
        return *this;
    }

    auto CurValue() {
        return curValue(std::make_index_sequence<sizeof...(Ts)>{});
    }

    auto& During(float duration) {
        during<0>(duration);
        return *this;
    }

    auto& OnFinish(std::function<void(void)> f) {
        onFinish_ = f;
        return *this;
    }

    auto& OnStart(std::function<void(void)> f) {
        onStart_ = f;
        return *this;
    }

    bool IsFinish() const { return isFinished_; }

    void Step(float step) {
        this->step<0>(step);
        if (std::get<0>(tweens_).IsStart()) {
            onStart_();
        }
        if (std::get<0>(tweens_).IsFinish() && !isFinished_) {
            isFinished_ = true;
            onFinish_();
        }
    }

private:
    using Container = std::tuple<BasicTween<Ts, float>...>;
    Container tweens_;
    std::function<void(void)> onFinish_;
    std::function<void(void)> onStart_;
    bool isFinished_ = false;

    template <size_t Idx, typename T, typename... Remains>
    void to(T&& value, Remains&&... args) {
        std::get<Idx>(tweens_).To(std::forward<T>(value));

        if constexpr (sizeof...(args) > 0) {
            to<Idx + 1>(std::forward<T>(args)...);
        }
    }

    template <size_t Idx, typename T, typename... Remains>
    void via(T&& value, Remains&&... args) {
        std::get<Idx>(tweens_).Via(std::forward<T>(value));

        if constexpr (sizeof...(args) > 0) {
            via<Idx + 1>(std::forward<T>(args)...);
        }
    }

    template <size_t Idx, typename T>
    void viaOne(T&& value) {
        std::get<Idx>(tweens_).Via(std::forward<T>(value));

        if constexpr (Idx < std::tuple_size_v<Container>) {
            via<Idx + 1>(std::forward<T>(value));
        }
    }

    template <size_t Idx>
    void during(float duration) {
        std::get<Idx>(tweens_).During(duration);

        if constexpr (Idx < std::tuple_size_v<Container> - 1) {
            during<Idx + 1>(duration);
        }
    }

    template <size_t Idx>
    void step(float step) {
        std::get<Idx>(tweens_).Step(step);

        if constexpr (Idx < std::tuple_size_v<Container> - 1) {
            this->step<Idx + 1>(step);
        }
    }

    template <size_t... Idxs>
    auto curValue(std::index_sequence<Idxs...>) {
        return std::array{(std::get<Idxs>(tweens_).CurValue())...};
    }
};

}  // namespace nickel