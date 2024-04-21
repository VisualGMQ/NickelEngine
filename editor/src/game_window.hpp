#pragma once

#include "nickel.hpp"
#include "widget.hpp"

class GameWindow final: public Window {
public:
    GameWindow();

protected:
    void update() override;

private:
    static constexpr float ScaleFactor = 0.05;
    static constexpr float minScaleFactor = 0.0001;

    float scale_ = 1.0;
    nickel::cgmath::Vec2 offset_;
};