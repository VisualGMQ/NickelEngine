#pragma once

#include "physics/config.hpp"

namespace nickel {

namespace physics {

struct Contact final {
    Vec2 point;
};

struct Manifold final {
    static constexpr int MaxContactNum = 2;

    enum class Type {
        Circles,
        FaceA,
        FaceB,
    } type;

    Contact contacts[MaxContactNum];
    int contactNum = 0;
    Vec2 normal;
    Vec2 tangent;
};

}

}