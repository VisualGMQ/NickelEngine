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
    cgmath::Vec2 translation = cgmath::Vec2{};
    float rotation = 0;  // in degress
    cgmath::Vec2 scale = {1, 1};

    auto& SetRotation(float degrees) {
        rotation = degrees;
        return *this;
    }

    auto& SetScale(const cgmath::Vec2& scale) {
        this->scale = scale;
        return *this;
    }

    auto& SetTranslation(const cgmath::Vec2& trans) {
        translation = trans;
        return *this;
    }

    cgmath::Mat44 ToMat() const {
        return cgmath::CreateTranslation(
                   cgmath::Vec3{translation.x, translation.y, 0.0}) *
               cgmath::CreateZRotation(cgmath::Deg2Rad(rotation)) *
               cgmath::CreateScale(cgmath::Vec3{scale.x, scale.y, 1.0});
    }

    static Transform Create(const cgmath::Vec2& translation, float rotation,
                            const cgmath::Vec2& scale) {
        return {translation, rotation, scale};
    }

    static Transform FromTranslation(const cgmath::Vec2& position) {
        return {position};
    }

    static Transform FromRotation(float rotation) { return {{}, rotation}; }

    static Transform FromScale(const cgmath::Vec2& scale) {
        return {{}, 0, scale};
    }
};

}  // namespace nickel