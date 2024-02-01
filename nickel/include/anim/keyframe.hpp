#pragma once

#include "common/timer.hpp"

namespace nickel {

template <typename T>
struct Interpolation final {

    static T Linear(float t, T a, T b) { return a + (b - a) * t; };
    static T Discrete(float t, T a, T b) { return a; };
};

enum class InterpolateType {
    Linear,
    Discrete,
};

struct IKeyFrame {
    using time_type = int;

    virtual ~IKeyFrame() = default;

    time_type timePoint;

    virtual mirrow::drefl::any GetValue() = 0;

    virtual toml::table Save2Toml() const = 0;

    InterpolateType GetInterpolateType() { return interpType_; }
    virtual void SetInterpolateType(InterpolateType type) { interpType_ = type; }

private:
    InterpolateType interpType_ = InterpolateType::Linear;
};

template <typename T>
struct KeyFrame : public IKeyFrame {
    template <typename>
    friend class AnimationTrack;

    using interpolate_func = T (*)(float, T, T);
    using value_type = T;

    T value = {};

    KeyFrame() = default;

    KeyFrame(value_type value, time_type timePoint,
             interpolate_func interpolate = Interpolation<T>::Linear)
        : value{value}, interpolate_{interpolate} {
        this->timePoint = timePoint;
    }

    mirrow::drefl::any GetValue() override {
        return mirrow::drefl::any_make_ref(value);
    }

    void SetInterpolateType(InterpolateType type) override {
        IKeyFrame::SetInterpolateType(type);
        switch (type) {
            case InterpolateType::Linear:
                interpolate_ = Interpolation<T>::Linear;
                break;
            case InterpolateType::Discrete:
                interpolate_ = Interpolation<T>::Discrete;
                break;
        }
    }

    toml::table Save2Toml() const override {
        toml::table tbl;
        mirrow::serd::drefl::serialize(
            tbl, mirrow::drefl::any_make_constref(value), "value");
        tbl.emplace("time_point", timePoint);
        if (interpolate_ == Interpolation<T>::Linear) {
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

        frame.interpolate_ = Interpolation<T>::Linear;

        return frame;
    }

private:
    interpolate_func interpolate_ = Interpolation<T>::Linear;
};

}  // namespace nickel