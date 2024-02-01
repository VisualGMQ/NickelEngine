#pragma once

#include "graphics/camera.hpp"
#include "ui/event.hpp"

namespace nickel::ui {

class Context final {
public:
    Camera camera;
    EventRecorder eventRecorder;

    Context(const cgmath::Vec2& size)
        : camera{Camera::CreateOrthoByWindowRegion(size)} {}
};

}  // namespace nickel::ui