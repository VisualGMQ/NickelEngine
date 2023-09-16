#include "renderer/camera.hpp"

namespace nickel {

Camera2D::Camera2D(float left, float right, float top, float bottom, float near, float far) {
    project_ = cgmath::CreateOrtho(left, right, top, bottom, near, far);
    cube_.center.Set((left + right) / 2.0, (top + bottom) / 2.0, (near + far) / 2.0);
    cube_.half_len.Set(std::abs((right - left) / 2.0),
                       std::abs((top - bottom) / 2.0),
                       std::abs((near - far) / 2.0));
}

}