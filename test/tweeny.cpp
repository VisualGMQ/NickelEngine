#include "anim/tweeny.hpp"

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

using namespace nickel;

TEST_CASE("tweeny from 0") {
    using tween_type = BasicTween<float, int>;

    tween_type tween = tween_type::From(0).To(100).During(4).To(300).During(2);
    REQUIRE(tween.Direction() == TweenyDirection::Forward);
    REQUIRE(tween.CurTick() == 0.0);
    REQUIRE(tween.CurValue() == 0.0);
    tween.Step(1);
    REQUIRE(tween.CurTick() == 1.0);
    REQUIRE(tween.CurValue() == 25.0);
    tween.Step(1);
    REQUIRE(tween.CurTick() == 2.0);
    REQUIRE(tween.CurValue() == 50.0);
    tween.Step(1);
    REQUIRE(tween.CurTick() == 3.0);
    REQUIRE(tween.CurValue() == 75.0);
    tween.Step(1);
    REQUIRE(tween.CurTick() == 4.0);
    REQUIRE(tween.CurValue() == 100.0);
    tween.Step(1);
    REQUIRE(tween.CurTick() == 5.0);
    REQUIRE(tween.CurValue() == 200.0);
    tween.Step(1);
    REQUIRE(tween.CurTick() == 6.0);
    REQUIRE(tween.CurValue() == 300.0);
    tween.Step(1);
    REQUIRE(tween.CurTick() == 7.0);
    REQUIRE(tween.CurValue() == 300.0);
}

TEST_CASE("tweeny from 100") {
    using tween_type = BasicTween<float, int>;

    tween_type tween = tween_type::From(100).Timing(3).To(200).During(4);
    REQUIRE(tween.Direction() == TweenyDirection::Forward);
    REQUIRE(tween.CurTick() == 0.0);
    REQUIRE(tween.CurValue() == 100.0);
    tween.Step(1);
    REQUIRE(tween.CurTick() == 1.0);
    REQUIRE(tween.CurValue() == 100.0);
    tween.Step(1);
    REQUIRE(tween.CurTick() == 2.0);
    REQUIRE(tween.CurValue() == 100.0);
    tween.Step(1);
    REQUIRE(tween.CurTick() == 3.0);
    REQUIRE(tween.CurValue() == 100.0);
    tween.Step(1);
    REQUIRE(tween.CurTick() == 4.0);
    REQUIRE(tween.CurValue() == 125);
    tween.Step(1);
    REQUIRE(tween.CurTick() == 5.0);
    REQUIRE(tween.CurValue() == 150);
    tween.Step(1);
    REQUIRE(tween.CurTick() == 6.0);
    REQUIRE(tween.CurValue() == 175);
    tween.Step(1);
    REQUIRE(tween.CurTick() == 7.0);
    REQUIRE(tween.CurValue() == 200.0);
    tween.Step(1);
    REQUIRE(tween.CurTick() == 8.0);
    REQUIRE(tween.CurValue() == 200.0);
}

TEST_CASE("backward tweeny") {
    using tween_type = BasicTween<float, int>;

    tween_type tween = tween_type::From(100).Timing(3).To(200).During(4).Backward();
    tween.SetTick(7);
    REQUIRE(tween.Direction() == TweenyDirection::Backward);
    REQUIRE(tween.CurTick() == 7);
    REQUIRE(tween.CurValue() == 200.0);
    tween.Step(1);
    REQUIRE(tween.CurTick() == 6.0);
    REQUIRE(tween.CurValue() == 175.0);
    tween.Step(1);
    REQUIRE(tween.CurTick() == 5.0);
    REQUIRE(tween.CurValue() == 150.0);
    tween.Step(1);
    REQUIRE(tween.CurTick() == 4.0);
    REQUIRE(tween.CurValue() == 125.0);
    tween.Step(1);
    REQUIRE(tween.CurTick() == 3.0);
    REQUIRE(tween.CurValue() == 100.0);
    tween.Step(1);
    REQUIRE(tween.CurTick() == 2.0);
    REQUIRE(tween.CurValue() == 100);
    tween.Step(1);
    REQUIRE(tween.CurTick() == 1.0);
    REQUIRE(tween.CurValue() == 100.0);
}
