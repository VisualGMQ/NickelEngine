#pragma once

#include "misc/timer.hpp"

namespace nickel {

template <typename T>
struct Interpolation final {
    static T Linear(float t, T a, T b) { return a + (b - a) * t; };
};

template <typename T, typename TimeType>
struct BasicKeyFrame {
    using interpolate_func = std::function<T(float, T, T)>;
    using value_type = T;
    using time_type = TimeType;

    T value;
    TimeType timePoint;
    interpolate_func interpolate;

    static BasicKeyFrame Create(
        T value, TimeType t,
        interpolate_func interp = Interpolation<T>::Linear) {
        return {value, t, interp};
    }
};

template <typename T>
using KeyFrame = BasicKeyFrame<T, TimeType>;

}  // namespace nickel