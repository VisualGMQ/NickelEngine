#pragma once

#include "cgmath.hpp"

namespace nickel {

namespace geom {

struct Cube final {
    cgmath::Vec3 center;
    cgmath::Vec3 half_len;

    static Cube CreateFromCenter(const cgmath::Vec3& center, const cgmath::Vec3& half_len) {
        return Cube{center, half_len};
    }

    static Cube CreateFromTopLeft(const cgmath::Vec3& topleft, const cgmath::Vec3& half_len) {
        return Cube{topleft + half_len, half_len};
    }
};

struct Rect final {
    cgmath::Vec2 center;
    cgmath::Vec2 half_len;
};

}

}