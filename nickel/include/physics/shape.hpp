#pragma once

#include "config/config.hpp"
#include "core/assert.hpp"
#include <type_traits>

namespace nickel {

namespace physics {

class Shape {
public:
    enum class Type {
        Circle,
        OBB,
        Polygon,
        Capsule,
    };

    virtual ~Shape() = default;

    Shape(Type type) : type_{type} {}

    Type GetType() const { return type_; }

private:
    Type type_;
};

class CollideShape final {
public:
    template <typename T,
              typename = std::enable_if<std::is_base_of_v<Shape, T>>>
    CollideShape(T&& s) : shape{new T{std::forward<T>(s)}} {}

    std::unique_ptr<Shape> shape;
};

}  // namespace physics

}  // namespace nickel