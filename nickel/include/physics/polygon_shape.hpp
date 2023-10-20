#pragma once

#include "geom/geom2d.hpp"
#include "physics/config.hpp"
#include "physics/shape.hpp"

namespace nickel {

namespace physics {

class PolygonShape : public Shape {
public:
    static PolygonShape From(const std::vector<Vec2>& pts) {
        return {pts};
    }

    static PolygonShape From(std::vector<Vec2>&& pts) {
        return {std::move(pts)};
    }

    std::vector<Vec2> shape;

private:
    PolygonShape(const std::vector<Vec2>& pts)
        : Shape(Type::Polygon), shape(pts) {}

    PolygonShape(std::vector<Vec2>&& pts)
        : Shape(Type::Polygon), shape(std::move(pts)) {}
};

}  // namespace physics

}  // namespace nickel