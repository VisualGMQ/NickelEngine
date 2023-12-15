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

    toml::table Save2Toml() const {
        toml::table tbl;
        mirrow::serd::drefl::serialize(
            tbl, mirrow::drefl::any_make_constref(value), "value");
        tbl.emplace("time_point", timePoint);
        // tbl.emplace("interpolate", );
        return tbl;
    }

    static BasicKeyFrame LoadFromToml(const toml::table& tbl) {
        BasicKeyFrame frame;

        if (auto node = tbl.get("value"); node) {
            auto ref = mirrow::drefl::any_make_ref(frame.value);
            mirrow::serd::drefl::deserialize(ref, *node);
        }

        if (auto node = tbl.get("time_point"); node && node->is_integer()) {
            frame.timePoint = node->as_integer()->get();
        }

        frame.interpolate = Interpolation<T>::Linear;

        return frame;
    }

private:
};

template <typename T>
using KeyFrame = BasicKeyFrame<T, TimeType>;

}  // namespace nickel