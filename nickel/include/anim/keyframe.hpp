#pragma once

#include "misc/timer.hpp"

namespace nickel {

template <typename T>
struct Interpolation final {
    static T Linear(float t, T a, T b) { return a + (b - a) * t; };
};

struct IKeyFrame {
    using time_type = int;

    virtual ~IKeyFrame() = default;

    time_type timePoint;

    virtual mirrow::drefl::any GetValue() = 0;

    virtual toml::table Save2Toml() const = 0;
};

template <typename T>
struct KeyFrame: public IKeyFrame {
    using interpolate_func = T(*)(float, T, T);
    using value_type = T;

    T value = {};
    interpolate_func interpolate = Interpolation<T>::Linear;

    KeyFrame() = default;

    KeyFrame(value_type value, time_type timePoint,
             interpolate_func interpolate = Interpolation<T>::Linear)
        : value{value}, interpolate{interpolate} {
        this->timePoint = timePoint;
    }

    mirrow::drefl::any GetValue() override {
        return mirrow::drefl::any_make_ref(value);
    }

    toml::table Save2Toml() const override {
        toml::table tbl;
        mirrow::serd::drefl::serialize(
            tbl, mirrow::drefl::any_make_constref(value), "value");
        tbl.emplace("time_point", timePoint);
        if (interpolate == Interpolation<T>::Linear) {
            tbl.emplace("interpolate", std::string("linear"));
        }
        return tbl;
    }

    static KeyFrame LoadFromToml(const toml::table& tbl) {
        KeyFrame frame;

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
};

}  // namespace nickel