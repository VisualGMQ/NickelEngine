#pragma once

#include "nickel.hpp"
#include "widget.hpp"
#include "gizmos.hpp"

class GameWindow final: public Window {
public:
    GameWindow();
    void Update() override;

private:
    static constexpr float ScaleFactor = 0.05;
    static constexpr float minScaleFactor = 0.0001;

    float scale_ = 1.0;
    nickel::cgmath::Vec2 offset_;

    std::unique_ptr<nickel::gogl::RenderBuffer> rbo_;
    std::unique_ptr<nickel::gogl::Framebuffer> fbo_;
    std::unique_ptr<nickel::gogl::Texture> texture_;

    SRTGizmos srtGizmos_;
};