#include "renderer/camera.hpp"

namespace nickel {

Camera2D::Camera2D(const geom::Cube& cube): cube_(cube) {
    project_ = cgmath::CreateOrtho(
        cube.center.x - cube.half_len.x, cube.center.x + cube.half_len.x,
        cube.center.y - cube.half_len.y, cube.center.y + cube.half_len.y,
        cube.center.z - cube.half_len.z, cube.center.z + cube.half_len.z);
}

}