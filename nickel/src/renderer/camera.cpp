#include "renderer/camera.hpp"

namespace nickel {

Camera2D::Camera2D(float left, float right, float top, float bottom, float near,
                   float far) {
    SetProject(left, right, top, bottom, near, far);
}

void Camera2D::SetProject(float left, float right, float top, float bottom,
                          float near, float far) {
    project_ = cgmath::CreateOrtho(left, right, top, bottom, near, far);
    cube_.center.Set((left + right) / 2.0f, (top + bottom) / 2.0f,
                     (near + far) / 2.0f);
    cube_.halfLen.Set(std::abs((right - left) / 2.0f),
                      std::abs((top - bottom) / 2.0f),
                      std::abs((near - far) / 2.0f));
}

}  // namespace nickel