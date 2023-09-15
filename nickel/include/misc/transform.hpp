#pragma once

#include "core/cgmath.hpp"

namespace nickel {

enum Flip {
    None = 0,
    Horizontal = 0x01,
    Vertical = 0x02,
    Both = 0x03,
};

struct Transform final {
    cgmath::Vec2 position = cgmath::Vec2{};
    float rotation = 0;  // in degress
    cgmath::Vec2 scale = {1, 1};

    cgmath::Mat44 ToMat() const {
        return cgmath::CreateZRotation(cgmath::Deg2Rad(rotation)) *
               cgmath::CreateScale(cgmath::Vec3{scale.x, scale.y, 1.0}) *
               cgmath::CreateTranslation(
                   cgmath::Vec3{position.x, position.y, 0.0});
    }

    static Transform Create(const cgmath::Vec2& position, float rotation,
                            const cgmath::Vec2& scale) {
        return {position, rotation, scale};
    }

    static Transform FromPosition(const cgmath::Vec2& position) {
        return {position};
    }

    static Transform FromRotation(float rotation) { return {{}, rotation}; }

    static Transform FromScale(const cgmath::Vec2& scale) {
        return {{}, 0, scale};
    }
};

}  // namespace nickel