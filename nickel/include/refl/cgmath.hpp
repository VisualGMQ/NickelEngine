#pragma once

#include "pch.hpp"
#include "core/cgmath.hpp"

// clang-format off
#include "mirrow/srefl/srefl_begin.hpp"
srefl_class(::nickel::cgmath::Vec2,
    ctors()
    fields(
        field(&::nickel::cgmath::Vec2::x),
        field(&::nickel::cgmath::Vec2::y)
    )
)

srefl_class(::nickel::cgmath::Vec3,
    ctors()
    fields(
        field(&::nickel::cgmath::Vec3::x),
        field(&::nickel::cgmath::Vec3::y),
        field(&::nickel::cgmath::Vec3::z)
    )
)

srefl_class(::nickel::cgmath::Vec4,
    ctors()
    fields(
        field(&::nickel::cgmath::Vec4::x),
        field(&::nickel::cgmath::Vec4::y),
        field(&::nickel::cgmath::Vec4::z),
        field(&::nickel::cgmath::Vec4::w)
    )
)

srefl_class(::nickel::cgmath::Rect,
    ctors()
    fields(
        field(&::nickel::cgmath::Rect::position),
        field(&::nickel::cgmath::Rect::size)
    )
)

#include "mirrow/srefl/srefl_end.hpp"
// clang-format on