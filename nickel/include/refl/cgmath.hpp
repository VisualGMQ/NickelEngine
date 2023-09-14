#pragma once

#include "core/cgmath.hpp"
#include "pch.hpp"

// clang-format off
#include "mirrow/srefl/srefl_begin.hpp"
srefl_class(cgmath::Vec2,
    ctors()
    fields(
        field(&cgmath::Vec2::x),
        field(&cgmath::Vec2::y)
    )
)

#include "mirrow/srefl/srefl_end.hpp"
    // clang-format on