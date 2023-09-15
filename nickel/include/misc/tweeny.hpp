//! @file tweeny.hpp
//! @brief a small tween lib for game and gui interaction

#pragma once

#include <functional>
#include <cmath>
#include <algorithm>
#include <vector>
#include <utility>

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

template <typename T>
class BasicTween final {
public:
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

    BasicTween& Timing(float time) {
        if (!keyPoints_.empty()) {
            keyPoints_.back().time = time;
        }
        return *this;
    }

    BasicTween& During(float during) {
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

    BasicTween& Backward() {
        direction_ = TweenyDirection::Backward;
        return *this;
    }

    void Step(T step) {
        if (keyPoints_.size() < 2) {
            return;
        }

        curDur_ += static_cast<int>(direction_) * step;
        const auto& from = keyPoints_[curPoint_];
        const auto& to = keyPoints_[curPoint_ + 1];
        if (curDur_ < from.time) {
            curPoint_--;
        } else if (curDur_ > to.time) {
            curPoint_++;
        }

        if (curPoint_ < 0) {
            curPoint_ = 0;
            curDur_ = keyPoints_[curPoint_].time;
            if (loop_ != 0) {
                curPoint_ = keyPoints_.size() - 2;
                curDur_ = keyPoints_.back().time;
                loop_ -= loop_ < 0 ? 0 : 1;
            }
        } else if (curPoint_ + 2 >= keyPoints_.size() && curDur_ >= to.time) {
            curPoint_ = keyPoints_.size() - 2;
            curDur_ = keyPoints_.back().time;
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

        const auto& from = keyPoints_[curPoint_];
        const auto& to = keyPoints_[curPoint_ + 1];
        return func_(
            static_cast<float>(curDur_ - from.time) / (to.time - from.time),
            from.value, to.value);
    }

    T CurTick() const { return curDur_; }

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

private:
    struct KeyPoint {
        T value;
        float time;
    };

    TweenFunc<T> func_ = Easing<T>::Linear;
    std::vector<KeyPoint> keyPoints_;
    int loop_ = 0;
    TweenyDirection direction_ = TweenyDirection::Forward;
    T curDur_ = 0;
    size_t curPoint_ = 0;
};

using Tween = BasicTween<float>;

template <typename... Ts>
class Tweeny {
public:
    static auto From(Ts... args) {
        Tweeny<Ts...> tw;
        tw.tweens_ =
            std::make_tuple(BasicTween<Ts>::From(std::forward<Ts>(args))...);
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
    using Container = std::tuple<BasicTween<Ts>...>;
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