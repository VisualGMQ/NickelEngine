#pragma once

#include "pch.hpp"
#include "renderer/camera.hpp"
#include "ui/event.hpp"

namespace nickel::ui {

class Context final {
public:
    Camera camera;
    EventRecorder eventRecorder;

    Context()
        : camera{Camera::CreateOrthoByWindow(
              gWorld->cur_registry()->res<Window>().get())} {}
};

}  // namespace nickel::ui